#include "hahaui.h"
#include "common/logging.h"
#include "common/processinfo.h"
#include "common/time.h"
#include "image.h"
#include <QFontDatabase>
#include <QPainter>
#include <QPixmap>
#include <QTime>
#include <boost/filesystem.hpp>

static void scanFilesUseBoost(const std::string& rootPath,
                              std::vector<std::string>& container,
                              std::string extension)
{
    // container.clear();
    boost::filesystem::path fullpath(rootPath);

    if (!boost::filesystem::exists(fullpath) ||
        !boost::filesystem::is_directory(fullpath))
    {
        std::cerr << "File path not exist!" << std::endl;
        return;
    }

    boost::filesystem::recursive_directory_iterator end_iter;
    for (boost::filesystem::recursive_directory_iterator iter(fullpath);
         iter != end_iter;
         iter++)
    {
        try
        {
            if (boost::filesystem::is_directory(*iter))
            {
                std::cout << *iter << "is dir" << std::endl;
                // scanFilesUseRecursive(iter->path().string(),container,extension);
                // //if find file recursively
            }
            else
            {
                if (boost::filesystem::is_regular_file(*iter) &&
                    iter->path().extension() == extension)
                    container.push_back(iter->path().string());
                //                std::cout << *iter << " is a file" <<
                //                std::endl;
            }
        }
        catch (const std::exception& ex)
        {
            std::cerr << ex.what() << std::endl;
            continue;
        }
    }
}

static bool GreaterEqSort(std::string filePath1, std::string filePath2)
{
    int len1 = filePath1.length();
    int len2 = filePath2.length();
    //    std::cout<<"len1:"<<len1<<" path:"<<filePath1<<std::endl;
    //    std::cout<<"len2:"<<len2<<" path:"<<filePath2<<std::endl;
    if (len1 < len2)
    {
        return false;
    }
    else if (len1 > len2)
    {
        return true;
    }
    else
    {
        int iter = 0;
        while (iter < len1)
        {
            if (filePath1.at(iter) < filePath2.at(iter))
            {
                return false;
            }
            else if (filePath1.at(iter) > filePath2.at(iter))
            {
                return true;
            }
            ++iter;
        }
    }
    return true;
}

static bool LessSort(std::string filePath1, std::string filePath2)
{
    return (!GreaterEqSort(filePath1, filePath2));
}

static void pathSort(std::vector<std::string>& paths, int sortMode)
{
    if (sortMode == 1)
    {
        std::sort(paths.begin(), paths.end(), LessSort);
    }
}

DynamicEffect::DynamicEffect()
    : running_(false)
    , interval_time_(1000 / 24)
    , count_(0)
    , curIndex_(0)
    , point_(0, 0)
{
    connect(&timer_, &QTimer::timeout, this, &DynamicEffect::slot_timeout);
}

DynamicEffect::~DynamicEffect() { stop(); }

QPixmap* DynamicEffect::getCurrentPixmap()
{
    if (count_ == 0)
    {
        return nullptr;
    }

    mutex_.lock();
    int cur = curIndex_;
    mutex_.unlock();

    return &images_[cur];
}

int DynamicEffect::getCurrentIndex()
{
    std::lock_guard<std::mutex> guard(mutex_);
    return curIndex_;
}

void DynamicEffect::setCurrentIndex(int index)
{
    std::lock_guard<std::mutex> guard(mutex_);
    curIndex_ = index;
}

void DynamicEffect::loadImages(std::string dir, float scale)
{
    std::vector<std::string> vec;
    vec.clear();
    scanFilesUseBoost(dir, vec, ".png");
    pathSort(vec, 1);

    int size = vec.size();
    count_ = size;

    for (int i = 0; i < size; ++i)
    {
        // LOG_DEBUG("image: {}", vec[i]);
        QPixmap pix;
        pix.load(QString::fromStdString(vec[i]));
        if (fabs(scale - 1.0) > 0.01)
        {
            pix = pix.scaled(pix.width() * scale, pix.height() * scale);
        }

        images_.emplace_back(pix);
    }
}

