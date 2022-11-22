#ifndef HHHAHAUI_H
#define HHHAHAUI_H

#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>
#include <QObject>
#include <QPixmap>
#include <QRect>
#include <QTimer>

struct RobotStrategy
{
    uint8_t cruzer = 0;
    uint8_t wukong1 = 0;
    uint8_t wukong2 = 0;
    uint8_t jimu1 = 0;
    uint8_t jimu2 = 0;
    uint8_t jimu3 = 0;
    uint8_t jimu4 = 0;

    RobotStrategy operator=(RobotStrategy &data)
    {
        this->cruzer = data.cruzer;
        this->wukong1 = data.wukong1;
        this->wukong2 = data.wukong2;
        this->jimu1 = data.jimu1;
        this->jimu2 = data.jimu2;
        this->jimu3 = data.jimu3;
        this->jimu4 = data.jimu4;

        return *this;
    }
};

class DynamicEffect : public QObject
{
    Q_OBJECT
public:
    DynamicEffect();
    ~DynamicEffect();

    void loadImages(std::string dir, float scale = 1.0);
    void start();
    void stop();

    QPixmap *getCurrentPixmap();
    int getCurrentIndex();
    void setCurrentIndex(int index);

    void setPeriod(int period) { interval_time_ = 1000 / period; }
    void setPoint(QPoint point) { point_ = point; }

private slots:
    void slot_timeout();

private:
    void handleCurIndexCallback();

    friend class HahaUi;
    bool running_;
    int interval_time_;
    int count_;
    int curIndex_;
    std::vector<QPixmap> images_;
    QPoint point_;
    QTimer timer_;

    std::mutex mutex_;
    std::thread *thread_;
};

class HahaUi : public QObject
{
    Q_OBJECT
public:
    typedef enum HahaImageType
    {
        Sleep,
        TipLoop,
        TipApper,
        MirrorBroken,
        MirrorLoop,
        Robots,
        RobotsMirrorLoopTipApper,
        RobotsMirrorLoopTipLoop,
        RobotsMirrorBroken
    };

    typedef enum RobotType
    {
        WuKong,
        Jimu
    };

    HahaUi(/* QWidget *parent = nullptr*/);
    ~HahaUi();

    void addImage(cv::Mat &mat, const HahaImageType type);
    void addImage(QPixmap &pix, const HahaImageType type);

    void start();
    void stop();

    void setResolution(const cv::Size size)
    {
        resolutionSize_ = size;
        // updateAllImage();
    }

    void setCurrentMirrorBrokenIndex(int index) { mirrorBrokenEffect_.setCurrentIndex(index); }
    void setCurrentTipAppearIndex(int index) { tipAppearEffect_.setCurrentIndex(index); }
    int getMirrorBrokenIndex() { return mirrorBrokenEffect_.getCurrentIndex(); }
    int getTipAppearIndex() { return tipAppearEffect_.getCurrentIndex(); }

    void changeRobotStrategy();

private:
    void init();
    void initRobotStrategy();
    void loadFonts();
    void loadAllImage();

    void loadSleepImages(std::string dir);
    void loadMirrorBrokenImages(std::string dir);
    void loadMirrorBackgroundImages(std::string dir);
    void loadTipsLoopImages(std::string dir);
    void loadTipsAppearImages(std::string dir);

    void handleTaskCallback();

    void getRandomRobot(int &r1, int &r2);
    void getRandomPoint(QPoint &s1, QPoint &s2);

    void addSleepImage(QPainter &painter);
    void addMirrorBrokenImage(QPainter &painter);
    void addMirrorLoopImage(QPainter &painter);
    void addTipLoopImage(QPainter &painter);
    void addTipAppearImage(QPainter &painter);
    void addRobotImage(QPainter &painter);

    // QWidget *parent_;
    // QFont *font_;
    bool running_;
    std::thread *taskThread_;
    std::string imagePath_;

    DynamicEffect sleepEffect_;
    DynamicEffect mirrorBrokenEffect_;
    DynamicEffect mirrorLoopEffect_;
    DynamicEffect tipLoopEffect_;
    DynamicEffect tipAppearEffect_;
    DynamicEffect cruzrEffect_;
    DynamicEffect wukong1Effect_;
    DynamicEffect wukong2Effect_;
    DynamicEffect jimu1Effect_;
    DynamicEffect jimu2Effect_;
    DynamicEffect jimu3Effect_;
    DynamicEffect jimu4Effect_;
    std::vector<RobotStrategy> robotStrategyVector_;
    int robotStrategyCount_;
    RobotStrategy robotStrategy_;

    cv::Size resolutionSize_;
};

#endif // HAHAUI_H
