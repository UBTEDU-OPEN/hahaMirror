#include "putpng.h"

void putTranslucentPicture(cv::Mat& src_img, cv::Mat& logo_img, cv::Point point)
{
    cv::Mat imgPart4logo = src_img(
        cv::Rect(point.x, point.y, logo_img.cols, logo_img.rows)); // logo_img.rows
    cv::Mat dstImage;
    cv::addWeighted(logo_img,
                    1.0,
                    imgPart4logo, // 第二个参数表示字体的明亮度， 第4个参数表示背景的透明度。
                    0.3,
                    0.0,
                    dstImage);
    dstImage.copyTo(
        imgPart4logo); // // 将 logo_img 拷贝到 imgPart4logo空白区域，得到一个有透明背景的图片。
}

// void putPicture(cv::Mat& src_img, cv::Mat& logo_img, cv::Mat& logo_mask, cv::Point point)
// {
//     cv::Mat imgPart4logo=src_img(cv::Rect(point.x, point.y, logo_img.cols, logo_img.rows));
//     logo_img.copyTo(imgPart4logo, logo_mask);
// }

void putCountDownBackgroundPicture()
{
    // float scale_factor_x = ;
    // float scale_factor_y = ;
}

void putCountDownPicture() {}

void putTipsPicture() {}
