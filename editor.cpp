#include "editor.h"

Editor::Editor(QWidget *parent) : QWidget(parent)
{

}

void Editor::setBuffer(QVector<float> buf)
{
    buffer = buf;
    update();
}

