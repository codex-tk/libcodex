#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <codex/codex.hpp>
#include <codex/error_code.hpp>

#include <codex/vision/image_proc.hpp>

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
    codex::vision::image img(640,480);
    double x = 255.0f / img.width();
    double y = 255.0f / img.height();
    for ( std::size_t r = 0 ; r < img.height() ;++r ){
        for ( std::size_t c = 0 ;c < img.width() ;++c){
            img.at(c,r) = (c * x + r * y) / 2;
        }
    }
    QImage qimg(img.ptr() , img.width() , img.height() , QImage::Format_Grayscale8 );
    ui->label->setPixmap( QPixmap::fromImage(qimg));
    ui->label->setScaledContents( true );
    ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

void MainWindow::on_pushButton_2_clicked()
{
    codex::vision::image img = codex::vision::load_from("/Users/ghtak/Projects/personal/libcodex/tests.qt/2.bmp");
    QImage qimg(img.ptr() , img.width() , img.height() , QImage::Format_Grayscale8 );
    ui->label->setPixmap( QPixmap::fromImage(qimg));
    ui->label->setScaledContents( true );
    ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

void MainWindow::on_pushButton_3_clicked()
{
    codex::vision::image ori = codex::vision::load_from("/Users/ghtak/Projects/personal/libcodex/tests.qt/freedive.bmp");
    this->_image = codex::vision::image(ori.width(),ori.height());
    codex::vision::gray_scale( ori , this->_image );
    QImage qimg(this->_image.ptr() , this->_image.width() , this->_image.height() , QImage::Format_Grayscale8 );
    ui->label->setPixmap( QPixmap::fromImage(qimg));
    ui->label->setScaledContents( true );
    ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

void MainWindow::on_pushButton_4_clicked()
{
    codex::vision::image img(this->_image.width() , this->_image.height());
    codex::vision::histogram_equation(this->_image , img);
    QImage qimg(img.ptr() , img.width() , img.height() , QImage::Format_Grayscale8 );
    ui->label->setPixmap( QPixmap::fromImage(qimg));
    ui->label->setScaledContents( true );
    ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}
