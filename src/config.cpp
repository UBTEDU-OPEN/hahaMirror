#include "config.h"

#include <iostream>

#include "common/fileopt.h"
#include "common/logging.h"
#include "common/processinfo.h"

using namespace config;

Config::Config()
    : path_(common::ProcessInfo::exeDirPath() + "/config/")
{
    init();
}

void Config::init()
{
    using namespace common::fileopt;

    if (!isDir(path_))
    {
        if (!createDirectory(path_))
        {
            std::cerr << "create config directory failed!!" << std::endl;
            abort();
        }
    }

    std::string filepath = path_ + "config.ini";
    settings_ = new QSettings(QString::fromStdString(filepath), QSettings::IniFormat);

    if (!isExsit(filepath))
    {
        settings_->beginGroup("Camera");
        settings_->setValue("width", camera_.width);
        settings_->setValue("height", camera_.height);
        settings_->setValue("direction", camera_.direction);
        settings_->endGroup();

        settings_->beginGroup("Http");
        settings_->setValue("save_camera_time_url",
                            QString::fromStdString(http_.save_camera_time_url));
        settings_->setValue("face_identfiy_url", QString::fromStdString(http_.face_identfiy_url));
        settings_->setValue("identify_interval_time_ms", http_.identify_interval_time_ms);
        settings_->endGroup();

        settings_->beginGroup("Detect");
        settings_->setValue("faceMin", detect_.faceMin);
        settings_->setValue("interval_time_ms", detect_.interval_time_ms);
        settings_->setValue("max_face_num", detect_.max_face_num);
        settings_->endGroup();

        settings_->beginGroup("Haha");
        settings_->setValue("interval_time_ms", haha_.interval_time_ms);
        settings_->endGroup();

        settings_->beginGroup("Basic");
        settings_->setValue("log_level", basic_.log_level);
        settings_->endGroup();
    }
    else
    {
        camera_.width = settings_->value("Camera/width", camera_.width).toInt();
        camera_.height = settings_->value("Camera/height", camera_.height).toInt();
        camera_.direction = settings_->value("Camera/direction", camera_.direction).toInt();

        http_.save_camera_time_url = settings_
                                         ->value("Http/save_camera_time_url",
                                                 QString::fromStdString(http_.save_camera_time_url))
                                         .toString()
                                         .toStdString();
        http_.face_identfiy_url = settings_
                                      ->value("Http/face_identfiy_url",
                                              QString::fromStdString(http_.face_identfiy_url))
                                      .toString()
                                      .toStdString();
        http_.identify_interval_time_ms = settings_
                                              ->value("Http/identify_interval_time_ms",
                                                      http_.identify_interval_time_ms)
                                              .toInt();

        detect_.faceMin = settings_->value("Detect/faceMin", detect_.faceMin).toInt();
        detect_.max_face_num = settings_->value("Detect/max_face_num", detect_.max_face_num).toInt();
        detect_.interval_time_ms
            = settings_->value("Detect/interval_time_ms", detect_.interval_time_ms).toInt();

        basic_.log_level = settings_->value("Basic/log_level", basic_.log_level).toInt();
    }
}
