#include "hahaui.h"
#include "common/logging.h"
#include "common/processinfo.h"
#include "image.h"
#include <QFontDatabase>
#include <QPainter>
#include <QPixmap>

/// 1080p 画质下，左： 107, 右 973, 上 277 ， 下 1643

const double HahaUi::leftMarginScale_ = 0.0991;
const double HahaUi::rightMarginScale_ = 0.9009;
const double HahaUi::upMarginScale_ = 0.1443;
const double HahaUi::downMarginScale_ = 0.8557;

const QPoint HahaUi::left_up_point(107, 565);
const QPoint HahaUi::right_up_point(822, 445);
const QPoint HahaUi::left_down_point(50, 1419);
const QPoint HahaUi::right_down_point(735, 1438);

HahaUi::HahaUi(QWidget *parent)
    : parent_(parent)
    , imagePath_(common::ProcessInfo::exeDirPath() + "/image/")
    , resolutionSize_(480, 640)
    , curLeftRobots_(nullptr)
    , curRightRobots_(nullptr)
    , sleepStatus_(true)
{
    init();
}

HahaUi::~HahaUi() {}

void HahaUi::init()
{
    loadAllImage();
    loadFonts();
}

void HahaUi::loadFonts()
{
    std::string fontpath = common::ProcessInfo::exeDirPath() + "/YouSheBiaoTiHei-2.ttf";
    int id = QFontDatabase::addApplicationFont(QString::fromStdString(fontpath));

    QStringList families = QFontDatabase::applicationFontFamilies(id);
    // LOG_DEBUG("[load fonts] path:{},  size:{}", fontpath, families.size());
    if (!families.empty())
    {
        font_ = new QFont(families[0]);
        font_->setPixelSize(50);
        font_->setWeight(400);
    }
    else
    {
        LOG_ERROR("not found font file!!");
        abort();
    }
}

