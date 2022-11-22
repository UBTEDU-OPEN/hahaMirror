#ifndef FACEIDENTIFY_H
#define FACEIDENTIFY_H

#include "config.h"
#include "consumer_base.h"
#include "websocket_server.h"
#include <thread>
#include <QObject>

class FaceDetect;
class WebsocketServer;

struct FaceRecord
{
    std::string user_id;
    int64_t time;
    int smile;
};

class FaceIdentify : public QObject, public ConsumerRecord
{
    Q_OBJECT
public:
    FaceIdentify();
    ~FaceIdentify();

    void start() override;
    void stop() override;
    void consumeRecord(const cv::Mat color_mat, const cv::Mat original_mat) override;
    void setIntervalTime(int ms) { interval_time_ = ms; }

    void setFaceDetectObject(FaceDetect *faceDetectObj) { faceDetectObj_ = faceDetectObj; }
    void setWebsocketServerOject(WebsocketServer *server);
    void setDetectStatus(bool flag) { detectStatus_ = flag; }
    void setConfig(config::Config *config)
    {
        config_ = config;
        interval_time_ = config_->http()->identify_interval_time_ms;
    }

private:
    void init();
    void handleTaskCallback();
    void httpIdentifyRequest(cv::Mat mat);
    bool parseHttpResponse(std::string &response, ShowFaceInfo &faceinfo);
    void saveFaceResultCallback();

    bool running_;
    cv::Mat faceMat_;

    int interval_time_;
    std::thread *taskThread_;
    std::mutex matMutex_;
    FaceDetect *faceDetectObj_;
    WebsocketServer *webServer_;
    bool detectStatus_;

    std::vector<FaceRecord> faceRecordVector_;
    std::mutex faceRecordMutex_;
    std::thread *saveRecordThread_;
    bool waitSaved_;
    config::Config *config_;
};

#endif // FACEIDENTIFY_H
