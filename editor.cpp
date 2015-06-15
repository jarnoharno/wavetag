#include "editor.h"
#include "util.h"
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QTimer>
#include <cmath>

constexpr uchar Editor::dotData[];

AudioDevice::AudioDevice(Editor* editor):
    QIODevice(editor), editor(editor)
{}

qint64 AudioDevice::readData(char *data, qint64 maxlen)
{
    if (!editor->playing) return 0;
    qint64 rlen = std::min(maxlen,len-cursor);
    memcpy(data, start+cursor, rlen);
    // handle tagged regions
    float t1 = (cursor/4)/44100.f;
    float t2 = ((cursor+rlen)/4)/44100.f;
    if (editor->silenceRegions) {
        for (auto r: editor->tags.overlapping(t1,t2)) {
            int i1 = std::max(cursor,static_cast<qint64>(r.left*44100l)*4l);
            int i2 = std::min(cursor+rlen,static_cast<qint64>(r.right*44100l)*4l);
            memset(data+i1-cursor,0,i2-i1);
        }
    } else {
        IntervalTree<int> neg;
        neg.add(cursor,cursor+rlen);
        for (auto r: editor->tags.overlapping(t1,t2)) {
            int i1 = static_cast<int>(r.left*44100l)*4l;
            int i2 = static_cast<int>(r.right*44100l)*4l;
            neg.subtract(i1,i2);
        }
        for (auto r: neg) {
            memset(data+r.left-cursor,0,r.right-r.left);
        }
    }
    cursor += rlen;
    return rlen;
}

bool AudioDevice::atEnd() const
{
    return cursor == len;
}

void AudioDevice::setBuffer(float* buf, qint64 len)
{
    this->start = reinterpret_cast<char*>(buf);
    this->len = len*4;
    this->cursor = 0;
}

qint64 AudioDevice::writeData(const char *data, qint64 len)
{
    return -1;
}

void Editor::setSilenceRegions(bool b)
{
    silenceRegions = b;
}

void Editor::start()
{
    cursor = visCursor*44100.f;
    if (audioDevice->atEnd()) return;
    playing = true;
    audio->start(audioDevice);
}

void Editor::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Z) {
        switch (audio->state()) {
        case QAudio::ActiveState:
        case QAudio::IdleState:
            playing = false;
            break;
        case QAudio::SuspendedState:
        case QAudio::StoppedState:
            start();
            break;
        }
    }
}

void Editor::handleStateChanged(QAudio::State state)
{
    qDebug() << state;
    qDebug() << audio->error();
    switch (state) {
    case QAudio::ActiveState:
        timer->start(25);
        break;
    case QAudio::IdleState:
        if (audioDevice->atEnd()) audioDevice->cursor = 0;
        visCursor = (audioDevice->cursor/4)/44100.f;
        timer->stop();
        audio->stop();
        prevStart = visCursor;
        update();
        break;
    case QAudio::SuspendedState:
    case QAudio::StoppedState:
        break;
    }
}

void Editor::handleTimer()
{
    if (audio->state() == QAudio::ActiveState ||
        audio->state() == QAudio::IdleState) {
        visCursor = prevStart + audio->elapsedUSecs()/1e6f;
        update();
    }
}

Editor::Editor(QWidget *parent) : QWidget(parent)
{
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(32);
    format.setCodec("audio/pcm");
    format.setSampleType(QAudioFormat::Float);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning() << "Raw audio format not supported by backend, cannot play audio.";
        return;
    }

    audio = new QAudioOutput(format, this);
    connect(audio, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(handleStateChanged(QAudio::State)),
            Qt::QueuedConnection);
    audio->setNotifyInterval(25);

    audioDevice = new AudioDevice(this);
    audioDevice->open(QIODevice::ReadOnly);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(handleTimer()));
}

