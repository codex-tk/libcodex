#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QTime>

#include <codex/codex.hpp>
#include <codex/error_code.hpp>

#include <codex/vision/image_proc.hpp>
#include <codex/vision/image_draw.hpp>
#include "qtconvinience.hpp"

using namespace codex::vision;

#if defined( __codex_win32__ )
std::string path = R"(C:\Users\codex\works\libcodex\tests.qt\)";
#else
std::string path = "/Users/ghtak/Projects/personal/libcodex/tests.qt/";
#endif

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
    QTConvinience::bind(ui->label , img);
}

void MainWindow::on_pushButton_2_clicked()
{
    codex::vision::image img = codex::vision::load_from(path + "2.bmp");
    QTConvinience::bind(ui->label , img);
}

void MainWindow::on_pushButton_3_clicked()
{
    codex::vision::image ori = codex::vision::load_from(path +"freedive.bmp");
    this->_image =  codex::vision::gray_scale( ori);
    QTConvinience::bind(ui->label , this->_image);
}

void MainWindow::on_pushButton_4_clicked()
{
    codex::vision::image img(this->_image.width() , this->_image.height());
    codex::vision::histogram_equation(this->_image,img);
    codex::vision::image orig_hist;
    codex::vision::image euquation_hist;
    codex::vision::histogram_graph(this->_image,orig_hist);
    codex::vision::histogram_graph(img,euquation_hist);

    QTConvinience::bind(ui->label , img);
    QTConvinience::bind(ui->label_2 , orig_hist);
    QTConvinience::bind(ui->label_3 , euquation_hist);
}

void MainWindow::on_pushButton_5_clicked()
{
    codex::vision::image img(320,240);
    codex::vision::line_to(img,codex::vision::point{0,0}
                           , codex::vision::point{160,120}
                           , 255 );
    QTConvinience::bind(ui->label , img);
}

void MainWindow::on_pushButton_6_clicked()
{
    codex::vision::image ori = codex::vision::load_from(path +"freedive.bmp");
    codex::vision::image gray = codex::vision::gray_scale(ori);
    codex::vision::image sample( gray.width() , gray.height());
    //double x = 255.0f / sample.width();
    double y = 255.0f / sample.height();
    for ( std::size_t r = 0 ; r < sample.height() ;++r ){
        for ( std::size_t c = 0 ;c < sample.width() ;++c){
            sample.at(c,r) = 256 - ( r * y );
            //sample.at(c,r) = (c * x + r * y) / 2;
        }
    }
    codex::vision::image::value_type val = 100;
    codex::vision::image sum = gray + val;
    QTConvinience::bind(ui->label , sum);
}

void MainWindow::on_pushButton_7_clicked()
{
    QTime startTime = QTime::currentTime();
    codex::vision::image ori = codex::vision::load_from(path +"freedive.bmp");
    qDebug()<< "Load " << startTime.elapsed(); startTime = QTime::currentTime();
    codex::vision::image gray = codex::vision::gray_scale(ori);
    qDebug()<< "Gray " << startTime.elapsed(); startTime = QTime::currentTime();
    codex::vision::image sample( gray.width() , gray.height());
    qDebug()<< "newImg " << startTime.elapsed(); startTime = QTime::currentTime();
    codex::vision::convolution(gray,codex::vision::laplacian_mask,sample);
    qDebug()<< "conv " << startTime.elapsed(); startTime = QTime::currentTime();
    codex::vision::image hist( sample.width(),sample.height());
    codex::vision::histogram_equation( sample , hist);
    qDebug()<< "hist " << startTime.elapsed(); startTime = QTime::currentTime();
    QTConvinience::bind(ui->label , hist);
    qDebug()<< "bind " << startTime.elapsed(); startTime = QTime::currentTime();
}

void MainWindow::on_pushButton_8_clicked()
{
    codex::vision::image ori = codex::vision::load_from(path +"freedive.bmp");
    codex::vision::image gray = codex::vision::gray_scale(ori);
    codex::vision::image sample( gray.width() , gray.height());
    codex::vision::sobel(gray,sample);
    codex::vision::image hist( sample.width(),sample.height());
    codex::vision::histogram_equation( sample , hist);
    QTConvinience::bind(ui->label , hist);
}
