#include "editor.h"
#include <QDebug>
#include <QPainter>
#include <QMouseEvent>
//#include <algorithm>
#include <cmath>

constexpr uchar Editor::dotData[];

Editor::Editor(QWidget *parent) : QWidget(parent)
{
}

void Editor::setBuffer(const std::vector<float>& buf)
{
    buffer = MinMaxTree(buf.cbegin(), buf.cend());
    tags = TagTree();
    tags.add(1.0,2.0);
    tags.add(3.0,4.0);
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
    p.end();
}

void Editor::keyPressEvent(QKeyEvent* event)
{
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

void Editor::stopTagging()
{
    tagging = false;
    tags.add(tagBound1, tagBound2);
    update();
}

void Editor::stopErasing()
{
    erasing = false;
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
