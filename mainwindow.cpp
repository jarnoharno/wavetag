#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QFileInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    decoder = new QAudioDecoder(this);

    QAudioFormat desiredFormat;
    desiredFormat.setChannelCount(1);
    desiredFormat.setCodec("audio/x-raw");
    desiredFormat.setSampleType(QAudioFormat::SignedInt);
    desiredFormat.setSampleRate(44100);
    desiredFormat.setSampleSize(16);
    decoder->setAudioFormat(desiredFormat);

    connect(decoder, SIGNAL(bufferReady()),
            this, SLOT(readBuffer()));
    connect(decoder, SIGNAL(error(QAudioDecoder::Error)),
            this, SLOT(decodeError(QAudioDecoder::Error)));
    connect(decoder, SIGNAL(finished()),
            this, SLOT(decodeFinished()));

    ui->setupUi(this);
    connect(ui->actionOpen, SIGNAL(triggered()),
            this, SLOT(openFileDialog()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open(QString fn)
{
    QString file = QFileInfo(fn).absoluteFilePath();
    decoder->setSourceFilename(file);
    tmpBuffer.resize(0);
    decoder->start();
}

void MainWindow::openFileDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Audio File"),
                                                    QDir::currentPath(),
                                                    tr("Audio Files (*)"));
    qDebug() << fileName;
    open(fileName);
}

void MainWindow::readBuffer()
{
    QAudioBuffer buffer = decoder->read();
    const int16_t *data = buffer.constData<int16_t>();
    for (int i = 0; i < buffer.frameCount(); ++i) {
        float x = data[i]*scale;
        tmpBuffer.push_back(x);
    }
 }

void MainWindow::decodeError(QAudioDecoder::Error error)
{
    qDebug() << error;
    qDebug() << decoder->errorString();
}

void MainWindow::decodeFinished()
{
    ui->editor->setBuffer(tmpBuffer);
    qDebug() << "done"
}
