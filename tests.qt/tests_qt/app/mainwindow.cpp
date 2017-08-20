#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <codex/codex.hpp>
#include <codex/error_code.hpp>
#include <codex/vision/image.hpp>

using namespace codex::vision;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    codex::vision::image<uint8_t> img(640,480);
    double x = 255.0f / img.width();
    for ( int r = 0 ; r < img.height() ;++r ){
        for ( int c = 0 ;c < img.width() ;++c){
            img.at(c,r) = c * x;
        }
    }
    QImage qimg(img.ptr() , img.width() , img.height() , QImage::Format_Grayscale8 );
    ui->label->setPixmap( QPixmap::fromImage(qimg));
}
