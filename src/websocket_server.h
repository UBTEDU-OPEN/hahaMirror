#ifndef JUMP_WEBSOCKETSERVER_H
#define JUMP_WEBSOCKETSERVER_H
#include <mutex>
#include <set>
#include <string>

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

typedef struct _ShowFaceInfo
{
    std::string name;
    std::string user_id;
    int age;
    std::string sex;
    bool mask;
    int smile_level;
    std::string face_image;
    int64_t produce_time;
    _ShowFaceInfo()
    {
        name = "张三";
        user_id = "user_0";
        age = 18;
        sex = "男";
        mask = true;
        smile_level = 70; // 满分100
        face_image = "";
        produce_time = 0;
    }

    _ShowFaceInfo operator=(const _ShowFaceInfo& data)
    {
        name = data.name;
        user_id = data.user_id;
        age = data.age;
        sex = data.sex;
        mask = data.mask;
        smile_level = data.smile_level;
        face_image = data.face_image;
        produce_time = data.produce_time;
        return *this;
    }

} ShowFaceInfo;

class WebsocketServer
{
public:
    WebsocketServer();
    ~WebsocketServer();
    void start(uint16_t port);
    void startVirtualServer(uint16_t port);
    void stop();
    bool hasClient();

    void appendMessage(ShowFaceInfo info);

private:
    typedef websocketpp::server<websocketpp::config::asio> server;
    typedef server::message_ptr message_ptr;
    typedef std::set<websocketpp::connection_hdl,
                     std::owner_less<websocketpp::connection_hdl>>
        con_list;

    void init();
    void on_open(websocketpp::connection_hdl hdl);
    void on_close(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, message_ptr msg);
    void startSendThread();
    void connectTaskThread();

    bool running_;
    server m_server_;
    con_list m_connections_;
    std::mutex m_mutex;
    std::mutex msgMutex_;
    std::queue<ShowFaceInfo> messages_;
    int port_;

    std::thread* connectThread_;
    std::thread* sendThread_;
};

#endif // JUMP_WEBSOCKETSERVER_H
