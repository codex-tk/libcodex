#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <codex/codex.hpp>
#include <codex/error_code.hpp>

#include <codex/vision/image_proc.hpp>
#include <codex/vision/image_draw.hpp>

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
    codex::vision::histogram_equation(this->_image,img);
    codex::vision::image orig_hist;
    codex::vision::image euquation_hist;
    codex::vision::histogram_graph(this->_image,orig_hist);
    codex::vision::histogram_graph(img,euquation_hist);

    QImage qimg(img.ptr() , img.width() , img.height()
                , QImage::Format_Grayscale8 );
    ui->label->setPixmap( QPixmap::fromImage(qimg));
    ui->label->setScaledContents( true );
    ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    QImage qimg2(orig_hist.ptr() , orig_hist.width() , orig_hist.height()
                 , QImage::Format_Grayscale8 );
    ui->label_2->setPixmap( QPixmap::fromImage(qimg2));
    ui->label_2->setScaledContents( true );
    ui->label_2->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );


    QImage qimg3(euquation_hist.ptr() , euquation_hist.width() , euquation_hist.height()
                 , QImage::Format_Grayscale8 );
    ui->label_3->setPixmap( QPixmap::fromImage(qimg3));
    ui->label_3->setScaledContents( true );
    ui->label_3->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    /*
    codex::vision::image oh;
    codex::vision::image he;
    codex::vision::histogram_equation_debug(this->_image , img , oh , he );
    //this->_image = img;
    QImage qimg(img.ptr() , img.width() , img.height() , QImage::Format_Grayscale8 );
    ui->label->setPixmap( QPixmap::fromImage(qimg));
    ui->label->setScaledContents( true );
    ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    QImage qimg2(oh.ptr() , oh.width() , oh.height() , QImage::Format_Grayscale8 );
    ui->label_2->setPixmap( QPixmap::fromImage(qimg2));
    ui->label_2->setScaledContents( true );
    ui->label_2->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );


    QImage qimg3(he.ptr() , he.width() , he.height() , QImage::Format_Grayscale8 );
    ui->label_3->setPixmap( QPixmap::fromImage(qimg3));
    ui->label_3->setScaledContents( true );
    ui->label_3->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    */
}

void MainWindow::on_pushButton_5_clicked()
{
    codex::vision::image img(320,240);
    codex::vision::line_to(img,codex::vision::point{0,0}
                           , codex::vision::point{160,120}
                           , 255 );
    QImage qimg(img.ptr(),img.width(),img.height(),QImage::Format_Grayscale8 );
    ui->label->setPixmap( QPixmap::fromImage(qimg));
    ui->label->setScaledContents( true );
    ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}
