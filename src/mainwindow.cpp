#include "mainwindow.h"

#include <opencv2/opencv.hpp>
#include <QDesktopWidget>
#include <QMenu>
#include <QMetaType>
#include <QMouseEvent>
#include <QScreen>
#include <QTimer>

#include "common/logging.h"
#include "common/time.h"
#include "image.h"
#include "ui_mainwindow.h"

#include <QPainter>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , pressed_(false)
    , lastEffect_(Face)
    , resolution_(cv::Size{1080, 1920})
    , cameraServer_(nullptr)
    , sleepStatus_(Sleeping)
{
    ui->setupUi(this);
    common::log::initLogger();
    registerType();
    initUi();

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
    resolutionObject_ = new Resolution(resolution_);

    webServer_ = new WebsocketServer;
    webServer_->start(9004);

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

    hahaUi_ = new HahaUi;
    // hahaUi_->setSleepStatus(false);
    //hahaUi_->setResolutionObject(resolutionObject_);

    cameraServer_ = new ProducerRecordImpl;
    cameraServer_->startServer();
    cameraServer_->registerRecordConsumer(cameraClient_);
    cameraServer_->registerRecordConsumer(facedetect_);
    cameraServer_->registerRecordConsumer(hahaCore_);
    cameraServer_->registerRecordConsumer(faceIdentify_);
    cameraServer_->setHahaUi(hahaUi_);

    sleepTimer_ = new QTimer(this);
    sleepTimer_->setInterval(5 * 1000);
    sleepTimer_->start();

    showTimer_ = new QTimer(this);
    showTimer_->setInterval(1000 / 24);
    showTimer_->start();

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
    connect(sleepTimer_, &QTimer::timeout, this, &MainWindow::slot_uiSleep);
    connect(showTimer_, &QTimer::timeout, this, &MainWindow::slot_showtimeout);
}

void MainWindow::start()
{
    init();
}

void MainWindow::initUi()
{
    setAttribute(Qt::WA_TranslucentBackground); //背景透明
    setWindowFlags(Qt::FramelessWindowHint | windowFlags());

    selectScreen();
    initMenu();
}

void MainWindow::selectScreen()
{
    int count = QGuiApplication::screens().count();
    QRect geometry;
    QScreen *mScreen;

    for (int i = 0; i < count; ++i)
    {
        mScreen = QGuiApplication::screens()[i];
        geometry = mScreen->geometry();
        if (geometry.width() > geometry.height())
        {
            if (i == count - 1)
            {
                LOG_ERROR("please adjust screen resolution!!");
                return;
            }
        }
        else
        {
            deskRect_ = geometry;
            setGeometry(geometry);
            //        LOG_DEBUG("x:{},  y:{} , w:{}, h:{}",
            //                  geometry.x(),
            //                  geometry.y(),
            //                  geometry.width(),
            //                  geometry.height());
            return;
        }
    }
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
    hahaCore_->stop();
    faceIdentify_->stop();
    webServer_->stop();

    sleepTimer_->stop();
}

void MainWindow::slot_uiSleep()
{
    faceIdentify_->setDetectStatus(false);
    statusMutex_.lock();
    sleepStatus_ = Sleeping;
    statusMutex_.unlock();
}

void MainWindow::slot_getHahaImage(QImage img)
{
    ui->lbl_video->setPixmap(QPixmap::fromImage(img));
}

void MainWindow::slot_faceCountChanged(int cur, int last)
{
    // LOG_DEBUG("slot_faceCountChanged");
    if (cur == 0)
    {
        if (!sleepTimer_->isActive())
        {
            sleepTimer_->start();
        }
    }
    else
    {
        if (sleepTimer_->isActive())
        {
            sleepTimer_->stop();
        }

        statusMutex_.lock();
        if (sleepStatus_ == Sleeping)
        {
            sleepStatus_ = Waving;
            hahaUi_->setCurrentMirrorBrokenIndex(0);
            statusMutex_.unlock();
            faceIdentify_->setDetectStatus(true);
        }
        else
        {
            statusMutex_.unlock();
        }
    }

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
        hahaUi_->changeRobotStrategy();
    }

    //  LOG_DEBUG("effect: {}, cur: {}, last: {}", e, cur, last);

    lastEffect_ = e;
}

