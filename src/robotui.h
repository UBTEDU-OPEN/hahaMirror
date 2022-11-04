#ifndef ROBOTUI_H
#define ROBOTUI_H

#include <QObject>
#include <QRect>

class RobotUi
{
public:
    RobotUi() = default;
    ~RobotUi() = default;

    QRect rect_;
    bool showing_;

    virtual QPixmap *getCurrentPixmap() = 0;
    virtual void loadImages(std::string dir) = 0;
    virtual void start();
    virtual void stop();
};

class CruzerRobot : public RobotUi
{
public:
    CruzerRobot();
    ~CruzerRobot() = default;

    virtual QPixmap *getCurrentPixmap() override { return nullptr; }
    virtual void loadImages(std::string dir) override {}
    virtual void start() {}
    virtual void stop() {}

    //    int robotCount_;
    //    std::vector<QPixmap> cruzrRobotImages_;
    //    int cruzrRobotImagesCount_;
    //    std::vector<QPixmap> jimuRobotImages_;
    //    int jimuRobotImagesCount_;
    //    std::vector<QPixmap> wukongRobotImages_;
    //    int wukongRobotImagesCount_;
};

#endif // ROBOTUI_H
