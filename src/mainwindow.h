#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>

#include "facedetect.h"
#include "faceidentify.h"
#include "hahacore.h"
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

    // bool eventFilter(QObject *obj, QEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

protected slots:
    void slot_getHahaImage(QImage img);
    void slot_getHahaImage1(cv::Mat mat);
    void slot_setHahaEffect();
    void slot_closeWindow();

private:
    void init();
    void initConnect();
    void initUi();
    void initMenu();
    void registerType();
    void fillRects(cv::Mat mat, std::vector<FaceDetectResult> &);
    void setHahaEffect(const QString &effect);

    Ui::MainWindow *ui;
    bool pressed_;
    QPoint m_point;
    QMenu *rootMenu_;
    QMenu *hahaMenu_;
    QAction *closeAction_;
    QMenu *modeMenu_;

    ProducerRecordImpl *cameraServer_;
    PlayCamera *cameraClient_;
    WebsocketServer *webServer_;
    FaceDetect *facedetect_;
    Hahacore *hahaCore_;
    FaceIdentify *faceIdentify_;
};
#endif // MAINWINDOW_H
