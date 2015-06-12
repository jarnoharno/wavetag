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
#include <vector>

class Editor : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = 0);
    void setBuffer(const std::vector<float>& buf);

signals:

public slots:
    void setLines(bool b);
    void setDots(bool b);
    void setExtrema(bool b);

protected:
    void paintEvent(QPaintEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void keyReleaseEvent(QKeyEvent* event);

private:
    float clipStart;
    float clipLength;

    typedef SegTree<MinMax<float>> MinMaxTree;

    MinMaxTree buffer;

    const QBrush brush = QBrush(QColor(0,0,0));
    const QPen pen = QPen();
    const float zoom = 0.1f;
    const int dotSpaceThreshold = 4;

    int oldX;

    bool drawLines = true;
    bool drawDots = true;
    bool drawExtrema = true;

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
    typedef IntervalTree<float> TagTree;
    TagTree tags;

    void stopTagging();
    void stopErasing();
};

#endif // EDITOR_H
