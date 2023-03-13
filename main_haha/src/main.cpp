#include <vector>
#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "face_recognition_api.h"
#include "face_haha_api.h"


int main(int argc, char* argv[])
{
    try
    {
        double t0, t1, t2, inferenceTime;

        // get version，版本号 100 为v1.0
        std::cout << HaHaFaceVision::UBT_AIGetFRVersion_x86() << std::endl;
        std::cout << HaHaFaceVision::UBT_AIGetHaHaVersion_x86() << std::endl;

        // init model
        int fr_handle;
        HaHaFaceVision::FACE_CONFIG_PARAMS config_params;
        config_params.imageWidth = 4000;
        config_params.imageHeight = 4000;
        config_params.minFace = 80; //最小人脸限制，最长边小于该数值的脸会过滤
        config_params.isColor = true;
        config_params.FaceLandmarkPose_flag = true;
        config_params.device_type = "CPU";
        HaHaFaceVision::UBT_AIFaceRecInit(argv[1], config_params, fr_handle, 8);

        // input and output settings
        cv::VideoCapture cap(0);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, 720);
        cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
        if (!cap.isOpened())
        {
            std::cout << "open camera error" << std::endl;
            return -1;
        }

        ///*
        std::vector<cv::String> filenames; //读图片
        cv::glob(argv[2], filenames);
        std::sort(filenames.begin(), filenames.end());
        for (auto img_name : filenames)
        {
            //t1 = static_cast<double>(cv::getTickCount());
            std::cout << "***********************************************************" << std::endl;
            std::cout << img_name << std::endl;
            // get input
            cv::Mat image;
            image = cv::imread(img_name);
            cv::transpose(image, image);
        //*/
        /*
        while (1) //读相机
        {
            //t1 = static_cast<double>(cv::getTickCount());
            std::cout << "***********************************************************" << std::endl;
            // get input
            cv::Mat image;
            cap >> image;

            //cv::transpose(image, image);
        */

            if (!image.data)
            {
                continue;
            }

            // mirror
            //HaHaFaceVision::mirroY(image, image);

            t0 = static_cast<double>(cv::getTickCount());
            // get output
            std::vector<HaHaFaceVision::FACE_DET_SINGLE_RESULT> results;
            HaHaFaceVision::UBT_AIFaceDetection(fr_handle, image, results);

            cv::Mat img_transformed;
            img_transformed = image.clone();
            for (auto &oneFace : results)
            {
                // 0：眼睛； 1：嘴； 2：脸颊；
                HaHaFaceVision::UBT_AIFaceHaha(img_transformed, img_transformed, oneFace.faceRect, oneFace.faceShape, 2, 20);
            }

            t2 = static_cast<double>(cv::getTickCount());
            inferenceTime = (t2 - t0) / cv::getTickFrequency() * 1000;
            std::cout << "total: " << inferenceTime << std::endl;

            cv::resize(img_transformed, img_transformed, cv::Size(360, 640));
            cv::imshow("img_transformed", img_transformed);
            cv::waitKey(1);
        }

        // uninit model
        HaHaFaceVision::UBT_AIFaceRecUninit(fr_handle);
    }

    catch (const std::exception& error)
    {
        std::cerr << "[ ERROR ] " << error.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cerr << "[ ERROR ] Unknown/internal exception happened." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "[ INFO ] Execution successful" << std::endl;
    return EXIT_SUCCESS;
}