void MainWindow::fillRects(cv::Mat mat, std::vector<FaceDetectResult> &rects)
{
    for (auto it = rects.begin(); it != rects.end(); ++it)
    {
        //  cv::rectangle(mat, it->bigFaceRect, cv::Scalar(115, 210, 22), 2);
        //  cv::rectangle(mat, it->algorithmFaceRect, cv::Scalar(115, 210, 22), 2);
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

void MainWindow::checkUiStatus()
{
    statusMutex_.lock();
    SleepStatus status = sleepStatus_;
    statusMutex_.unlock();

    if (status == Waving)
    {
        static int lastIndex = 0;
        int index = hahaUi_->getMirrorBrokenIndex();
        if (index < lastIndex)
        {
            statusMutex_.lock();
            sleepStatus_ = Waved;
            statusMutex_.unlock();
            lastIndex = 0;

            hahaUi_->setCurrentTipAppearIndex(0);
        }
        else
        {
            lastIndex = index;
        }

        // LOG_DEBUG("index: {}", index);
    }
    else if (status == Waved)
    {
        static int lastIndex = 0;
        int index = hahaUi_->getTipAppearIndex();
        if (index < lastIndex)
        {
            statusMutex_.lock();
            sleepStatus_ = Work;
            statusMutex_.unlock();
            lastIndex = 0;
        }
        else
        {
            lastIndex = index;
        }
    }
}

void MainWindow::slot_showtimeout()
{
    std::string rate = common::time::caculateFPS();
    if (rate != "")
    {
        LOG_TRACE("MainWindow Fps: {}!!", rate);
    }

    if (curMat_.empty())
    {
        return;
    }

    matMutex_.lock();
    cv::Mat mat = curMat_.clone();
    matMutex_.unlock();

    auto rects = facedetect_->getCurrentPersonResults();
    fillRects(mat, rects);
    // hahaUi_->addImage(mat, cv::Rect(), HahaUi::Effect);
    statusMutex_.lock();
    SleepStatus status = sleepStatus_;
    statusMutex_.unlock();
    // LOG_DEBUG("status: {}", status);
    if (status == Waving || status == Waved)
    {
        checkUiStatus();

        statusMutex_.lock();
        status = sleepStatus_;
        statusMutex_.unlock();
    }

    if (status == Sleeping)
    {
        hahaUi_->addImage(mat, HahaUi::Sleep);
    }
    else if (status == Waving)
    {
        hahaUi_->addImage(mat, HahaUi::MirrorBroken);
    }
    else if (status == Waved)
    {
        //LOG_DEBUG("waved");
        hahaUi_->addImage(mat, HahaUi::RobotsMirrorLoopTipApper);
    }
    else if (status == Work)
    {
        hahaUi_->addImage(mat, HahaUi::RobotsMirrorLoopTipLoop);
    }

    static cv::Size size(deskRect_.width(), deskRect_.height());
    if (deskRect_.width() != mat.cols || deskRect_.height() != mat.rows)
    {
        cv::resize(mat, mat, size);
    }

    // std::cout << std::endl;

    //    int count = 0;
    //    std::string name = "/home/ubt/code/Qt/hahaMirror/png/frame_" + std::to_string(count++) + ".png";
    //    cv::imwrite(name, mat);

    QImage img = image::mat2qim(mat);
    QPixmap pix = QPixmap::fromImage(img);
    ui->lbl_video->setPixmap(pix);
    mat.release();

    cv::Rect rect;
    rect.empty();
}

void MainWindow::slot_getHahaImage1(cv::Mat mat)
{
    matMutex_.lock();
    if (!curMat_.empty())
    {
        curMat_.release();
    }
    curMat_ = mat.clone();
    mat.release();
    matMutex_.unlock();
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
