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
    filePath = QDir::currentPath();
    labelsPath = QDir::currentPath();

    QAudioFormat desiredFormat;
    desiredFormat.setChannelCount(1);
    desiredFormat.setCodec("audio/x-raw");
    desiredFormat.setSampleType(QAudioFormat::Float);
    desiredFormat.setSampleRate(44100);
    desiredFormat.setSampleSize(32);
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
    connect(ui->actionOpenLabels, SIGNAL(triggered()),
            this, SLOT(openLabelsDialog()));
    connect(ui->actionSaveLabels, SIGNAL(triggered()),
            this, SLOT(openSaveDialog()));
    connect(ui->silenceRegions, SIGNAL(toggled(bool)),
            ui->editor, SLOT(setSilenceRegions(bool)));
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
                                                    filePath,
                                                    tr("Audio Files (*.flac);;"
                                                       "All Files (*)"));
    if (fileName.isEmpty()) return;
    filePath = fileName;
    open(fileName);
}

void MainWindow::openLabelsDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Labels File"),
                                                    labelsPath,
                                                    tr("Label Files (*.txt);;"
                                                       "All Files (*)"));
    if (fileName.isEmpty()) return;
    labelsPath = fileName;
    ui->editor->openLabels(fileName);
}

void MainWindow::openSaveDialog()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Labels"),
                                                    labelsPath,
                                                    tr("Label Files (*.txt);;"
                                                       "All Files (*)"));
    if (fileName.isEmpty()) return;
    labelsPath = fileName;
    ui->editor->saveLabels(fileName);
}

void MainWindow::readBuffer()
{
    QAudioBuffer buffer = decoder->read();
    const float *data = buffer.constData<float>();
    for (int i = 0; i < buffer.frameCount(); ++i) {
        tmpBuffer.push_back(data[i]);
    }
 }

void MainWindow::decodeError(QAudioDecoder::Error error)
{
    qDebug() << error;
    qDebug() << decoder->errorString();
}

void MainWindow::decodeFinished()
{
    qDebug() << "decoding done";
    ui->editor->setBuffer(std::move(tmpBuffer));
    qDebug() << "done";
}
