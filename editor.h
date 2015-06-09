#ifndef EDITOR_H
#define EDITOR_H

#include <QWidget>
#include <QVector>

class Editor : public QWidget
{
    Q_OBJECT
public:
    explicit Editor(QWidget *parent = 0);
    void setBuffer(QVector<float> buf);

signals:

public slots:

private:
    QVector<float> buffer;
};

#endif // EDITOR_H
