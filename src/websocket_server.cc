#include "websocket_server.h"
#include "image.h"

#include <atomic>
#include <iostream>
#include <sys/prctl.h>

#include <opencv2/opencv.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#include "common/json.hpp"
#include "common/logging.h"
#include "common/time.h"

#include <QByteArray>
#include <QDateTime>
#include <QDebug>

using websocketpp::connection_hdl;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::placeholders::_3;

std::atomic<int> g_msgIndex_(0);

WebsocketServer::WebsocketServer()
    : running_(false)
{
    m_server_.init_asio();
    m_server_.set_reuse_addr(true); // 关键， 可以复用TIME_WAIT状态的端口
    m_server_.set_open_handler(bind(&WebsocketServer::on_open, this, _1));
    m_server_.set_close_handler(bind(&WebsocketServer::on_close, this, _1));
    m_server_.set_message_handler(bind(&WebsocketServer::on_message, this, _1, _2));
}

WebsocketServer::~WebsocketServer()
{
    stop();
}

bool WebsocketServer::hasClient()
{
    return !m_connections_.empty();
}

void WebsocketServer::startSendThread()
{
    using namespace nlohmann;
    running_ = true;
    auto dispatchSendFunc = [&] {
        prctl(PR_SET_NAME, "WebsocketServer-startSendThread");
        while (running_)
        {
            msgMutex_.lock();
            if (messages_.empty())
            {
                msgMutex_.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }

            auto front = messages_.front();
            messages_.pop();
            msgMutex_.unlock();

            int index = g_msgIndex_++;
            json j;
            j["name"] = front.name;
            j["user_id"] = front.user_id;
            j["age"] = front.age;
            j["sex"] = front.sex;
            j["mask"] = front.mask;
            j["smile_level"] = front.smile_level;
            j["face_image"] = front.face_image;
            j["index"] = index;
            j["produce_time"] = front.produce_time;

            std::string sendtxt = j.dump(2);
            LOG_TRACE(sendtxt);

            common::time::TimeConsumingAnalysis analysis;

            for (auto it = m_connections_.begin(); it != m_connections_.end(); ++it)
            {
                std::string uri = m_server_.get_con_from_hdl(*it)->get_remote_endpoint();
                LOG_DEBUG("send uri {} index {}", uri, index);
                j["time"]
                    = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toStdString();
                j["send_time"] = QDateTime::currentMSecsSinceEpoch();
                m_server_.send(*it, j.dump(2), websocketpp::frame::opcode::text);
                analysis.addTimePoint("websocket send faceinfo");

                // json jack;
                // jack["index"] = index;
                // jack["time"] = QDateTime::currentMSecsSinceEpoch();
                // m_server_.send(
                //     *it, jack.dump(2), websocketpp::frame::opcode::text);
                // analysis.addTimePoint("websocket send faceinfo ack");

                // LOG_DEBUG(analysis.print());
            }
        }

        LOG_INFO("*****start to stop webserver*****");
        std::lock_guard<std::mutex> lock(m_mutex);
        std::string reason = "server normal close";
        for (auto it = m_connections_.begin(); it != m_connections_.end(); ++it)
        {
            m_server_.close(*it, websocketpp::close::status::normal, reason);
        }

        m_server_.stop_listening();
        m_server_.stop();

        LOG_INFO("*****finish to stop webserver*****");
    };

    sendThread_ = new std::thread(dispatchSendFunc);
}

static nlohmann::json getDefaultMsg()
{
    using namespace nlohmann;

    json j;

    return j;
}

void WebsocketServer::on_open(connection_hdl hdl)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connections_.insert(hdl);

    auto j = getDefaultMsg();
    LOG_DEBUG("websocket new connection into");
    std::string sendtxt = j.dump(2);
    m_server_.send(hdl, "new connection into", websocketpp::frame::opcode::text);
}

void WebsocketServer::on_close(connection_hdl hdl)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connections_.erase(hdl);
    std::string uri = m_server_.get_con_from_hdl(hdl)->get_remote_endpoint();

    LOG_DEBUG("erase uri {}!!", uri);
}

void WebsocketServer::on_message(websocketpp::connection_hdl hdl, message_ptr msg)
{
    std::string payload = msg->get_payload();
    std::string uri = m_server_.get_con_from_hdl(hdl)->get_remote_endpoint();
    LOG_DEBUG("receive uri: {}, payload: {}", uri, payload);
}