void DynamicEffect::slot_timeout()
{
    //  LOG_DEBUG("slot_timeout");
    std::lock_guard<std::mutex> guard(mutex_);
    //    if (count_ == 100)
    //    {
    //        LOG_DEBUG("mirrorLoop index: {}", curIndex_);
    //    }
    if (curIndex_ >= count_ - 1)
    {
        curIndex_ = 0;
    }
    else
    {
        ++curIndex_;
    }
}

void DynamicEffect::handleCurIndexCallback()
{
    static qint64 last = QDateTime::currentDateTime().toMSecsSinceEpoch();
    while (running_)
    {
        common::time::TimeConsumingAnalysis a;
        // LOG_DEBUG("interval_time: {}", interval_time_);
        qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();
        if (now - last < interval_time_ / 2)
        {
            a.addTimePoint("sleep before");
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            a.addTimePoint("sleep after");
            // LOG_DEBUG(a.print());
            continue;
        }
        if (count_ == 100)
        {
            LOG_DEBUG("mirrorLoop index: {}", curIndex_);
        }
        a.addTimePoint("lock before");
        mutex_.lock();
        a.addTimePoint("lock after");
        if (curIndex_ >= count_ - 1)
        {
            curIndex_ = 0;
        }
        else
        {
            ++curIndex_;
        }
        mutex_.unlock();
        a.addTimePoint("unlock after");
        last = now;
        //  LOG_DEBUG(a.print());
    }
}

void DynamicEffect::start()
{
    running_ = true;
    //  thread_ = new std::thread(&DynamicEffect::handleCurIndexCallback, this);

    timer_.setInterval(interval_time_);
    timer_.start(); // 优化项，就是需要一个更精准的定时器
}
void DynamicEffect::stop()
{
    if (timer_.isActive())
    {
        timer_.stop();
    }

    if (thread_ && thread_->joinable())
    {
        thread_->join();
        delete thread_;
    }
}

HahaUi::HahaUi(/*QWidget *parent*/)
    /* : parent_(parent) */
    : imagePath_(common::ProcessInfo::exeDirPath() + "/image/")
    , robotStrategyCount_(0)
    , resolutionSize_(1080, 1920)
{
    init();
}

HahaUi::~HahaUi()
{
    sleepEffect_.stop();
    mirrorBrokenEffect_.stop();
    mirrorLoopEffect_.stop();
    tipLoopEffect_.stop();
    tipAppearEffect_.stop();
}

void HahaUi::init()
{
    initRobotStrategy();
    loadAllImage();
    // loadFonts();
}

// void HahaUi::loadFonts()
//{
//     std::string fontpath = common::ProcessInfo::exeDirPath() +
//     "/YouSheBiaoTiHei-2.ttf"; int id =
//     QFontDatabase::addApplicationFont(QString::fromStdString(fontpath));

//    QStringList families = QFontDatabase::applicationFontFamilies(id);

//    if (!families.empty())
//    {
//        font_ = new QFont(families[0]);
//        font_->setPixelSize(50);
//        font_->setWeight(400);
//    }
//    else
//    {
//        LOG_ERROR("not found font file!!");
//        abort();
//    }
//}

void HahaUi::initRobotStrategy()
{
    RobotStrategy strate1;
    strate1.jimu2 = 1;
    strate1.jimu4 = 1;
    robotStrategyVector_.emplace_back(strate1);

    RobotStrategy strate2;
    strate2.jimu1 = 1;
    strate2.jimu3 = 1;
    robotStrategyVector_.emplace_back(strate2);

    RobotStrategy strate3;
    strate3.cruzer = 1;
    robotStrategyVector_.emplace_back(strate3);

    RobotStrategy strate4;
    strate4.wukong1 = 1;
    robotStrategyVector_.emplace_back(strate4);

    RobotStrategy strate5;
    strate5.wukong2 = 1;
    robotStrategyVector_.emplace_back(strate5);

    robotStrategyCount_ = robotStrategyVector_.size();
}

