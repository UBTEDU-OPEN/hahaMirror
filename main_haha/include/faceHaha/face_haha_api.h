#ifndef FACE_HAHA_API_H
#define FACE_HAHA_API_H
#include <vector>
#include <iostream>
#include <opencv2/opencv.hpp>

namespace HaHaFaceVision {
/******************************************************************************************
Function:    mirroY
Description:   人脸左右镜像
Params:
src[IN]                 - 源图像
dst[OUT]                - 镜像后图像
*******************************************************************************************/
void mirroY(cv::Mat& src, cv::Mat& dst);

/******************************************************************************************
Function:    UBT_AIFaceHaha
Description:   选取五官特效变换
Params:
image[IN]                - 源图像 
faceRect[IN]		 - 要变换的人脸检测框
faceShape[IN]		 - 要变换的人脸关键点
type[IN]                 - 要变换的特效类型，0：眼睛； 1：嘴； 2：脸颊
beauty[IN]               - 美颜程度，0-100，0表示不美颜，100表示最大程度美颜
img_transformed[OUT]     - 转换后图像
*******************************************************************************************/
void UBT_AIFaceHaha(cv::Mat& image, cv::Mat& img_transformed,cv::Rect faceRect, cv::Point2f faceShape[5], int type, int beauty = 0);


/******************************************************************************************
Function:    UBT_AIGetHaHaVersion_x86
Description: 获取五官特效变换版本号
Return:
版本号 100 为v1.0.0
*******************************************************************************************/
int UBT_AIGetHaHaVersion_x86();
}

#endif //FACE_HAHA_API_H

