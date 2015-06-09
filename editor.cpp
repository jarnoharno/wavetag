#include "editor.h"
#include <QDebug>
#include <QPainter>
#include <algorithm>

Editor::Editor(QWidget *parent) : QWidget(parent)
{
    /*
    pen.setStyle(Qt::DashDotLine);
    pen.setWidth(3);
    pen.setBrush(Qt::green);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    */
}

void Editor::setBuffer(QVector<float> buf)
{
    buffer = buf;
    update();
}

void Editor::paintEvent(QPaintEvent *event)
{
    if (buffer.empty()) return;
    QPainter p;
    p.begin(this);
    int64_t w = p.device()->width();
    int64_t h = p.device()->height();
    int64_t len = buffer.length();
    for (int64_t x = 0; x < w; ++x) {
        int x1 = x*len/w;
        int x2 = (x+1)*len/w;
        if (x1 == x2) continue;
        float y1 = buffer[x1];
        float y2 = buffer[x1];
        for (int z = x1+1; z < x2; ++z) {
            y1 = std::min(y1,buffer[z]);
            y2 = std::max(y2,buffer[z]);
        }
        int z1 = (1.0f-y1)*h/2;
        int z2 = (1.0f-y2)*h/2;
        p.fillRect(x,z1,1,z2-z1,brush);
    }
    p.end();
}
