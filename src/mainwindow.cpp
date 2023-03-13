#include "mainwindow.h"

#include <opencv2/opencv.hpp>
#include <set>
#include <stdlib.h>
#include <QDesktopWidget>
#include <QMenu>
#include <QMetaType>
#include <QMouseEvent>
#include <QPainter>
#include <QScreen>
#include <QTimer>

#include "boost/filesystem.hpp"
#include "common/logging.h"
#include "common/time.h"
#include "image.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , pressed_(false)
    , showRectangle_(false)
    , showValidArea_(false)
    , lastEffect_(Face)
    , resolution_(cv::Size{1080, 1920})
    , cameraServer_(nullptr)
    , sleepStatus_(Sleeping)
{
    ui->setupUi(this);
    config_ = new config::Config;
    this->initLogger();
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

void MainWindow::initLogger()
{
    int log_level = config_->basic()->log_level;
    if (log_level == 0)
    {
        common::log::setLogLevel("trace");
    }
    else if (log_level == 1)
    {
        common::log::setLogLevel("debug");
    }
    else if (log_level == 2)
    {
        common::log::setLogLevel("info");
    }
    else if (log_level == 3)
    {
        common::log::setLogLevel("error");
    }
    else
    {
        abort();
    }
}

void MainWindow::init()
{
    webServer_ = new WebsocketServer;
    // webServer_->startVirtualServer(9004);
    webServer_->start(9004);

    //    cameraClient_ = new PlayCamera();
    //    cameraClient_->start();

    hahaCore_ = new Hahacore;
    hahaCore_->setConfig(config_);
    hahaCore_->setFaceDetectObject(facedetect_);
    hahaCore_->start();
    facedetect_ = new FaceDetect;
    facedetect_->setConfig(config_);
    facedetect_->setHahacoreObject(hahaCore_);
    facedetect_->start();
    faceIdentify_ = new FaceIdentify;
    faceIdentify_->setConfig(config_);
    faceIdentify_->setFaceDetectObject(facedetect_);
    faceIdentify_->setWebsocketServerOject(webServer_);
    faceIdentify_->start();

    hahaUi_ = new HahaUi;

    cameraServer_ = new ProducerRecordImpl(0);
    cameraServer_->setConfig(config_);
    cameraServer_->startServer();
    // cameraServer_->registerRecordConsumer(cameraClient_);
    cameraServer_->registerRecordConsumer(facedetect_);
    // cameraServer_->registerRecordConsumer(hahaCore_);
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

int MainWindow::getVideoDevice()
{
    boost::filesystem::path fullpath("/dev/");

    boost::filesystem::recursive_directory_iterator end_iter;
    std::set<std::string> container;
    for (boost::filesystem::recursive_directory_iterator iter(fullpath); iter != end_iter; iter++)
    {
        try
        {
            if (boost::filesystem::is_other(*iter))
            {
                auto name = iter->path().string();
                if (name.find("/dev/video") != std::string::npos)
                {
                    LOG_DEBUG("name: {}", name);
                    container.insert(iter->path().string());
                }
            }
        }
        catch (const std::exception &ex)
        {
            std::cerr << ex.what() << std::endl;
            continue;
        }
    }

    if (container.size() == 0)
    {
        LOG_ERROR("not found camera!!");
        abort();
    }

    return QString(container.begin()->c_str()).toInt();
}

void MainWindow::initConnect()
{
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
        if (geometry.width() < geometry.height())
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
            move(geometry.x(), geometry.y());
            this->showFullScreen();
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

    areaMenu_ = rootMenu_->addMenu("area");
    QAction *rectangle_action = areaMenu_->addAction("headArea");
    rectangle_action->setObjectName("headArea");
    QAction *validArea = areaMenu_->addAction("validArea");
    validArea->setObjectName("validArea");

    connect(face_action, &QAction::triggered, this, &MainWindow::slot_setHahaEffect);
    connect(mouth_action, &QAction::triggered, this, &MainWindow::slot_setHahaEffect);
    connect(eyes_action, &QAction::triggered, this, &MainWindow::slot_setHahaEffect);
    connect(none_action, &QAction::triggered, this, &MainWindow::slot_setHahaEffect);
    connect(rectangle_action, &QAction::triggered, this, &MainWindow::slot_setArea);
    connect(validArea, &QAction::triggered, this, &MainWindow::slot_setArea);
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
        cv::rectangle(mat, it->bigFaceRect, cv::Scalar(115, 210, 22), 2);
        //  cv::rectangle(mat, it->algorithmFaceRect, cv::Scalar(115, 210, 22),
        //  2);
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

void MainWindow::slot_setArea()
{
    QAction *action = qobject_cast<QAction *>(sender());
    QString name = action->objectName();

    if (name == "headArea")
    {
        showRectangle_ = !showRectangle_;
    }
    else if (name == "validArea")
    {
        showValidArea_ = !showValidArea_;
    }
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
    static common::time::CaculateFps fps;
    std::string rate = fps.add();
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
    if (showRectangle_)
    {
        fillRects(mat, rects);
    }

    if (showValidArea_)
    {
        cv::rectangle(mat, facedetect_->getValidArea(), cv::Scalar(0, 0, 255), 2);
    }

    static cv::Size ui_size(1080, 1920);
    cv::resize(mat, mat, ui_size);

    statusMutex_.lock();
    SleepStatus status = sleepStatus_;
    statusMutex_.unlock();

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
        hahaUi_->addImage(mat, HahaUi::MirrorLoop);
        hahaUi_->addImage(mat, HahaUi::MirrorBroken);
    }
    else if (status == Waved)
    {
        hahaUi_->addImage(mat, HahaUi::RobotsMirrorLoopTipApper);
    }
    else if (status == Work)
    {
        hahaUi_->addImage(mat, HahaUi::RobotsMirrorLoopTipLoop);
    }

    static cv::Mat clone;
    static int direction = config_->camera()->direction; // 0 摄像头横放，1摄像头竖放
    if (direction == 0 || direction == 1)
    {
        image::rotate_arbitrarily_angle(mat, clone, 90);
    }
    mat.release();

    static cv::Size size(deskRect_.width(), deskRect_.height());
    if (deskRect_.width() != clone.cols || deskRect_.height() != mat.rows)
    {
        cv::resize(clone, clone, size);
    }

    QImage img = image::mat2qim(clone);
    QPixmap pix = QPixmap::fromImage(img);
    ui->lbl_video->setPixmap(pix);
    clone.release();

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
