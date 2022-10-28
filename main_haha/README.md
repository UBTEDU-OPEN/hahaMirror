# 功能介绍：哈哈镜算法
使用示例见main.cpp  
- 画面镜像，模拟镜子效果；  
- 检测画面中的所有人脸；  
- 对画面中的所有人脸进行美化；  
- 对画面中的所有人脸的特定部位施加特效；  

# 使用方法：
按照示例代码编译后执行可执行文件，需要注意OpenCV lib和算法so库需要加载到`LD_LIBRARY_PATH`中
`./main ../models/`  

# 输入及输出接口：
## ------------获取人脸检测版本号-------------
- 获取人脸检测版本号：  
    `Function:    UBT_AIGetFRVersion_x86`  
    `Description: 获取人脸识别版本号`  
    `Return: `  
    `   版本号 100 为v1.0.0`  
    `int UBT_AIGetFRVersion_x86();`  

## ------------获取哈哈镜特效版本号-------------
- 获取哈哈镜特效版本号：  
    `Function:    UBT_AIGetHaHaVersion_x86`  
    `Description: 获取五官特效变换版本号`  
    `Return: `  
    `   版本号 100 为v1.0.0`  
    `int UBT_AIGetHaHaVersion_x86();`  

## ------------初始化人脸模型-------------
- 输入模型的路径、推理设置、线程数，进行模型初始化：  
    `Function:    UBT_AIFaceRecInit`  
    `Description: 人脸识别初始化`  
    `Params: `  
    `   modelDir[IN]                - 模型路径`
    `   FACE_CONFIG_PARAMS[IN]      - 人脸参数`  
    `   thread_num[IN]              - 推理线程数`  
    `   faceHandle[OUT]             - face recognition handle`  
    `Return: `  
    `   AI_OK                       - succeed`
    `   AI_FAIL                     - failed`  
    `   AI_INVALID_PARAM		   - invalid parameter`  
    `   AI_MEM_ALLOC_FAIL		   - memory allocation failed`  
    `int UBT_AIFaceRecInit(std::string modelDir, const FACE_CONFIG_PARAMS &configParam, int &faceHandle, int thread_num);`  

## ------------画面镜像，模拟镜子效果-------------
- 输入原图像，输出镜像后的图像：  
    `Function:    mirroY`  
    `Description: 人脸左右镜像`  
    `Params: `  
    `   src[IN]                 - 源图像`
    `   dst[OUT]                - 镜像后图像`  
    `void mirroY(cv::Mat& src, cv::Mat& dst);`  

## ------------进行人脸检测和关键点提取-------------
- 输入图片，获得人脸检测和关键点提取结果：  
    `Function:    UBT_AIFaceDetection`  
    `Description: 人脸检测功能`  
    `Params: `  
    `   faceHandle[IN]       - face recognition handle`
    `   imageData[IN]       - 传入图像`  
    `   faceResults[OUT]    - 输出图像中的所有人脸结果`  
    `Return: `  
    `   AI_OK                       - succeed`
    `   AI_FAIL                     - failed`  
    `   AI_INVALID_PARAM		   - invalid parameter`  
    `   AI_MEM_ALLOC_FAIL		   - memory allocation failed`  
    `int UBT_AIFaceDetection(int faceHandle, cv::Mat imageData, std::vector<FACE_DET_SINGLE_RESULT> &faceResults);`  

## ------------进行人脸哈哈镜特效-------------
- 输入图片，并逐个输入图片中的人脸信息，进行特定部位的人脸变形：  
    `Function:    UBT_AIFaceHaha`  
    `Description: 选取五官特效变换`  
    `Params: `  
    `   image[IN]                - 源图像`
    `   faceRect[IN]		 - 要变换的人脸检测框`  
    `   faceShape[IN]		 - 要变换的人脸关键点`  
    `   type[IN]                 - 要变换的特效类型，0：眼睛； 1：嘴； 2：脸颊`  
    `   img_transformed[OUT]     - 转换后图像`  
    `void UBT_AIFaceHaha(cv::Mat& image, cv::Mat& img_transformed,cv::Rect faceRect,cv::Point2f faceShape[5], int type);`  

## ------------释放人脸模型-------------
- 函数结束时需要释放资源：  
    `Function:    UBT_AIFaceRecUninit`  
    `Description: 人脸识别释放资源`  
    `Params: `  
    `   faceHandle[IN]       - face recognition handle`  
    `Return: `  
    `   AI_OK                       - succeed`
    `   AI_INVALID_PARAM		   - invalid parameter`  
    `int UBT_AIFaceRecUninit(int faceHandle);`  

## _FACE_DET_SINGLE_RESULT_和_FACE_CONFIG_PARAMS_解析
- 参考face_recognition_api.h
