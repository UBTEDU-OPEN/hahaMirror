#include "imageoverlay.h"
#include "image.h"

#include <QPainter>
#include <QPixmap>

ImageOverlay::ImageOverlay() {}

void overlayImage(cv::Mat src_mat, const cv::Mat overlay_mat, const cv::Rect rect)
{
    cv::Mat overlay_mat_clone = overlay_mat.clone();
    QImage img_src = image::mat2qim(src_mat);
    QImage img_overlay = image::mat2qim(overlay_mat_clone);

    QPixmap pix_src = QPixmap::fromImage(img_src);
    QPixmap pix_overlay = QPixmap::fromImage(img_overlay);
    QPainter painter(&pix_src);
    painter.drawPixmap(0, 0, pix_overlay.width(), pix_overlay.height(), pix_overlay);
}
