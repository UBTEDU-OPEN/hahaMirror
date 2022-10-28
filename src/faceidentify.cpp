#include "faceidentify.h"
#include "briefhttpclient.h"
#include "common/json.hpp"
#include "common/logging.h"
#include "common/time.h"
#include "facedetect.h"
#include "websocket_server.h"

FaceIdentify::FaceIdentify()
    : running_(false)
    , frame_rate_(10)
    , interval_time_(100)
    , url_("")
    , taskThread_(nullptr)
    , faceDetectObj_(nullptr)
    , webServer_(nullptr)
{
}

FaceIdentify::~FaceIdentify()
{
    stop();
}

void FaceIdentify::start()
{
    running_ = true;
    init();
}

void FaceIdentify::stop()
{
    running_ = false;
    if (taskThread_ && taskThread_->joinable())
    {
        taskThread_->join();
    }
}

void FaceIdentify::init()
{
    taskThread_ = new std::thread(std::bind(&FaceIdentify::handleTaskCallback, this));
}

void FaceIdentify::consumeRecord(const cv::Mat color_mat, const cv::Mat original_mat)
{
    std::lock_guard<std::mutex> guard(matMutex_);
    if (!faceMat_.empty())
    {
        faceMat_.release();
    }
    faceMat_ = original_mat.clone();
}

void FaceIdentify::setWebsocketServerOject(WebsocketServer *server)
{
    webServer_ = server;
}

bool FaceIdentify::parseHttpResponse(std::string &response, ShowFaceInfo &info)
{
    using namespace nlohmann;
    bool rret = false;
    try
    {
        json j = json::parse(response);
        int code = j["code"].get<int>();
        std::string message = j["message"].get<std::string>();
        if (code == 0 || message == "ok")
        {
            auto data = j["data"];

            info.age = data["age"].get<int>();
            info.user_id = data["userId"].get<std::string>();
            info.name = data["name"].get<std::string>();
            int gender = data["gender"].get<int>();
            if (gender == 0)
            {
                info.sex = "男";
            }
            else if (gender == 1)
            {
                info.sex = "女";
            }
            else
            {
                info.sex = "不详";
            }

            int mask = data["faceMask"].get<int>();
            if (mask == 0)
            {
                info.mask = true;
            }
            else if (mask == 1)
            {
                info.mask = false;
            }
            else
            {
                info.mask = false;
            }
            info.smile_level = data["smile"].get<int>();
            if (info.smile_level < 0)
            {
                info.smile_level = 0;
            }
            rret = true;
        }
    }
    catch (const json::parse_error &error)
    {
        LOG_WARN("parse error: {}", error.what());
        rret = false;
    }
    catch (const json::type_error &error)
    {
        LOG_WARN("type error: {}", error.what());
        rret = false;
    }

    return rret;
}

void FaceIdentify::httpIdentifyRequest(cv::Mat mat)
{
    std::vector<uint8_t> sourceImg;
    sourceImg.resize(2 * 1024 * 1024);
    imencode(".jpeg", mat, sourceImg);
    int size = sourceImg.size();

    uint8_t *tmp = new uint8_t[size];
    memcpy(tmp, (uint8_t *) &sourceImg[0], size); // sourceImg.data()
    std::string response;
    BriefHttpClient client(url_);
    bool ret = client.faceIdentify(&tmp[0], size, response);
    if (ret)
    {
        // 解析repsonse
        ShowFaceInfo faceinfo;
        ret = parseHttpResponse(response, faceinfo);
        if (webServer_ && ret)
        {
            // LOG_DEBUG("**********");
            QByteArray ba((char *) tmp, size);
            faceinfo.face_image = ba.toBase64().toStdString();
            webServer_->appendMessage(std::move(faceinfo));
        }
        //  else
        //  {
        // LOG_DEBUG("@@@@@@@@@@");
        //  }
    }

    delete[] tmp;
    mat.release();
}

void FaceIdentify::handleTaskCallback()
{
    using namespace common::time;

    while (running_)
    {
        matMutex_.lock();
        if (faceMat_.empty())
        {
            matMutex_.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            continue;
        }

        uint64_t begin_point_time = getCurrentMilliTime();

        cv::Mat mat = faceMat_.clone();
        matMutex_.unlock();

        auto results = faceDetectObj_->getCurrentPersonResults();
        int size = results.size();

        if (size > 0)
        {
            std::thread *t[size];
            for (int i = 0; i < size; ++i)
            {
                cv::Mat face_mat = mat(results[i].bigFaceRect);
                t[i] = new std::thread(
                    std::bind(&FaceIdentify::httpIdentifyRequest, this, face_mat.clone()));
                face_mat.release();
            }

            std::thread *tt;
            for (int i = 0; i < size; ++i)
            {
                tt = t[i];
                if (tt && tt->joinable())
                    tt->join();
                delete tt;
            }
        }

        uint64_t end_point_time = getCurrentMilliTime();
        int diff = int(end_point_time - begin_point_time);
        if (diff < interval_time_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_time_ - diff));
        }
    }
}
