#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioDecoder>
#include <QVector>
#include <limits>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void open(QString fn);

private slots:
    void openFileDialog();
    void readBuffer();
    void decodeError(QAudioDecoder::Error error);
    void decodeFinished();

private:
    Ui::MainWindow *ui;
    QAudioDecoder *decoder;
    QVector<float> tmpBuffer;
    static const float scale = 1.0f / (1 << 15);
};

#endif // MAINWINDOW_H
