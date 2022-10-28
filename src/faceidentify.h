#ifndef FACEIDENTIFY_H
#define FACEIDENTIFY_H

#include "consumer_base.h"
#include "websocket_server.h"
#include <thread>
#include <QObject>

class FaceDetect;
class WebsocketServer;

class FaceIdentify : public QObject, public ConsumerRecord
{
    Q_OBJECT
public:
    FaceIdentify();
    ~FaceIdentify();

    void start() override;
    void stop() override;
    void consumeRecord(const cv::Mat color_mat, const cv::Mat original_mat) override;
    void setFrameRate(int rate)
    {
        frame_rate_ = rate;
        interval_time_ = 1000 / rate;
    }
    void setIntervalTime(int ms) { interval_time_ = ms; }

    void setFaceDetectObject(FaceDetect *faceDetectObj) { faceDetectObj_ = faceDetectObj; }
    void setWebsocketServerOject(WebsocketServer *server);
    void setHttpUrl(std::string url) { url_ = url; }

private:
    void init();
    void handleTaskCallback();
    void httpIdentifyRequest(cv::Mat mat);
    bool parseHttpResponse(std::string &response, ShowFaceInfo &faceinfo);

    bool running_;
    cv::Mat faceMat_;

    int frame_rate_;
    int interval_time_;
    std::string url_;

    std::thread *taskThread_;
    std::mutex matMutex_;
    FaceDetect *faceDetectObj_;
    WebsocketServer *webServer_;
};

#endif // FACEIDENTIFY_H
