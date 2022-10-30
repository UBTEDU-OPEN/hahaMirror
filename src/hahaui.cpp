#include "hahaui.h"
#include "common/logging.h"
#include "common/processinfo.h"
#include "image.h"
#include <QLabel>
#include <QPainter>
#include <QPixmap>

HahaUi::HahaUi(QWidget *parent)
    : parent_(parent)
    , imagePath_(common::ProcessInfo::exeDirPath() + "/image/")
    , resolutionSize_(480, 640)
{
    init();
}

HahaUi::~HahaUi() {}

void HahaUi::init()
{
    loadAllImage();
}

void HahaUi::loadAllImage()
{
    using namespace cv;

    robotCount_ = 3;
    std::string robot_path = imagePath_ + "robot/";

    QImage cruzr(QString::fromStdString(robot_path + "cruzr1.png"));
    if (cruzr.isNull())
    {
        abort();
    }

    QPixmap cruzr_pixmap = QPixmap::fromImage(cruzr).scaled(50, 50);
    cruzrRobotImages_.emplace_back(cruzr_pixmap);
    cruzrRobotImagesCount_ = 1;

    QLabel *label = new QLabel(parent_);
    label->setPixmap(cruzr_pixmap);
    label->setGeometry(0, 0, cruzr_pixmap.width(), cruzr_pixmap.height());

    QImage jimu1(QString::fromStdString(robot_path + "jimu1.png"));
    if (jimu1.isNull())
    {
        abort();
    }
    QImage jimu2(QString::fromStdString(robot_path + "jimu2.png"));
    if (jimu2.isNull())
    {
        abort();
    }
    QImage jimu3(QString::fromStdString(robot_path + "jimu3.png"));
    if (jimu3.isNull())
    {
        abort();
    }
    QImage jimu4(QString::fromStdString(robot_path + "jimu4.png"));
    if (jimu4.isNull())
    {
        abort();
    }
    QPixmap jimu1_pixmap = QPixmap::fromImage(jimu1);
    QPixmap jimu2_pixmap = QPixmap::fromImage(jimu2);
    QPixmap jimu3_pixmap = QPixmap::fromImage(jimu3);
    QPixmap jimu4_pixmap = QPixmap::fromImage(jimu4);
    jimuRobotImages_.emplace_back(jimu1_pixmap);
    jimuRobotImages_.emplace_back(jimu2_pixmap);
    jimuRobotImages_.emplace_back(jimu3_pixmap);
    jimuRobotImages_.emplace_back(jimu4_pixmap);
    jimuRobotImagesCount_ = 4;

    QImage wukong1(QString::fromStdString(robot_path + "wukong1.png"));
    if (wukong1.isNull())
    {
        abort();
    }
    QImage wukong2(QString::fromStdString(robot_path + "wukong2.png"));
    if (wukong2.isNull())
    {
        abort();
    }
    QPixmap wukong1_pixmap = QPixmap::fromImage(wukong1);
    QPixmap wukong2_pixmap = QPixmap::fromImage(wukong2);
    wukongRobotImages_.emplace_back(wukong1_pixmap);
    wukongRobotImages_.emplace_back(wukong2_pixmap);
    wukongRobotImagesCount_ = 2;

    QPixmap sleep_pixmap;
    sleep_pixmap.load(QString::fromStdString(imagePath_ + "sleep.png"));
    if (sleep_pixmap.isNull())
    {
        abort();
    }
    sleepImages_.emplace_back(sleep_pixmap);
    sleepImageCount_ = 1;

    QImage tips(QImage(QString::fromStdString(imagePath_ + "tips.png")));
    if (tips.isNull())
    {
        abort();
    }
    QPixmap tips_pixmap = QPixmap::fromImage(tips);
    tipsImages_.emplace_back(tips_pixmap);
    tipsImagesCount_ = 1;

    mirrorMat_ = imread(imagePath_ + "mirror.png");
    // QImage mirror(QImage(QString::fromStdString(imagePath_ + "mirror.png")));
    QImage mirror = image::mat2qim(mirrorMat_);
    LOG_DEBUG("MIRRROR: {}", mirror.format());
    if (mirror.isNull())
    {
        abort();
    }
    QPixmap mirrorImage = QPixmap::fromImage(mirror);
    LOG_DEBUG("MIRRROR2: {}", mirrorImage.toImage().format());
    mirrorImages_.emplace_back(mirrorImage);
    mirrorImagesCount_ = 1;
}