void WebsocketServer::connectTaskThread()
{
    try
    {
        // Set logging settings
        m_server_.set_access_channels(websocketpp::log::alevel::all);
        m_server_.clear_access_channels(websocketpp::log::alevel::frame_payload);
        m_server_.clear_access_channels(websocketpp::log::alevel::frame_header); // frame_payload

        // Register our message handler
        m_server_.listen(port_);
        m_server_.start_accept();
        m_server_.run();
    }
    catch (websocketpp::exception const &e)
    {
        std::cout << e.what() << std::endl;
        LOG_ERROR(e.what());
        abort();
    }
    catch (...)
    {
        std::string err = "other exception";
        std::cout << err << std::endl;
        LOG_ERROR(err);
        abort();
    }
}

static std::string getHumanPicture(int id)
{
    using namespace std;

    std::string picture = "";
    static const string pic1 = "/home/ubt/Pictures/haha7.jpeg";
    static const string pic2 = "/home/ubt/Pictures/haha2.jpeg";
    static const string pic3 = "/home/ubt/Pictures/haha3.jpeg";
    static const string pic4 = "/home/ubt/Pictures/haha6.jpeg";
    static const string pic5 = "/home/ubt/Pictures/haha5.jpeg";
    if (id == 0)
    {
        picture = pic1;
    }
    else if (id == 1)
    {
        picture = pic2;
    }
    else if (id == 2)
    {
        picture = pic3;
    }
    else if (id == 3)
    {
        picture = pic4;
    }
    else if (id == 4)
    {
        picture = pic5;
    }
    else
    {
        picture = "/home/ubt/Pictures/haha1.jpeg";
    }

    return image::img2base64(picture, "jpeg");
}

void WebsocketServer::startVirtualServer(uint16_t port)
{
    start(port);
    int64_t num = 0;
    auto virtualDataCreateFunc = [&] {
        time(NULL);

        while (running_)
        {
            ShowFaceInfo info;

            int val = rand() % 5;
            if (val == 0)
            {
                info.name = "张三";
                info.user_id = "user_0";
                info.age = 31;
                info.sex = "男";
                info.mask = true;
                info.smile_level = 70;
            }
            else if (val == 1)
            {
                info.name = "小红";
                info.user_id = "user_3";
                info.age = 24;
                info.sex = "女";
                info.mask = false;
                info.smile_level = 30;
            }
            else if (val == 2)
            {
                info.name = "花花";
                info.user_id = "user_5";
                info.age = 9;
                info.sex = "女";
                info.mask = false;
                info.smile_level = 100;
            }
            else if (val == 3)
            {
                info.name = "大壮";
                info.user_id = "user_6";
                info.age = 31;
                info.sex = "男";
                info.mask = false;
                info.smile_level = 90;
            }
            else if (val == 4)
            {
                info.name = "小美";
                info.user_id = "user_7";
                info.age = 27;
                info.sex = "女";
                info.mask = false;
                info.smile_level = 0;
            }

            // qDebug() << "val: " << val;

            info.face_image = getHumanPicture(val);

            appendMessage(std::move(info));
            if (num++ >= 3)
            {
                sleep(2);
            }
        }
    };

    std::thread t(virtualDataCreateFunc);
    t.detach();
}

void WebsocketServer::start(uint16_t port)
{
    running_ = true;
    port_ = port;
    init();
}

void WebsocketServer::stop()
{
    running_ = false;
    if (sendThread_)
    {
        if (sendThread_->joinable())
        {
            sendThread_->join();
        }

        delete sendThread_;
    }

    if (connectThread_)
    {
        if (connectThread_->joinable())
        {
            connectThread_->join();
        }
        delete connectThread_;
    }
}

void WebsocketServer::init()
{
    startSendThread();
    connectThread_ = new std::thread(std::bind(&WebsocketServer::connectTaskThread, this));
}

void WebsocketServer::appendMessage(ShowFaceInfo info)
{
    msgMutex_.lock();
    int size = messages_.size();
    while (size >= 3)
    {
        messages_.pop();
        --size;
    }

    messages_.push(std::move(info));

    msgMutex_.unlock();
}
