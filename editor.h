#ifndef EDITOR_H
#define EDITOR_H

#include "segtree.h"
#include "intervaltree.h"
#include <QWidget>
#include <QVector>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QBitmap>
#include <QFile>
#include <QAudioOutput>
#include <QTimer>
#include <vector>

class Editor;

class AudioDevice : public QIODevice
{
    Q_OBJECT

public:
    AudioDevice(Editor* editor);
    void setBuffer(float* buf, qint64 len);
    void setCursor(qint64 c);
    bool atEnd() const;
    qint64 cursor = 0;

private:
    qint64 len = 0;
    char* start = 0;
    Editor* editor;

protected:
    qint64 readData(char *data, qint64 maxlen);
    qint64 writeData(const char *data, qint64 len);
};

class Editor : public QWidget
{
    Q_OBJECT

public:
    typedef IntervalTree<float> TagTree;

    explicit Editor(QWidget *parent = 0);
    void setBuffer(std::vector<float>&& buf);
    void saveLabels(QString fn);
    void openLabels(QString fn);

    bool playing = false;
    bool silenceRegions = true;
    TagTree tags;

signals:

public slots:
    void setSilenceRegions(bool b);
    void start();

private slots:
    void handleStateChanged(QAudio::State state);
    void handleTimer();

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    QTimer* timer;

    float clipStart;
    float clipLength;

    typedef SegTree<MinMax<float>> MinMaxTree;

    MinMaxTree buffer;
    std::vector<float> audioBuffer;

    const float zoom = 0.1f;
    const int dotSpaceThreshold = 4;

    int oldX;

    float angle = 0.f;

    void accurateDrawLine(QPainter& p, int x1, int y1, int x2, int y2);

    static constexpr uchar dotData[] = {
        0b00000100,
        0b00001110,
        0b00011111,
        0b00001110,
        0b00000100
    };
    const QBitmap dot = QBitmap::fromData(QSize(5,5),dotData);

    void swapClipTags();
    float tagBound1;
    float tagBound2;
    bool tagging = false;
    bool erasing = false;
    bool rewind = false;
    float rewindCursor = 0.f;

    void stopRewind(QMouseEvent* event);
    void stopTagging();
    void stopErasing();

    bool parseLabels(QFile* file);

    float visCursor = 0.f;
    float prevStart = 0.f;

    int cursor = 0;
    void pushBytes();

    QIODevice* pushAudio;
    QAudioOutput* audio;
    AudioDevice* audioDevice;

    // minimum distance between labels
    int rulerMinSpacing = 50;

};

#endif // EDITOR_H
