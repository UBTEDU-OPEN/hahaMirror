#include "consumer_exception.h"

#include <ostream>
#include <thread>

#include "common/fileopt.h"
#include "common/logging.h"
#include "common/processinfo.h"
#include "common/run_cmd.h"
#include "common/time.h"
#include <boost/filesystem.hpp>

namespace
{
   static const std::string g_base_color_mat_name = "color_";
static const std::string g_base_depth_mat_name = "depth_";
} // namespace

ConsumerException::ConsumerException()
    : running_(false)
    , havePersonIdentify_(true)
    , base_save_dir_(common::ProcessInfo::exeDirPath() + "/exception/")
    , save_days_(1)
    , compress_monitor_(base_save_dir_)
{
}

ConsumerException::~ConsumerException()
{
    LOG_DEBUG("exiting...");
    if (running_)
    {
        stop();
    }
    LOG_DEBUG("exited!!");
}

void ConsumerException::init()
{
    handleRecordThread_ = new std::thread(&ConsumerException::handle, this);
    saveThread_ = new std::thread(&ConsumerException::saveVideoCallback, this);
    postBackThread_ = new std::thread(&ConsumerException::postBackVideoCallback, this);
    compressThread_ = new std::thread(&ConsumerException::compressVideoDirCallback, this);
    storageThread_ = new std::thread(&ConsumerException::storageMonitorCallback, this);
    tarThread_ = new std::thread(&ConsumerException::tarDirCallback, this);

    common::fileopt::createDirectory(base_save_dir_);
}

void ConsumerException::start()
{
    running_ = true;
    init();
}

void ConsumerException::stop()
{
    running_ = false;
    //  LOG_DEBUG("here1");
    if (saveThread_)
    {
        if (saveThread_->joinable())
        {
            delete saveThread_;
        }
    }
    //   LOG_DEBUG("here2");
    if (postBackThread_)
    {
        if (postBackThread_->joinable())
        {
            delete postBackThread_;
        }
    }
    //   LOG_DEBUG("here3");
    if (handleRecordThread_)
    {
        if (handleRecordThread_->joinable())
        {
            delete handleRecordThread_;
        }
    }
    //  LOG_DEBUG("here4");
    if (compressThread_)
    {
        if (compressThread_->joinable())
        {
            delete compressThread_;
        }
    }
    //  LOG_DEBUG("here5");
    if (storageThread_)
    {
        if (storageThread_->joinable())
        {
            delete storageThread_;
        }
    }
    //  LOG_DEBUG("here6");
    if (tarThread_)
    {
        if (tarThread_->joinable())
        {
            delete tarThread_;
        }
    }
    //   LOG_DEBUG("here7");
}

void ConsumerException::consumeRecord(const cv::Mat color_mat, const cv::Mat depth_mat)
{
    std::unique_lock<std::mutex> record_unique(record_mutex_, std::try_to_lock);
    if (record_unique.owns_lock()) // 如果拿到锁
    {
        color_mat_ = color_mat;
        depth_mat_ = depth_mat;
    }
}