void HahaUi::loadAllImage()
{
    using namespace cv;

    robotCount_ = 3;
    std::string robot_path = imagePath_ + "robot/";
    // cruzr 图片缓存
    QImage cruzr(QString::fromStdString(robot_path + "cruzr1.png"));
    if (cruzr.isNull())
    {
        LOG_ERROR("load cruzr image failed!!");
        abort();
    }
    QPixmap cruzr_pixmap = QPixmap::fromImage(cruzr);
    cruzrRobotImages_.emplace_back(cruzr_pixmap);
    cruzrRobotImagesCount_ = 1;
    // jimu 图片缓存
    QImage jimu1(QString::fromStdString(robot_path + "jimu1.png"));
    if (jimu1.isNull())
    {
        LOG_ERROR("load jimu1 image failed!!");
        abort();
    }
    QImage jimu2(QString::fromStdString(robot_path + "jimu2.png"));
    if (jimu2.isNull())
    {
        LOG_ERROR("load jimu2 image failed!!");
        abort();
    }
    QImage jimu3(QString::fromStdString(robot_path + "jimu3.png"));
    if (jimu3.isNull())
    {
        LOG_ERROR("load jimu3 image failed!!");
        abort();
    }
    QImage jimu4(QString::fromStdString(robot_path + "jimu4.png"));
    if (jimu4.isNull())
    {
        LOG_ERROR("load jimu4 image failed!!");
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
    // wukong 图片缓存
    QImage wukong1(QString::fromStdString(robot_path + "wukong1.png"));
    if (wukong1.isNull())
    {
        LOG_ERROR("load wukong1 image failed!!");
        abort();
    }
    QImage wukong2(QString::fromStdString(robot_path + "wukong2.png"));
    if (wukong2.isNull())
    {
        LOG_ERROR("load wukong2 image failed!!");
        abort();
    }
    QPixmap wukong1_pixmap = QPixmap::fromImage(wukong1);
    QPixmap wukong2_pixmap = QPixmap::fromImage(wukong2);
    wukongRobotImages_.emplace_back(wukong1_pixmap);
    wukongRobotImages_.emplace_back(wukong2_pixmap);
    wukongRobotImagesCount_ = 2;
    // 休眠图片缓存
    QPixmap sleep_pixmap;
    sleep_pixmap.load(QString::fromStdString(imagePath_ + "sleep.jpg"));
    if (sleep_pixmap.isNull())
    {
        LOG_ERROR("load sleep image failed!!");
        abort();
    }
    sleepImages_.emplace_back(sleep_pixmap);
    sleepImageCount_ = 1;
    // 提示框图片缓存
    QImage tips(QImage(QString::fromStdString(imagePath_ + "tips.png")));
    if (tips.isNull())
    {
        LOG_ERROR("load tips image failed!!");
        abort();
    }
    QPixmap tips_pixmap = QPixmap::fromImage(tips);

    // LOG_DEBUG("first tips: {},{}", tips_pixmap.size().width(), tips_pixmap.size().height());
    tipsImages_.emplace_back(tips_pixmap);
    tipsImagesCount_ = 1;
    // 魔镜图片缓存
    QImage mirror(QImage(QString::fromStdString(imagePath_ + "mirror.png")));
    if (mirror.isNull())
    {
        LOG_ERROR("load mirror png image failed, {}!!", imagePath_ + "mirror.png");
        abort();
    }
    QPixmap mirror_pixmap = QPixmap::fromImage(mirror);
    mirrorImages_.emplace_back(mirror_pixmap);
    mirrorImagesCount_ = 1;

    QImage emoji(QImage(QString::fromStdString(imagePath_ + "emoji-smile.png")));
    if (mirror.isNull())
    {
        LOG_ERROR("load emoji png image failed, {}!!", imagePath_ + "emoji.png");
        abort();
    }
    emojiImage_ = QPixmap::fromImage(emoji).scaled(43, 41);
}

void HahaUi::updateAllImage()
{
    if (!resolutionObject_)
    {
        return;
    }

    auto &cruzrs = resolutionObject_->robotRects_[0];
    QSize cruzr_size(cruzrs.width, cruzrs.height);
    for (int i = 0; i < cruzrRobotImagesCount_; ++i)
    {
        cruzrRobotImages_[i] = cruzrRobotImages_[i].scaled(cruzr_size,
                                                           Qt::AspectRatioMode::IgnoreAspectRatio,
                                                           Qt::SmoothTransformation);
    }

    auto &jimus = resolutionObject_->robotRects_[1];
    QSize jimu_size(jimus.width, jimus.height);
    for (int i = 0; i < jimuRobotImagesCount_; ++i)
    {
        jimuRobotImages_[i] = jimuRobotImages_[i].scaled(jimu_size,
                                                         Qt::AspectRatioMode::IgnoreAspectRatio,
                                                         Qt::SmoothTransformation);
    }

    auto &wukongs = resolutionObject_->robotRects_[2];
    QSize wukong_size(wukongs.width, wukongs.height);
    for (int i = 0; i < wukongRobotImagesCount_; ++i)
    {
        wukongRobotImages_[i] = wukongRobotImages_[i].scaled(wukong_size,
                                                             Qt::AspectRatioMode::IgnoreAspectRatio,
                                                             Qt::SmoothTransformation);
    }

    QSize size(resolutionSize_.width, resolutionSize_.height);
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

    auto &tips = resolutionObject_->tipsMarginRect_;
    QSize tips_size(tips.width, tips.height);
    //LOG_DEBUG("tips: {},{}", tipsImages_[0].size().width(), tipsImages_[0].size().height());
    for (int i = 0; i < tipsImagesCount_; ++i)
    {
        tipsImages_[i] = tipsImages_[i].scaled(tips_size,
                                               Qt::AspectRatioMode::IgnoreAspectRatio,
                                               Qt::SmoothTransformation);
    }
}

void HahaUi::addImage(cv::Mat &mat, const cv::Rect rect, const HahaImageType type)
{
    QPixmap pix = QPixmap::fromImage(image::mat2qim(mat));
    QPainter painter;
    painter.begin(&pix);

    if (type == Sleep)
    {
        addSleepImage(painter);
    }
    else if (type == Mirror)
    {
        addMirrorImage(painter);
    }
    else if (type == Tips)
    {
        addTipsImage(painter);
    }
    else if (type == Robots)
    {
        addRobotImage(painter);
    }
    else if (type == All)
    {
        addRobotImage(painter);
        addMirrorImage(painter);
        addTipsImage(painter);
    }
    else if (type == Effect)
    {
        addRobotImage(painter);
        addMirrorImage(painter);
        addTipsImage(painter);
    }

    painter.end();
    mat = image::QPixmapToCvMat(pix, true);
}

void HahaUi::addMirrorImage(QPainter &painter)
{
    //    mat.copyTo(mirrorMat_, mirrorMaskMat_);
    //    mat = mirrorMat_.clone();

    auto &pix = mirrorImages_[0];
    painter.drawPixmap(0, 0, pix.width(), pix.height(), mirrorImages_[0]);
}

void HahaUi::addSleepImage(QPainter &painter)
{
    auto &pix = sleepImages_[0];
    painter.drawPixmap(0, 0, pix.width(), pix.height(), sleepImages_[0]);
}

void HahaUi::addTipsImage(QPainter &painter)
{
    auto pix = tipsImages_[0];
    painter.drawPixmap(118, 88, pix.width(), pix.height(), pix);

    //绘制字体
    painter.setFont(*font_);
    painter.setPen("#2AADFF");
    painter.drawText(QRect(375, 131, 279, 65), Qt::AlignCenter, tr("遮住脸试试看"));

    // 绘制 emoji
    painter.drawPixmap(661, 143, emojiImage_.width(), emojiImage_.height(), emojiImage_);
}

void HahaUi::addRobotImage(QPainter &painter)
{
    if (curLeftRobots_ == nullptr || curRightRobots_ == nullptr)
    {
        changeRobot();
    }

    painter.drawPixmap(leftPoint_.x(),
                       leftPoint_.y(),
                       (*curLeftRobots_)[0].width(),
                       (*curLeftRobots_)[0].height(),
                       (*curLeftRobots_)[0]);
    painter.drawPixmap(rightPoint_.x(),
                       rightPoint_.y(),
                       (*curRightRobots_)[0].width(),
                       (*curRightRobots_)[0].height(),
                       (*curRightRobots_)[0]);
}

void HahaUi::getRandomRobot(int &r1, int &r2)
{
    time(NULL);
    r1 = rand() % 3;
    r2 = rand() % 3;
    while (r1 == r2)
    {
        r2 = rand() % 3;
    }
}

void HahaUi::getRandomPoint(QPoint &left, QPoint &right)
{
    int l = rand() % 2;
    int r = rand() % 2;
    if (l == 0)
    {
        left = left_up_point;
    }
    else
    {
        left = left_down_point;
    }

    if (r == 0)
    {
        right = right_up_point;
    }
    else
    {
        right = right_down_point;
    }
}

void HahaUi::changeRobot()
{
    int rob1, rob2;

    getRandomRobot(rob1, rob2);
    getRandomPoint(leftPoint_, rightPoint_);

    if (rob1 == 0)
    {
        curLeftRobots_ = &cruzrRobotImages_;
    }
    else if (rob1 == 1)
    {
        curLeftRobots_ = &jimuRobotImages_;
    }
    else if (rob1 == 2)
    {
        curLeftRobots_ = &wukongRobotImages_;
    }
    if (rob2 == 0)
    {
        curRightRobots_ = &cruzrRobotImages_;
    }
    else if (rob2 == 1)
    {
        curRightRobots_ = &jimuRobotImages_;
    }
    else if (rob2 == 2)
    {
        curRightRobots_ = &wukongRobotImages_;
    }
}