void HahaUi::loadAllImage()
{
    std::string sleep_dir = imagePath_ + "sleep/";
    sleepEffect_.loadImages(sleep_dir);
    sleepEffect_.start();

    std::string mirror_broken_dir = imagePath_ + "mirrorBroken/";
    mirrorBrokenEffect_.loadImages(mirror_broken_dir);
    mirrorBrokenEffect_.start();

    std::string mirror_loop_dir = imagePath_ + "mirrorLoop/background/";
    mirrorLoopEffect_.loadImages(mirror_loop_dir);
    mirrorLoopEffect_.start();

    std::string tip_loop_dir = imagePath_ + "tipsLoop/";
    tipLoopEffect_.loadImages(tip_loop_dir);
    tipLoopEffect_.start();

    std::string tip_appear_dir = imagePath_ + "tipsAppear/";
    tipAppearEffect_.loadImages(tip_appear_dir);
    tipAppearEffect_.start();

    std::string cruzr_dir = imagePath_ + "cruzr/";
    cruzrEffect_.loadImages(cruzr_dir, 1.0);
    cruzrEffect_.setPoint(QPoint(700, 1400));
    cruzrEffect_.start();

    std::string wukong1_dir = imagePath_ + "wukong1/";
    wukong1Effect_.loadImages(wukong1_dir, 1.0);
    wukong1Effect_.setPoint(QPoint(90, 1400));
    wukong1Effect_.start();

    std::string wukong2_dir = imagePath_ + "wukong2/";
    wukong2Effect_.loadImages(wukong2_dir, 1.0);
    wukong2Effect_.setPoint(QPoint(745, 1400));
    wukong2Effect_.start();

    std::string jimu1_dir = imagePath_ + "jimu1/";
    jimu1Effect_.loadImages(jimu1_dir, 2.0);
    jimu1Effect_.setPoint(QPoint(0, 565));
    jimu1Effect_.start();

    std::string jimu2_dir = imagePath_ + "jimu2/";
    jimu2Effect_.loadImages(jimu2_dir, 2.0);
    jimu2Effect_.setPoint(QPoint(0, 1443));
    jimu2Effect_.start();

    std::string jimu3_dir = imagePath_ + "jimu3/";
    jimu3Effect_.loadImages(jimu3_dir, 2.0);
    jimu3Effect_.setPoint(QPoint(790, 1377));
    jimu3Effect_.start();

    std::string jimu4_dir = imagePath_ + "jimu4/";
    jimu4Effect_.loadImages(jimu4_dir, 2.0);
    jimu4Effect_.setPoint(QPoint(730, 310));
    jimu4Effect_.start();
}

void HahaUi::addImage(cv::Mat& mat, const HahaImageType type)
{
    QPixmap pix = QPixmap::fromImage(image::mat2qim(mat));
    QPainter painter;
    painter.begin(&pix);

    if (type == Sleep)
    {
        addSleepImage(painter);
    }
    else if (type == TipLoop)
    {
        addTipLoopImage(painter);
    }
    else if (type == TipApper)
    {
        addTipAppearImage(painter);
    }
    else if (type == MirrorBroken)
    {
        addMirrorBrokenImage(painter);
    }
    else if (type == MirrorLoop)
    {
        addMirrorLoopImage(painter);
    }
    else if (type == Robots)
    {
        addRobotImage(painter);
    }
    else if (type == RobotsMirrorLoopTipApper)
    {
        addRobotImage(painter);
        addMirrorLoopImage(painter);
        addTipAppearImage(painter);
    }
    else if (type == RobotsMirrorLoopTipLoop)
    {
        addRobotImage(painter);
        addMirrorLoopImage(painter);
        addTipLoopImage(painter);
    }
    else if (type == RobotsMirrorBroken)
    {
        addRobotImage(painter);
        addMirrorBrokenImage(painter);
    }

    painter.end();
    mat = image::QPixmapToCvMat(pix, true);
}