void HahaUi::updateAllImage(QSize size)
{
    for (int i = 0; i < cruzrRobotImagesCount_; ++i)
    {
        cruzrRobotImages_[i] = cruzrRobotImages_[i].scaled(size,
                                                           Qt::AspectRatioMode::IgnoreAspectRatio,
                                                           Qt::SmoothTransformation);
    }
    for (int i = 0; i < jimuRobotImagesCount_; ++i)
    {
        jimuRobotImages_[i] = jimuRobotImages_[i].scaled(size,
                                                         Qt::AspectRatioMode::IgnoreAspectRatio,
                                                         Qt::SmoothTransformation);
    }
    for (int i = 0; i < wukongRobotImagesCount_; ++i)
    {
        wukongRobotImages_[i] = wukongRobotImages_[i].scaled(size,
                                                             Qt::AspectRatioMode::IgnoreAspectRatio,
                                                             Qt::SmoothTransformation);
    }
    for (int i = 0; i < sleepImageCount_; ++i)
    {
        auto &pix = sleepImages_[i];
        pix = pix.scaled(size, Qt::AspectRatioMode::IgnoreAspectRatio, Qt::SmoothTransformation);
    }

    for (int i = 0; i < mirrorImagesCount_; ++i)
    {
        mirrorImages_[i] = mirrorImages_[i].scaled(size,
                                                   Qt::AspectRatioMode::IgnoreAspectRatio,
                                                   Qt::SmoothTransformation);
        // cv::imshow("mirror", image::QPixmapToCvMat(mirrorImages_[i]));
    }
    for (int i = 0; i < tipsImagesCount_; ++i)
    {
        tipsImages_[i] = tipsImages_[i].scaled(size,
                                               Qt::AspectRatioMode::IgnoreAspectRatio,
                                               Qt::SmoothTransformation);
    }
}

void HahaUi::addImage(cv::Mat mat, const cv::Rect rect, const HahaImageType type)
{
    if (type == Sleep)
    {
        cv::imshow("sourceImage", mat);
        QPixmap pix = QPixmap::fromImage(image::mat2qim(mat));
        QRect qrect(image::rect2qrect(rect));
        addSleepImage(pix, qrect);

        mat = image::QPixmapToCvMat(pix.copy(), true);

        cv::imshow("addImage-sleepImage", mat);
    }
    else if (type == Mirror)
    {
        QPixmap pix = QPixmap::fromImage(image::mat2qim(mat));
        QRect qrect(image::rect2qrect(rect));
        addMirrorImage(pix, qrect);

        mat = image::QPixmapToCvMat(pix, true);
        cv::imshow("addImage-mirrorImage", mat);
    }
}

void HahaUi::addMirrorImage(QPixmap &pix, const QRect rect)
{
    cv::imshow("before mirror2 ", image::QPixmapToCvMat(pix));
    QPainter painter(&pix);
    // painter.setCompositionMode(QPainter::CompositionMode_DestinationOver);
    auto &mirror_pixmap = mirrorImages_[0];
    cv::imshow("mirror", image::QPixmapToCvMat(mirror_pixmap, true));
    LOG_DEBUG("fomat2: {}", mirror_pixmap.toImage().format());
    painter.drawPixmap(0, 0, mirror_pixmap.width(), mirror_pixmap.height(), mirror_pixmap);
    // painter.end();
    cv::imshow("after mirror", image::QPixmapToCvMat(pix, true));
    LOG_DEBUG("fomat3: {}", pix.toImage().format());
}

void HahaUi::addSleepImage(QPixmap &pix, const QRect rect)
{
    QPainter painter(&pix);
    auto &sleep_pixmap = sleepImages_[0];
    painter.drawPixmap(0, 0, sleep_pixmap.width(), sleep_pixmap.height(), sleepImages_[0]);
}
