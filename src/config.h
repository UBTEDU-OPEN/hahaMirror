#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <QSettings>

namespace config
{
struct Camera
{
    int width = 1920;
    int height = 1080;
    int direction = 1; // 0 摄像头横着，1 摄像头竖着
};

struct Http
{
    std::string save_camera_time_url = "http://192.168.1.169:60901/api/v1/save_camera_time_record";
    std::string face_identfiy_url = "http://192.168.1.169:60901/api/v1/faceIdentify";
    int identify_interval_time_ms = 2000;
};

struct Detect
{
    int faceMin = 80;
    int max_face_num = 10;
    int interval_time_ms = 30;
};

struct Haha
{
    int interval_time_ms = 30;
};

struct Basic
{
    int log_level = 1; // 0: trace; 1: debug; 2: info; 3: error.
};

class Config
{
public:
    Config();
    ~Config()
    {
        if (settings_)
        {
            delete settings_;
        }
    }

    Camera *camera() { return &camera_; }
    Http *http() { return &http_; }
    Detect *detect() { return &detect_; }
    Haha *haha() { return &haha_; }
    Basic *basic() { return &basic_; }

private:
    void init();

    std::string path_;
    Camera camera_;
    Http http_;
    Detect detect_;
    Haha haha_;
    Basic basic_;

    QSettings *settings_;
};
} // namespace config

#endif // CONFIG_H
