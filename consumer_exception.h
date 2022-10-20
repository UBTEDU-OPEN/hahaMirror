#ifndef JUMP_CORE_CONSUMER_Exception_H
#define JUMP_CORE_CONSUMER_Exception_H

#include "consumer_base.h"
#include <gtest/gtest.h>
#include <mutex>

namespace std
{
class thread;
}

class CompressMonitor
{
public:
    CompressMonitor(std::string dir)
        : dir_(dir)
    {
    }
    std::vector<std::string> findObsoleteCompressFile(const std::string dir,
                                                      const std::string ext = ".tar.xz",
                                                      const int days = 3);

    void setMonitorDir(std::string dir) { dir_ = dir; }
    void setSaveDays(int days);

private:
    std::string dir_;
};

class ConsumerException : public ConsumerRecord
{
public:
    ConsumerException();
    ~ConsumerException();

    void start() override;
    void stop() override;

    void consumeRecord(const cv::Mat color_mat, const cv::Mat depth_mat) override;

    void setPersonIdentify(bool flag) { havePersonIdentify_ = flag; }
    void setSaveDir(std::string dir)
    {
        base_save_dir_ = dir;
        compress_monitor_.setMonitorDir(dir);
    }
    void setSaveDays() { save_days_ = 3; }

protected:
    void handle();

private:
    void init();
    void pushSaveMat();
    bool postBackZip(std::string zip_name);

    void saveVideoCallback();
    void postBackVideoCallback();
    void compressVideoDirCallback();
    void storageMonitorCallback();
    void tarDirCallback();

    bool running_;

    std::thread *saveThread_;
    std::thread *postBackThread_;
    std::thread *compressThread_;
    std::thread *handleRecordThread_;
    std::thread *storageThread_;
    std::thread *tarThread_;

    std::mutex record_mutex_;
    cv::Mat color_mat_;
    cv::Mat depth_mat_;

    bool havePersonIdentify_;
    std::mutex person_mutex_; // 因为是取算法处理的帧，而不是相机输出的实时帧，
                              // 所以可以和 record_mutex_ 合为一个锁

    std::string base_save_dir_;
    int save_days_;
    std::queue<std::pair<std::string, cv::Mat>> save_mats_;
    std::queue<std::string> compress_dirs_;
    std::queue<std::string> post_zips_;
    std::queue<std::string> tar_dirs_;
    CompressMonitor compress_monitor_;
};

#endif // JUMP_CORE_CONSUMER_IDENTIFY_H
