#ifndef MULTIFACE_FACE_DETECTION_API_H
#define MULTIFACE_FACE_DETECTION_API_H
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

#include "../common/ai_common.h"
namespace HaHaFaceVision
{
/************************************************************
data struct
************************************************************/
typedef struct _FACE_CONFIG_PARAMS_
{
    unsigned int minFace = 40; // minimum face size
    unsigned int imageWidth;   // input image width
    unsigned int imageHeight;  // input image height
    bool isColor;              // if the input image size is fixex
    // attribute
    bool FaceLandmarkPose_flag = false;
    bool FaceMask_flag = false;
    bool FaceQuality_flag = false;

    std::string device_type = "CPU";
} FACE_CONFIG_PARAMS; //人脸相关参数

typedef struct _FACE_DET_SINGLE_RESULT_
{
    cv::Rect faceRect;                     // face rectangle
    int pointNum;                          // 0-no facial points
    cv::Point2f faceShape[AI_POINT_NUM];   // face feature points
    float facePose[3];                     // face pose: roll, pitch, yaw
    cv::Point2f facePosePoint[AI_Point98]; // face feature points
    bool is_sideFace = false;              // is sideface or not
    int mask_stat = 1;                     // 0-masked 1-unmasked 2-not masked well
    int faceQualityScore = 100;
} FACE_DET_SINGLE_RESULT; //人脸检测结构体

typedef struct _FACE_FEATURE_
{
    std::vector<float> pFeat; // the feature vector
} FACE_FEATURE;               //人脸特征提取结构体

/******************************************************************************************
Function:    UBT_AIFaceRecInit
Description: 人脸识别初始化
Params:
modelDir[IN]                - 模型路径
FACE_CONFIG_PARAMS[IN]      - 人脸参数
thread_num[IN]              - 推理线程数
faceHandle[OUT]             - face recognition handle
Return:
AI_OK                       - succeed
AI_FAIL                     - failed
AI_INVALID_PARAM		   - invalid parameter
AI_MEM_ALLOC_FAIL		   - memory allocation failed
*******************************************************************************************/
int UBT_AIFaceRecInit(std::string modelDir,
                      const FACE_CONFIG_PARAMS &configParam,
                      int &faceHandle,
                      int thread_num);

/******************************************************************************************
Function:    UBT_AIFaceRecUninit
Description: 人脸识别释放资源
Params:
faceHandle[IN]       - face recognition handle
Return:
AI_OK                      - succeed
AI_INVALID_PARAM		   - invalid parameter
*******************************************************************************************/
int UBT_AIFaceRecUninit(int faceHandle);

/******************************************************************************************
Function:    UBT_AIFaceDetection
Description: 人脸检测功能
Params:
faceHandle[IN]       - face recognition handle
imageData[IN]       - 传入图像
faceResults[OUT]    - 输出图像中的所有人脸结果
Return:
AI_OK                      - succeed
AI_FAIL                    - failed
AI_INVALID_PARAM		   - invalid parameter
AI_MEM_ALLOC_FAIL		   - memory allocation failed
*******************************************************************************************/
int UBT_AIFaceDetection(int faceHandle,
                        cv::Mat imageData,
                        std::vector<FACE_DET_SINGLE_RESULT> &faceResults);

/******************************************************************************************
Function:    UBT_AIGetFRVersion_x86
Description: 获取人脸识别版本号
Return:
版本号 100 为v1.0.0
*******************************************************************************************/
int UBT_AIGetFRVersion_x86();

} // namespace HaHaFaceVision

#endif // MULTIFACE_FACE_DETECTION_API_H
