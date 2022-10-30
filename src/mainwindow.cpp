#include "mainwindow.h"

#include <opencv2/opencv.hpp>
#include <QMenu>
#include <QMetaType>
#include <QMouseEvent>

#include "common/logging.h"
#include "common/time.h"
#include "image.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , pressed_(false)
    , lastEffect_(Face)
    , cameraServer_(0)
{
    ui->setupUi(this);
    registerType();
    initUi();
    resize(480, 640);
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
    common::log::initLogger();

    webServer_ = new WebsocketServer;
    webServer_->start(9005);

    cameraClient_ = new PlayCamera;
    cameraClient_->start();
    facedetect_ = new FaceDetect;
    facedetect_->start();
    hahaCore_ = new Hahacore;
    hahaCore_->setFaceDetectObject(facedetect_);
    hahaCore_->start();
    faceIdentify_ = new FaceIdentify;
    faceIdentify_->setFaceDetectObject(facedetect_);
    faceIdentify_->setWebsocketServerOject(webServer_);
    faceIdentify_->setHttpUrl("http://exhibit-pre.ubtrobot.com:60901/api/v1/faceIdentify");
    faceIdentify_->start();

    hahaUi_ = new HahaUi(this);
    hahaUi_->setResolution(cv::Size(480, 640));

    cameraServer_ = new ProducerRecordImpl;
    cameraServer_->startServer();
    cameraServer_->registerRecordConsumer(cameraClient_);
    cameraServer_->registerRecordConsumer(facedetect_);
    cameraServer_->registerRecordConsumer(hahaCore_);
    cameraServer_->registerRecordConsumer(faceIdentify_);
    // cameraServer_->setHahaUi(hahaUi_);
    initConnect();
}

void MainWindow::initConnect()
{
    connect(cameraClient_, &PlayCamera::sig_getHahaImage, this, &MainWindow::slot_getHahaImage);
    //    connect(cameraClient_,
    //            &PlayCamera::sig_getHahaImage1,
    //            this,
    //            &MainWindow::slot_getHahaImage1,
    //            Qt::QueuedConnection);
    connect(hahaCore_,
            &Hahacore::sig_sendHahaMat,
            this,
            &MainWindow::slot_getHahaImage1,
            Qt::QueuedConnection);
    connect(facedetect_,
            &FaceDetect::sig_faceCountChanged,
            this,
            &MainWindow::slot_faceCountChanged);
}

void MainWindow::start()
{
    init();
}

void MainWindow::initUi()
{
    setAttribute(Qt::WA_TranslucentBackground); //背景透明
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    initMenu();
    // showMaximized();

    // ui->cbx_face_effect->addItems(QStringList() << "eyes"
    //                                                << "mouth"
    //                                                << "face");

    // ui->cbx_face_effect->installEventFilter(this);
}

void MainWindow::initMenu()
{
    rootMenu_ = new QMenu(this);
    hahaMenu_ = rootMenu_->addMenu("特效");
    QAction *face_action = hahaMenu_->addAction("脸颊");
    face_action->setObjectName("face");
    QAction *mouth_action = hahaMenu_->addAction("嘴巴");
    mouth_action->setObjectName("mouth");
    QAction *eyes_action = hahaMenu_->addAction("眼睛");
    eyes_action->setObjectName("eyes");
    QAction *none_action = hahaMenu_->addAction("无");
    none_action->setObjectName("none");
    closeAction_ = rootMenu_->addAction("exit");
    modeMenu_ = rootMenu_->addMenu("模式");

    connect(face_action, &QAction::triggered, this, &MainWindow::slot_setHahaEffect);
    connect(mouth_action, &QAction::triggered, this, &MainWindow::slot_setHahaEffect);
    connect(eyes_action, &QAction::triggered, this, &MainWindow::slot_setHahaEffect);
    connect(none_action, &QAction::triggered, this, &MainWindow::slot_setHahaEffect);
    connect(closeAction_, &QAction::triggered, this, &MainWindow::slot_closeWindow);
}

void MainWindow::stop()
{
    cameraServer_->stopServer();
    cameraClient_->stop();
    facedetect_->stop();
    faceIdentify_->stop();
    hahaCore_->stop();
    webServer_->stop();
}

void MainWindow::slot_getHahaImage(QImage img)
{
    ui->lbl_video->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::slot_faceCountChanged(int cur, int last)
{
    if (lastEffect_ == None || cur < last)
    {
        return;
    }

    time(NULL);
    int e = rand() % 3;
    while (e == lastEffect_)
    {
        e = rand() % 3;
    }

    if (hahaCore_)
    {
        hahaCore_->setHahaEffect((HahaEffect) e);
    }

    // LOG_DEBUG("effect: {}", e);

    lastEffect_ = e;
}

void MainWindow::fillRects(cv::Mat mat, std::vector<FaceDetectResult> &rects)
{
    for (auto it = rects.begin(); it != rects.end(); ++it)
    {
        cv::rectangle(mat, it->bigFaceRect, cv::Scalar(115, 210, 22), 2);
        // cv::rectangle(mat, it->algorithmFaceRect, cv::Scalar(115, 210, 22), 2);
    }
}

void MainWindow::slot_setHahaEffect()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QString effect = action->objectName();

    HahaEffect e_effect;
    if (effect == "face")
    {
        e_effect = Face;
    }
    else if (effect == "eyes")
    {
        e_effect = Eyes;
    }
    else if (effect == "mouth")
    {
        e_effect = Mouth;
    }
    else if (effect == "none")
    {
        e_effect = None;
    }

    if (hahaCore_)
    {
        hahaCore_->setHahaEffect(e_effect);
    }

    //   LOG_DEBUG("effect: {}", e_effect);

    lastEffect_ = e_effect;
}

void MainWindow::slot_getHahaImage1(cv::Mat mat)
{
    auto rects = facedetect_->getCurrentPersonResults();
    fillRects(mat, rects);

    std::string rate = common::time::caculateFPS();
    if (rate != "")
    {
        LOG_TRACE("MainWindow Fps: {}!!", rate);
    }

    QImage img = image::mat2qim(mat);
    QPixmap pix = QPixmap::fromImage(img);
    ui->lbl_video->setPixmap(pix);
    mat.release();

    cv::Rect rect;
    rect.empty();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (pressed_)
        move(event->pos() - m_point + pos());
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    pressed_ = false;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        pressed_ = true;
        m_point = event->pos();
        rootMenu_->close();
    }
    else if (event->button() == Qt::RightButton)
    {
        rootMenu_->exec(QCursor::pos());
    }
}

void MainWindow::slot_closeWindow()
{
    stop();
    close();
}

//bool MainWindow::eventFilter(QObject *obj, QEvent *event)
//{
//    if (obj == ui->cbx_face_effect)
//    {
//        if (event->type() == QEvent::MouseButtonPress)
//        {
//            QMouseEvent *mevent = dynamic_cast<QMouseEvent *>(event);
//            if (mevent->button() == Qt::LeftButton)
//            {
//                if (hahaCore_)
//                {
//                    if (ui->cbx_face_effect)
//                    hahaCore_->setHahaEffect()
//                }
//            }
//            else
//            {
//            }
//            return true;
//        }
//        else
//        {
//            return false;
//        }
//    }
//    else
//    {
//        // pass the event on to the parent class
//        return QMainWindow::eventFilter(obj, event);
//    }
// }
