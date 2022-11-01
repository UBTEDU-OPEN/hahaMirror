#ifndef HHHAHAUI_H
#define HHHAHAUI_H

#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>
#include <QObject>
#include <QPixmap>
#include <QRect>

#include "resolution.h"

class HahaUi : public QObject
{
    Q_OBJECT
public:
    typedef enum HahaImageType
    {
        Sleep,
        Tips,
        Mirror,
        Robots,
        Effect,
        Tests,
        All
    };

    typedef enum RobotType
    {
        WuKong,
        Jimu
    };

    HahaUi(QWidget *parent = nullptr);
    ~HahaUi();

    void addImage(cv::Mat &mat, const cv::Rect rect, const HahaImageType type);
    void addImage(QPixmap &pix, const QRect rect, const HahaImageType type);
    void addRobot(cv::Mat mat, const cv::Rect rect, const RobotType type);
    void addRobot(QPixmap &pix, const QRect rect, const RobotType type);
    void geMirrorROIImage(cv::Mat mat);
    void setSleepStatus(bool flag) { sleepStatus_ = flag; }

    void start();
    void stop();

    void setResolution(const cv::Size size)
    {
        resolutionSize_ = size;
        // updateAllImage();
    }

    void setResolutionObject(Resolution *resolution)
    {
        if (resolution)
        {
            resolutionObject_ = resolution;
            resolutionSize_ = resolution->resolution_;
            // updateAllImage();
        }
    }
    void changeRobot();

private:
    void init();
    void loadAllImage();
    void loadFonts();
    void updateAllImage();
    void handleTaskCallback();

    void getRandomRobot(int &r1, int &r2);
    void getRandomPoint(QPoint &s1, QPoint &s2);

    void addSleepImage(QPainter &painter);
    void addMirrorImage(QPainter &painter);
    void addTipsImage(QPainter &painter);
    void addRobotImage(QPainter &painter);

    QWidget *parent_;
    QFont *font_;
    bool running_;
    std::thread *taskThread_;
    std::string imagePath_;

    static const double leftMarginScale_;
    static const double rightMarginScale_;
    static const double upMarginScale_;
    static const double downMarginScale_;

    static const QPoint left_up_point;
    static const QPoint right_up_point;
    static const QPoint left_down_point;
    static const QPoint right_down_point;

    int robotCount_;
    std::vector<QPixmap> cruzrRobotImages_;
    int cruzrRobotImagesCount_;
    std::vector<QPixmap> jimuRobotImages_;
    int jimuRobotImagesCount_;
    std::vector<QPixmap> wukongRobotImages_;
    int wukongRobotImagesCount_;
    std::vector<QPixmap> sleepImages_;
    int sleepImageCount_;
    std::vector<QPixmap> tipsImages_;
    int tipsImagesCount_;
    std::vector<QPixmap> mirrorImages_;
    QPixmap emojiImage_;
    int mirrorImagesCount_;
    cv::Size resolutionSize_;

    cv::Mat cruzerMat_;
    cv::Mat mirrorMat_;
    cv::Mat mirrorMaskMat_;
    Resolution *resolutionObject_;

    std::vector<QPixmap> *curLeftRobots_;
    std::vector<QPixmap> *curRightRobots_;
    QPoint leftPoint_;
    QPoint rightPoint_;
    bool sleepStatus_;
};

#endif // HAHAUI_H
