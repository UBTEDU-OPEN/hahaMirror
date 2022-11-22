#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>

#include "config.h"
#include "facedetect.h"
#include "faceidentify.h"
#include "hahacore.h"
#include "hahaui.h"
#include "playcamera.h"
#include "producer_record_impl.h"
#include "resolution.h"
#include "websocket_server.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
class MainWindow;
}
QT_END_NAMESPACE

enum SleepStatus
{
    Sleeping = 0,
    Waving,
    Waved,
    Work,
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void start();
    void stop();

    // bool eventFilter(QObject *obj, QEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

protected slots:
    void slot_getHahaImage(QImage img);
    void slot_getHahaImage1(cv::Mat mat);
    void slot_setHahaEffect();
    void slot_closeWindow();
    void slot_faceCountChanged(int cur, int last);
    void slot_uiSleep();
    void slot_showtimeout();
    void slot_setArea();

private:
    void checkUiStatus();
    void init();
    void initConnect();
    void initUi();
    void initLogger();
    void initMenu();
    void registerType();
    void selectScreen();
    void fillRects(cv::Mat mat, std::vector<FaceDetectResult> &);
    void setHahaEffect(const QString &effect);
    int getVideoDevice();

    Ui::MainWindow *ui;
    bool pressed_;
    bool showRectangle_;
    bool showValidArea_;
    int lastEffect_;
    QPoint m_point;
    QMenu *rootMenu_;
    QMenu *hahaMenu_;
    QAction *closeAction_;
    QMenu *areaMenu_;
    cv::Size resolution_;
    QTimer *sleepTimer_;
    QTimer *showTimer_;

    ProducerRecordImpl *cameraServer_;
    PlayCamera *cameraClient_;
    WebsocketServer *webServer_;
    FaceDetect *facedetect_;
    Hahacore *hahaCore_;
    FaceIdentify *faceIdentify_;
    HahaUi *hahaUi_;
    Resolution *resolutionObject_;

    SleepStatus sleepStatus_;
    QRect deskRect_;
    std::mutex statusMutex_;
    cv::Mat curMat_;
    std::mutex matMutex_;

    config::Config *config_;
};
#endif // MAINWINDOW_H
