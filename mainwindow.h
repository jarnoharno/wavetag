#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAudioDecoder>
#include <QVector>
#include <limits>
#include <vector>

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
    std::vector<float> tmpBuffer;
};

#endif // MAINWINDOW_H