void HahaUi::addMirrorLoopImage(QPainter& painter)
{
    auto pix = mirrorLoopEffect_.getCurrentPixmap();
    painter.drawPixmap(0, 0, pix->width(), pix->height(), (*pix).copy());
}

void HahaUi::addMirrorBrokenImage(QPainter& painter)
{
    auto pix = mirrorBrokenEffect_.getCurrentPixmap();
    painter.drawPixmap(0, 0, pix->width(), pix->height(), *pix);
}

void HahaUi::addSleepImage(QPainter& painter)
{
    auto pix = sleepEffect_.getCurrentPixmap();
    painter.drawPixmap(0, 0, pix->width(), pix->height(), *pix);
}

void HahaUi::addTipAppearImage(QPainter& painter)
{
    auto pix = tipAppearEffect_.getCurrentPixmap();
    painter.drawPixmap(118, 88, pix->width(), pix->height(), *pix);
}

void HahaUi::addTipLoopImage(QPainter& painter)
{
    auto pix = tipLoopEffect_.getCurrentPixmap();
    painter.drawPixmap(118, 88, pix->width(), pix->height(), *pix);
}

void HahaUi::addRobotImage(QPainter& painter)
{
    if (robotStrategy_.cruzer == 1)
    {
        auto pix = cruzrEffect_.getCurrentPixmap();
        QPoint point = cruzrEffect_.point_;
        painter.drawPixmap(
            point.x(), point.y(), pix->width(), pix->height(), *pix);
    }
    if (robotStrategy_.wukong1 == 1)
    {
        auto pix = wukong1Effect_.getCurrentPixmap();
        QPoint point = wukong1Effect_.point_;
        painter.drawPixmap(
            point.x(), point.y(), pix->width(), pix->height(), *pix);
    }
    if (robotStrategy_.wukong2 == 1)
    {
        auto pix = wukong2Effect_.getCurrentPixmap();
        QPoint point = wukong2Effect_.point_;
        painter.drawPixmap(
            point.x(), point.y(), pix->width(), pix->height(), *pix);
    }
    if (robotStrategy_.jimu1 == 1)
    {
        auto pix = jimu1Effect_.getCurrentPixmap();
        QPoint point = jimu1Effect_.point_;
        painter.drawPixmap(
            point.x(), point.y(), pix->width(), pix->height(), *pix);
    }
    if (robotStrategy_.jimu2 == 1)
    {
        auto pix = jimu2Effect_.getCurrentPixmap();
        QPoint point = jimu2Effect_.point_;
        painter.drawPixmap(
            point.x(), point.y(), pix->width(), pix->height(), *pix);
    }
    if (robotStrategy_.jimu3 == 1)
    {
        auto pix = jimu3Effect_.getCurrentPixmap();
        QPoint point = jimu3Effect_.point_;
        painter.drawPixmap(
            point.x(), point.y(), pix->width(), pix->height(), *pix);
    }
    if (robotStrategy_.jimu4 == 1)
    {
        auto pix = jimu4Effect_.getCurrentPixmap();
        QPoint point = jimu4Effect_.point_;
        painter.drawPixmap(
            point.x(), point.y(), pix->width(), pix->height(), *pix);
    }
}

void HahaUi::getRandomRobot(int& r1, int& r2)
{
    time(NULL);
    r1 = rand() % 3;
    r2 = rand() % 3;
    while (r1 == r2)
    {
        r2 = rand() % 3;
    }
}

void HahaUi::changeRobotStrategy()
{
    static int lastStrategyIndex = 0;
    time(NULL);

    int index = rand() % robotStrategyCount_;
    while (index == lastStrategyIndex)
    {
        index = rand() % robotStrategyCount_;
    }

    robotStrategy_ = robotStrategyVector_[index];
    lastStrategyIndex = index;
}
