#ifndef HHHAHAUI_H
#define HHHAHAUI_H

#include <thread>
#include <vector>

#include <opencv2/opencv.hpp>
#include <QObject>
#include <QPixmap>
#include <QRect>

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
    };

    typedef enum RobotType
    {
        WuKong,
        Jimu
    };

    HahaUi(QWidget *parent = nullptr);
    ~HahaUi();

    void addImage(cv::Mat mat, const cv::Rect rect, const HahaImageType type);
    void addImage(QPixmap &pix, const QRect rect, const HahaImageType type);
    void addRobot(cv::Mat mat, const cv::Rect rect, const RobotType type);
    void addRobot(QPixmap &pix, const QRect rect, const RobotType type);

    void start();
    void stop();

    void setResolution(const cv::Size size)
    {
        resolutionSize_.setWidth(size.width);
        resolutionSize_.setHeight(size.height);

        updateAllImage(resolutionSize_);
    }

private:
    void init();
    void loadAllImage();
    void updateAllImage(QSize rect);
    void handleTaskCallback();

    void addSleepImage(QPixmap &pix, const QRect rect);
    void addMirrorImage(QPixmap &pix, const QRect rect);

    QWidget *parent_;
    bool running_;
    std::thread *taskThread_;
    std::string imagePath_;

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
    int mirrorImagesCount_;
    QSize resolutionSize_;

    cv::Mat cruzerMat_;
    cv::Mat mirrorMat_;
};

#endif // HAHAUI_H
