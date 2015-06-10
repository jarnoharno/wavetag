#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QVector>
#include <QBrush>
#include <QPen>
#include <QPainter>
#include <QBitmap>

class Editor : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = 0);
    void setBuffer(QVector<float> buf);

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

private:
    float clipStart;
    float clipLength;
    QVector<float> buffer;

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

};

#endif // EDITOR_H
