#include "faceidentify.h"
#include "briefhttpclient.h"
#include "common/json.hpp"
#include "common/logging.h"
#include "common/time.h"
#include "facedetect.h"
#include "websocket_server.h"

#include <QDateTime>

FaceIdentify::FaceIdentify()
    : running_(false)
    , interval_time_(2000)
    , taskThread_(nullptr)
    , faceDetectObj_(nullptr)
    , webServer_(nullptr)
    , detectStatus_(true)
    , waitSaved_(false)
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
    if (taskThread_)
    {
        if (taskThread_->joinable())
        {
            taskThread_->join();
        }

        delete taskThread_;
    }

    if (saveRecordThread_)
    {
        if (saveRecordThread_->joinable())
        {
            saveRecordThread_->join();
        }

        delete saveRecordThread_;
    }
}

void FaceIdentify::init()
{
    taskThread_ = new std::thread(std::bind(&FaceIdentify::handleTaskCallback, this));
    saveRecordThread_ = new std::thread(std::bind(&FaceIdentify::saveFaceResultCallback, this));
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

void FaceIdentify::saveFaceResultCallback()
{
    using namespace nlohmann;

    while (running_)
    {
        if (!waitSaved_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            continue;
        }
        waitSaved_ = false;

        std::vector<FaceRecord> record_vec;
        faceRecordMutex_.lock();
        int size = faceRecordVector_.size();
        if (size <= 0)
        {
            faceRecordMutex_.unlock();
            continue;
        }
        record_vec.swap(faceRecordVector_);
        faceRecordMutex_.unlock();

        json records;
        for (int i = 0; i < size; ++i)
        {
            json record;
            record["userId"] = record_vec[i].user_id;
            record["timestamp"] = record_vec[i].time;
            record["smile"] = record_vec[i].smile;
            records.push_back(std::move(record));
        }

        json params;
        params["cameraTag"] = "haha_camera";
        params["groupId"] = QDateTime::currentDateTime().toString("yyyyMMdd").toStdString();
        params["records"] = records;

        BriefHttpClient client(config_->http()->save_camera_time_url);
        std::string response;
        std::string request = params.dump(2);
        LOG_TRACE("request: {}", request);
        bool ret = client.saveFaceRecord((uint8_t *) request.c_str(), request.size(), response);
        if (ret)
        {
            try
            {
                auto jresp = json::parse(response);
                if (jresp["code"].get<int>() != 0)
                {
                    LOG_ERROR("save camera record error!!");
                }
            }
            catch (json::exception &e)
            {
                LOG_ERROR("json parse error: {}!!", e.what());
            }
        }
        else
        {
            LOG_ERROR("save camera record failed!!");
        }
    }
}

void FaceIdentify::httpIdentifyRequest(cv::Mat mat)
{
    // cv::imshow("face", mat);
    std::vector<uint8_t> sourceImg;
    sourceImg.resize(2 * 1024 * 1024);
    imencode(".jpeg", mat, sourceImg);
    int size = sourceImg.size();

    uint8_t *tmp = new uint8_t[size];
    memcpy(tmp, (uint8_t *) &sourceImg[0], size); // sourceImg.data()
    std::string response;
    BriefHttpClient client(config_->http()->face_identfiy_url);
    bool ret = client.faceIdentify(&tmp[0], size, response);
    if (ret)
    {
        // 解析repsonse
        ShowFaceInfo faceinfo;
        ret = parseHttpResponse(response, faceinfo);

        FaceRecord record;
        record.user_id = faceinfo.user_id;
        record.time = QDateTime::currentDateTime().toTime_t();
        record.smile = faceinfo.smile_level;
        faceRecordMutex_.lock();
        faceRecordVector_.push_back(std::move(record));
        faceRecordMutex_.unlock();
        if (webServer_ && ret)
        {
            // LOG_DEBUG("**********");
            QByteArray ba((char *) tmp, size);
            faceinfo.face_image = ba.toBase64().toStdString();
            webServer_->appendMessage(std::move(faceinfo));
        }
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
        if (faceMat_.empty() || detectStatus_ == false)
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

        //  LOG_DEBUG("size : {}", size);

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

            // 上传
            waitSaved_ = true;
        }

        uint64_t end_point_time = getCurrentMilliTime();
        int diff = int(end_point_time - begin_point_time);
        if (diff < interval_time_)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval_time_ - diff));
        }
    }
}