void Editor::saveLabels(QString fn)
{
    QFile file(fn);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Wavetag"),
                              tr("Unable to open") + '\"' + fn + '\"');
        return;
    }
    QTextStream out(&file);
    int i = 0;
    for (auto r: tags) {
        out << r.left << '\t' << r.right << '\t' << ++i << '\n';
    }
}

void Editor::openLabels(QString fn)
{
    QFile file(fn);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, tr("Wavetag"),
                              tr("Unable to open") + '\"' + fn + '\"');
        return;
    }
    if (!parseLabels(&file)) {
        QMessageBox::critical(this, tr("Wavetag"),
                              tr("Unable to parse") + '\"' + fn + '\"');
    }
}

bool Editor::parseLabels(QFile *file)
{
    QTextStream in(file);
    TagTree tmp;
    while (!in.atEnd()) {
        QString line = in.readLine();
        auto words = line.split(QRegExp("\\s+"));
        if (words.length() == 0) continue;
        if (words.length() < 2) return false;
        bool ok = true;
        float a = words[0].toFloat(&ok);
        if (!ok) return false;
        float b = words[1].toFloat(&ok);
        if (!ok) return false;
        tmp.add(a, b);
    }
    tags = tmp;
    return true;
}



void Editor::setBuffer(std::vector<float>&& buf)
{
    buffer = MinMaxTree(buf.cbegin(), buf.cend());
    audioBuffer = buf;
    tags = TagTree();
    cursor = 0;
    visCursor = 0.f;
    prevStart = 0.f;
    audioDevice->setBuffer(&audioBuffer[0],audioBuffer.size());
    clipStart = 0.f;
    clipLength = buffer.length()/44100.f;
    update();
}

void Editor::setLines(bool b)
{
    drawLines = b;
    update();
}

void Editor::setDots(bool b)
{
    drawDots = b;
    update();
}

void Editor::setExtrema(bool b)
{
    drawExtrema = b;
    update();
}

void Editor::paintEvent(QPaintEvent *event)
{
    if (buffer.empty()) return;
    QPainter p;
    p.begin(this);
    p.setPen(pen);
    int64_t n = buffer.length();
    float t = n/44100.f;
    int64_t w = p.device()->width();
    float h2 = p.device()->height()/2;

    int x3 = 0;
    int z3 = (1.0f-buffer[0].mn)*h2;

    int space = w/(clipLength*44100.f);

    for (int64_t x = 0; x < w; ++x) {
        float t1 = clipLength*x/w + clipStart;
        float t2 = clipLength*(x+1)/w + clipStart;
        int i1 = t1*44100;
        int i2 = t2*44100;
        if (i1 == i2) continue;

        auto mm = buffer.range(i1,i2);
        float y1 = mm.mn;
        float y2 = mm.mx;

        p.setPen(Qt::black);
        // draw connecting line
        int z0 = (1.f-buffer[i1].mn)*h2;
        if (drawLines) accurateDrawLine(p,x3,z3,x,z0);
        x3 = x;
        z3 = (1.f-buffer[i2-1].mn)*h2;

        // draw extrema
        int z1 = (1.f-y1)*h2;
        int z2 = (1.f-y2)*h2;
        if (drawExtrema) p.drawLine(x,z1,x,z2);

        // draw dot
        if (i1+1 == i2 && space >= dotSpaceThreshold) {
            if (drawDots) p.drawPixmap(x-2,z1-2,dot);
        }

    }
    p.setCompositionMode(QPainter::CompositionMode_Difference);
    int64_t h = p.device()->height();

    // draw all tags
    for (auto i : tags.overlapping(clipStart,clipStart+clipLength)) {
        int x1 = w*(i.left-clipStart)/clipLength;
        int x2 = w*(i.right-clipStart)/clipLength;
        p.fillRect(x1,0,std::max(x2-x1,1),h,QBrush(Qt::gray));
    }
    // draw tagging interval
    if (tagging) {
        int x1 = w*(tagBound1-clipStart)/clipLength;
        int x2 = w*(tagBound2-clipStart)/clipLength;
        if (x1 > x2) std::swap(x1,x2);
        p.fillRect(x1,0,std::max(x2-x1,1),h,QBrush(Qt::darkGreen));
    } else if (erasing) {
        int x1 = w*(tagBound1-clipStart)/clipLength;
        int x2 = w*(tagBound2-clipStart)/clipLength;
        if (x1 > x2) std::swap(x1,x2);
        p.fillRect(x1,0,std::max(x2-x1,1),h,QBrush(Qt::darkRed));
    }

    // draw cursor
    int c = w*(visCursor-clipStart)/clipLength;
    if (c >= 0 && c < w) {
        p.setPen(Qt::red);
        p.drawLine(c,0,c,h);
    }

    p.end();
}