void ConsumerException::handle()
{
    while (running_)
    {
        if (color_mat_.empty() || depth_mat_.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        std::unique_lock<std::mutex> record_unique(record_mutex_);

        pushSaveMat();

        usleep(1 * 1000);

        color_mat_.release();
        depth_mat_.release();
    }
}

void ConsumerException::pushSaveMat()
{
    static int mat_index = 1;
    static std::string save_dir = "";
    // LOG_DEBUG("pushSaveMat");
    // LOG_DEBUG("target: {}, user_id: {}, h:{},w:{}",
    //           person_.has_target,
    //           person_.user_id,
    //           person_.rect.height,
    //           person_.rect.width);
    // LOG_DEBUG("target: {}, user_id: {}， last_target: {}",
    //           person_.has_target,
    //           person_.user_id,
    //           last_person_.has_target);
    // 改变存放的目录
    //    if (person_.has_target)
    //    {
    //        if (havePersonIdentify_)
    //        {
    //            if (person_.user_id != "" && last_person_.has_target == false)
    //            {
    //                save_dir = base_save_dir_ + jump::time::timeFormatStamp() + "-" + person_.user_id
    //                           + "/";
    //                mat_index = 1;
    //            }
    //        }
    //        else
    //        {
    //            if (last_person_.has_target == false)
    //            {
    //                save_dir = base_save_dir_ + jump::time::timeFormatStamp() + "-" + "model" + "/";
    //                mat_index = 1;
    //            }
    //        }
    //    }

    //    if (last_person_.has_target == true)
    //    {
    //        if (havePersonIdentify_ && last_person_.user_id == "")
    //        {
    //            LOG_DEBUG("user id is empty!!");
    //            return;
    //        }

    //        std::string color_mat_name = save_dir + g_base_color_mat_name + std::to_string(mat_index)
    //                                     + ".png";
    //        save_mats_.push({color_mat_name, color_mat_.clone()});

    //        std::string depth_mat_name = save_dir + g_base_depth_mat_name + std::to_string(mat_index)
    //                                     + ".tiff";
    //        save_mats_.push({depth_mat_name, depth_mat_.clone()});

    //        // LOG_DEBUG(
    //        //     "color_name: {}, depth_nane: {}", color_mat_name,
    //        //     depth_mat_name);
    //        ++mat_index;
    //    }
}

bool ConsumerException::postBackZip(std::string zip_name)
{
    time(NULL);

    int val = rand() % 2;
    if (val == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::vector<std::string> CompressMonitor::findObsoleteCompressFile(const std::string dir,
                                                                   const std::string ext,
                                                                   const int days)
{
    using namespace boost::filesystem;

    const int64_t max_diff = 3 * 24 * 60 * 60;
    std::vector<std::string> vec;
    try
    {
        for (directory_iterator itr(dir); itr != directory_iterator(); ++itr)
        {
            if (is_regular_file(itr->status()))
            {
                std::string name = itr->path().c_str();
                int64_t cur = common::time::getCurrentTime();
                if (name.find(ext) != std::string::npos)
                {
                    if (cur - last_write_time(name) > max_diff)
                    {
                        vec.emplace_back(name);
                    }
                    else
                    {
                        // LOG_DEBUG("{} is new, time is {}!!",
                        //           name,
                        //           creation_time(name));
                    }
                }
            }
        }
    }
    catch (boost::filesystem::filesystem_error &e)
    {
        LOG_DEBUG(e.what());
    }

    return vec;
}

void ConsumerException::storageMonitorCallback()
{
    while (running_)
    {
        auto files = compress_monitor_.findObsoleteCompressFile(base_save_dir_,
                                                                "tar.xz",
                                                                save_days_);
        if (files.empty())
        {
            sleep(1);
            continue;
        }

        for (auto it = files.begin(); it != files.end(); ++it)
        {
            remove(it->c_str());
        }
    }
}

void ConsumerException::saveVideoCallback()
{
    using namespace common::fileopt;

    static int64_t start_time;
    static const int64_t max_different_time = 5;
    static std::string last_dir_name = "";
    static int count = 0;
    while (running_)
    {
        if (save_mats_.empty())
        {
            int64_t time_diff = common::time::getCurrentTime() - start_time;
            // LOG_DEBUG("timediff: {}", time_diff);
            if (count > 0 && time_diff > max_different_time)
            {
                LOG_DEBUG("[autosave] push compress_dir {}", last_dir_name);
                tar_dirs_.push(last_dir_name);

                count = 0;
                last_dir_name = "";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
            continue;
        }

        start_time = common::time::getCurrentTime();
        int size = save_mats_.size();
        for (int i = 0; i < size; ++i)
        {
            auto front = save_mats_.front();

            std::string name = front.first;
            auto mat = front.second;

            std::string dir_name = name.substr(0, name.rfind("/"));
            if (last_dir_name == "")
            {
                last_dir_name = dir_name;
            }

            // LOG_DEBUG("name: {} , last: {}, cur: {}",
            //           front.first,
            //           last_dir_name,
            //           dir_name);
            if (!isDir(dir_name))
            {
                if (createDirectory(dir_name))
                {
                    cv::imwrite(name, mat);
                }
                else
                {
                    LOG_ERROR("create savedir failed!");
                }
            }
            else
            {
                cv::imwrite(name, mat);
            }

            if (last_dir_name != dir_name)
            {
                count = 0;
                LOG_DEBUG("push compress_dir {}", last_dir_name);
                tar_dirs_.push(last_dir_name);
            }
            else
            {
                count++;
            }

            last_dir_name = dir_name;

            save_mats_.pop();
        }
    }
}

void ConsumerException::postBackVideoCallback()
{
    while (running_)
    {
        if (post_zips_.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(30));
            continue;
        }

        // LOG_DEBUG("postBack");

        int size = post_zips_.size();
        bool send_ret;
        int remove_ret;
        for (int i = 0; i < size; ++i)
        {
            std::string zipname = post_zips_.front();
            send_ret = postBackZip(zipname);
            if (send_ret == true)
            {
                // LOG_DEBUG("postback success!");
                remove_ret = remove(zipname.c_str());
                if (remove_ret == 0)
                {
                    LOG_INFO("remove zip file \"{}\" success!", zipname);
                }
                else
                {
                    LOG_ERROR("remove zip file \"{}\" failed!", zipname);
                }
            }
            else
            {
                LOG_ERROR("postback {} failed!", zipname);
            }

            post_zips_.pop();
        }
    }
}

static bool compress(std::string outname, std::string tarname)
{
    // boost::filesystem::path path(tarname);
    // if (!is_directory(path))
    // {
    //     LOG_ERROR("{} is not directory!!", tarname);
    //     return false;
    // }
    // std::string cmd = "tar cfJ " + outname + " -C " + tarname + " .";
    // runBlockingCmd(cmd);

    FILE *fp;
    char cmd[1024];
    char buffer[1024];
    sprintf(cmd, "xz  -T 0 -z %s", tarname.c_str());
    fp = popen(cmd, "r");
    fgets(buffer, sizeof(buffer), fp);
    printf("% s\n", buffer);
    pclose(fp);

    if (errno)

        return true;
}

static bool tar(std::string outname, std::string dirname)
{
    boost::filesystem::path path(dirname);
    if (!is_directory(path))
    {
        LOG_ERROR("{} is not directory!!", dirname);
        return false;
    }
    // std::string cmd = "tar cfJ " + outname + " -C " + dirname + " .";
    // runBlockingCmd(cmd);

    FILE *fp;
    char cmd[1024];
    char buffer[1024];
    sprintf(cmd, "tar cf %s -C %s .", outname.c_str(), dirname.c_str());
    fp = popen(cmd, "r");
    fgets(buffer, sizeof(buffer), fp);
    printf("% s\n", buffer);
    pclose(fp);

    return true;
}

void ConsumerException::tarDirCallback()
{
    while (running_)
    {
        if (tar_dirs_.empty())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }

        LOG_DEBUG("tar-ing...");

        int size = tar_dirs_.size();
        for (int i = 0; i < size; ++i)
        {
            std::string dirname = tar_dirs_.front();
            std::string tarname = dirname;
            if (dirname[dirname.size() - 1] == '/')
            {
                tarname.pop_back();
            }
            tarname += ".tar";

            // if (common::Zip::compressDir(zipname, dirname) == true)
            if (tar(tarname, dirname) == true)
            {
                common::fileopt::removeNotEmptyDirectory(dirname.c_str());
                LOG_DEBUG("tar  {} success!", tarname);
                // 压缩
                compress_dirs_.push(tarname);
            }
            else
            {
                LOG_ERROR("tar {} failed!", dirname);
            }

            // 删除目录
            tar_dirs_.pop();
        }

        LOG_DEBUG("tar-ed!!");
    }
}

void ConsumerException::compressVideoDirCallback()
{
    while (running_)
    {
        if (compress_dirs_.empty())
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        LOG_DEBUG("compressing");

        int size = compress_dirs_.size();
        for (int i = 0; i < size; ++i)
        {
            std::string tarname = compress_dirs_.front();
            std::string xzname = tarname;
            xzname += ".xz";

            // if (common::Zip::compressDir(zipname, dirname) == true)
            if (compress(xzname, tarname) == true)
            {
                LOG_DEBUG("compress  {} success!", xzname);
                // 回传
                // post_zips_.push(zipname);
            }
            else
            {
                LOG_ERROR("compress {} failed!", tarname);
            }

            // 删除目录
            compress_dirs_.pop();
        }

        LOG_DEBUG("compressed!!");
    }
}
