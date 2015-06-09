#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QVector>
#include <QBrush>

class Editor : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = 0);
    void setBuffer(QVector<float> buf);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent* event);

private:
    QVector<float> buffer;
    QBrush brush = QBrush(QColor(0,0,0));
};

#endif // EDITOR_H
