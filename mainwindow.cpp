#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cameraServer_(0)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}