void Editor::mousePressEvent(QMouseEvent* event)
{
    oldX = event->x();
    int w = width();
    // tagging is only initiated here
    if (event->modifiers() & Qt::ControlModifier) {
        tagging = true;
        tagBound1 = clipStart + clipLength*oldX/w;
    } else if (event->modifiers() & Qt::AltModifier) {
        erasing = true;
        tagBound1 = clipStart + clipLength*oldX/w;
    }
}

void Editor::swapClipTags()
{
    if (tagBound1 > tagBound2) {
        std::swap(tagBound1,tagBound2);
    }
    if (tagBound1 < clipStart) {
        tagBound1 = clipStart;
    }
    float clipEnd = clipStart + clipLength;
    if (tagBound2 > clipEnd) {
        tagBound2 = clipEnd;
    }
}

void Editor::stopTagging()
{
    tagging = false;
    swapClipTags();
    tags.add(tagBound1, tagBound2);
    update();
}

void Editor::stopErasing()
{
    erasing = false;
    swapClipTags();
    tags.subtract(tagBound1, tagBound2);
    update();
}


void Editor::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Control && tagging) {
        stopTagging();
    } else if (event->key() == Qt::Key_Alt && erasing) {
        stopErasing();
    }
}

void Editor::mouseReleaseEvent(QMouseEvent* event)
{
    if (tagging) {
        stopTagging();
    } else if (erasing) {
        stopErasing();
    }
}

void Editor::mouseMoveEvent(QMouseEvent* event)
{
    int x = event->x();
    int w = width();
    if (tagging) {
        tagBound2 = clipStart + clipLength*x/w;
    } else if (erasing) {
        tagBound2 = clipStart + clipLength*x/w;
    } else {
        clipStart -= (x - oldX)*clipLength/w;
        // check bounds
        clipStart = std::max(0.f,clipStart);
        int n = buffer.length();
        float t = n/44100.f;
        clipStart = std::min(t-clipLength,clipStart);
    }
    oldX = x;
    update();
}

void Editor::wheelEvent(QWheelEvent* event)
{
    int d = event->delta() > 0 ? 1 : -1;
    int x = event->x();
    int w = width();
    clipStart += x*clipLength*d*zoom/w;
    clipLength *= (1.0f - d*zoom);

    // check bounds
    if (clipStart < 0.f) {
        clipLength += clipStart;
        clipStart = 0.f;
    }
    int n = buffer.length();
    float t = n/44100.f;
    if (clipStart + clipLength > t) {
        clipLength = t - clipStart;
    }

    update();
}

void Editor::accurateDrawLine(QPainter& p, int x1, int y1, int x2, int y2)
{
    using namespace std;
    if (x1 > x2) {
        swap(x1,x2);
        swap(y1,y2);
    } else if (x1 == x2 && y1 > y2) {
        swap (y1,y2);
    }
    if (y2 >= y1) {
        p.drawLine(x1,y1,x2,y2);
    } else if (y1-y2 <= x2-x1) {
        p.drawLine(x1,y1+1,x2,y2+1);
    } else {
        p.drawLine(x1+1,y1,x2+1,y2);
    }
}
