#include "fileopt.h"
#include "checkcode.h"
#include "logging.h"

#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <fstream>
#include <sstream>

#include "common/checkcode.h"
#include "muduo/base/Exception.h"

namespace common
{
namespace fileopt
{
#ifdef linux
const std::string kSeparator = "/";
#elif _WIN32
const std::string kSeparator = "\\";
#endif

void saveImage(std::string filename, const uint8_t *facedata, const size_t facedata_len)
{
    int fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd != -1)
    {
        int size = ::write(fd, facedata, facedata_len);
        if (size != facedata_len)
        {
            LOG_WARN("storage error image failed!");
        }
        close(fd);
    }
    else
    {
        LOG_WARN("open {} error!", filename);
    }
}

std::string getFileContent(std::string filepath)
{
    std::ifstream input(filepath);
    if (!input.is_open())
    {
        throw muduo::Exception("configfile open failed!");
        return "";
    }

    std::stringstream buffer;
    buffer << input.rdbuf();

    return std::string(buffer.str());
}

std::string getFileMd5Value(std::string filepath)
{
    using namespace common;

    std::string md5_value;

    std::ifstream file(filepath.c_str(), std::ifstream::binary);
    if (!file.is_open())
    {
        throw muduo::Exception("configfile open failed!");
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string content(buffer.str());
    return common::checkcode::md5(content);
}

bool isDir(std::string dirpath)
{
    struct stat st;
    if (stat(dirpath.c_str(), &st) == 0)
    {
        if (!S_ISDIR(st.st_mode))
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool isExsit(std::string filepath)
{
    if (access(filepath.c_str(), F_OK) == 0)
    {
        return true;
    }

    return false;
}

static bool do_mkdir(const std::string dir)
{
    if (isExsit(dir))
    {
        if (!isDir(dir))
        {
            return false;
        }
    }
    else
    {
        if (mkdir(dir.c_str(), 0777) != 0)
        {
            return false;
        }
    }

    return true;
}
/// 也可以使用 boost 或者 std 的 filesystem 库
bool createDirectory(const std::string filepath)
{
    std::string file = filepath;
    if (filepath == "")
    {
        return false;
    }

    int size = filepath.size();
    int kSeparator_size = kSeparator.size();
    if (filepath.substr(size - kSeparator_size, kSeparator_size) != kSeparator)
    {
        file += kSeparator;
        size += kSeparator_size;
    }

    std::string dirname;
    size_t index = file.find(kSeparator, 0);
    while (index != std::string::npos)
    {
        std::string dirname = file.substr(0, index + kSeparator_size);
        if (!do_mkdir(dirname))
        {
            return false;
        }
        index = file.find(kSeparator, index + kSeparator_size);
    }

    return true;
}

bool removeEmptyDirectory(std::string dirpath)
{
    if (rmdir(dirpath.c_str()) != 0)
    {
        return false;
    }

    return true;
}

bool removeNotEmptyDirectory(const char *dir)
{
    char cur_dir[] = ".";
    char up_dir[] = "..";
    char dir_name[128];
    DIR *dirp;
    struct dirent *dp;
    struct stat dir_stat;

    // 参数传递进来的目录不存在，直接返回
    if (0 != access(dir, F_OK))
    {
        return true;
    }

    // 获取目录属性失败，返回错误
    if (0 > stat(dir, &dir_stat))
    {
        perror("get directory stat error");
        return false;
    }

    if (S_ISREG(dir_stat.st_mode))
    { // 普通文件直接删除
        remove(dir);
    }
    else if (S_ISDIR(dir_stat.st_mode))
    { // 目录文件，递归删除目录中内容
        dirp = opendir(dir);
        while ((dp = readdir(dirp)) != NULL)
        {
            // 忽略 . 和 ..
            if ((0 == strcmp(cur_dir, dp->d_name)) || (0 == strcmp(up_dir, dp->d_name)))
            {
                continue;
            }

            sprintf(dir_name, "%s/%s", dir, dp->d_name);
            removeNotEmptyDirectory(dir_name); // 递归调用
        }
        closedir(dirp);

        rmdir(dir); // 删除空目录
    }
    else
    {
        perror("unknow file type!");
    }

    return true;
}

} // namespace fileopt
} // namespace common
