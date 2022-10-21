#ifndef HAHACORE_H
#define HAHACORE_H

#include <opencv2/opencv.hpp>

class hahacore
{
public:
    hahacore();
};

enum FaceLocation
{
    Eyes,
    Nose,
    Mouth,
    Brow,  // 眉毛
    Cheek, // 脸颊

};

/*
 * @brief: 哈哈镜接口
 * @parameter
    * face_mat: 经过截取后的人脸图片
    * type: 施加特效的人脸部位
 *
*/

void addFaceEffects(cv::Mat &face_mat, const FaceLocation type);

#endif // HAHACORE_H
