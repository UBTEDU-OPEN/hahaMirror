#include "mainwindow.h"
#include "common/logging.h"
#include "image.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QMetaType>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cameraServer_(0)
{
    ui->setupUi(this);
    start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::registerType()
{
    qRegisterMetaType<cv::Mat>("cv::Mat");
}

void MainWindow::init()
{
    registerType();
    common::log::initLogger();

    webServer_ = new WebsocketServer;
    webServer_->startVirtualServer(9005);

    cameraClient_.start();
    facedetect_.start();

    cameraServer_.startServer();
    cameraServer_.registerRecordConsumer(&cameraClient_);
    cameraServer_.registerRecordConsumer(&facedetect_);
    initConnect();
}

void MainWindow::initConnect()
{
    connect(&cameraClient_, &PlayCamera::sig_getHahaImage, this, &MainWindow::slot_getHahaImage);
    connect(&cameraClient_, &PlayCamera::sig_getHahaImage1, this, &MainWindow::slot_getHahaImage1);
}

void MainWindow::start()
{
    init();
}

void MainWindow::stop()
{
    cameraServer_.stopServer();
    cameraClient_.stop();
}

void MainWindow::slot_getHahaImage(QImage img)
{
    ui->lbl_video->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::slot_getHahaImage1(cv::Mat mat)
{
    QImage img = image::mat2qim(mat);
    QPixmap pix = QPixmap::fromImage(img);
    ui->lbl_video->setPixmap(pix);
    mat.release();

    cv::Rect rect;
    rect.empty();
}
