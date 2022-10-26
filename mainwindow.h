#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "facedetect.h"
#include "playcamera.h"
#include "producer_record_impl.h"
#include "websocket_server.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void start();
    void stop();

protected slots:
    void slot_getHahaImage(QImage img);
    void slot_getHahaImage1(cv::Mat mat);

private:
    void init();
    void initConnect();
    void registerType();

    Ui::MainWindow *ui;

    ProducerRecordImpl cameraServer_;
    PlayCamera cameraClient_;
    WebsocketServer *webServer_;
    FaceDetect facedetect_;
};
#endif // MAINWINDOW_H
