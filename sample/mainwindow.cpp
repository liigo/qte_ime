#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->textEdit->setText("汉字TextEdit");
    ui->lineEdit_no_ime->setAttribute(Qt::WA_InputMethodEnabled, false);
}

MainWindow::~MainWindow()
{
    delete ui;
}
