#ifndef PLAYCAMERA_H
#define PLAYCAMERA_H

#include "consumer_base.h"

#include <mutex>
#include <thread>

#include <opencv2/opencv.hpp>
#include <QImage>
#include <QObject>

class PlayCamera : public QObject, public ConsumerRecord
{
    Q_OBJECT
public:
    PlayCamera();
    PlayCamera(const PlayCamera &) = delete;
    ~PlayCamera();

    virtual void consumeRecord(const cv::Mat color_mat, const cv::Mat depth_mat) override;
    virtual void start() override;
    virtual void stop() override;

    void init();

protected:
    void handleTaskCallback();

signals:
    void sig_getHahaImage(QImage img);
    void sig_getHahaImage1(cv::Mat mat);

private:
    bool running_;

    std::mutex matMutex_;
    cv::Mat curMat_;
    cv::Mat lastMat_;
    QImage hahaImg_;
    std::thread *taskThread_;
};

#endif // PLAYCAMERA_H
