#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "playcamera.h"
#include "producer_record_impl.h"

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

private:
    Ui::MainWindow *ui;

    ProducerRecordImpl cameraServer_;
    PlayCamera cameraClient_;
};
#endif // MAINWINDOW_H
