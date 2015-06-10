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
        int i1 = n*t1/t;
        int i2 = n*t2/t;
        if (i1 == i2) continue;

        const MinMax<float>& mm = buffer.range(i1,i2);
        float y1 = mm.mn;
        float y2 = mm.mx;
        /*
        float y1 = buffer[i1];
        float y2 = buffer[i1];
        for (int j = i1+1; j < i2; ++j) {
            y1 = std::min(y1,buffer[j]);
            y2 = std::max(y2,buffer[j]);
        }
        */

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
    p.end();
}

void Editor::mousePressEvent(QMouseEvent* event)
{
    oldX = event->x();
}

void Editor::mouseReleaseEvent(QMouseEvent* event)
{
}

void Editor::mouseMoveEvent(QMouseEvent* event)
{
    int x = event->x();
    int w = width();
    clipStart -= (x - oldX)*clipLength/w;
    oldX = x;

    // check bounds
    clipStart = std::max(0.f,clipStart);
    int n = buffer.length();
    float t = n/44100.f;
    clipStart = std::min(t-clipLength,clipStart);

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
