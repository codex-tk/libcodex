#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <QtDebug>
#include <QTime>
#include <QDir>

#include <codex/codex.hpp>
#include <codex/error_code.hpp>

#include <codex/vision/image_proc.hpp>
#include <codex/vision/image_draw.hpp>
#include "qtconvinience.hpp"
#include <cmath>

#include "histogramdialog.hpp"

std::string path() {
#if defined( __codex_win32__ )
    std::string path = R"(C:\Users\codex\works\libcodex\tests.qt\)";
#else
    std::string path = "/Users/ghtak/Projects/personal/libcodex/tests.qt/";
#endif
    return path;
}
using namespace codex::vision;

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringListModel* model = new QStringListModel(this);

    QDir res_dir = QDir(":/res/images/");
    res_dir.setNameFilters(QStringList() << "*.jpg" << "*.png" << "*.bmp");

    model->setStringList(res_dir.entryList());

    ui->image_file_list->setModel(model );


    model = new QStringListModel(this);
    QStringList lists;
    lists.append( "prewittX" );
    lists.append( "prewittY" );
    lists.append( "sobleX" );
    lists.append( "sobleY" );
    lists.append( "emboss1" );
    lists.append( "emboss2" );
    lists.append( "laplacian4" );
    lists.append( "laplacian8" );
    lists.append( "unsharp4" );
    lists.append( "unsharp8" );
    model->setStringList(lists);
    ui->filter_combobox->setModel(model);

    connect(this
            , SIGNAL(sigShowEvent())
            , this
            , SLOT(slotShowEvent()));
}

MainWindow::~MainWindow()
{
  delete ui;
}
/*
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
    //QTConvinience::bind(ui->label , img);
}

void MainWindow::on_pushButton_2_clicked()
{
    codex::vision::image img = codex::vision::load_from(path() + "2.bmp");
    //QTConvinience::bind(ui->label , img);
}

void MainWindow::on_pushButton_3_clicked()
{
    codex::vision::image ori = codex::vision::load_from(path() +"freedive.bmp");
    this->_image =  codex::vision::gray_scale( ori);
    //QTConvinience::bind(ui->label , this->_image);
}

void MainWindow::on_pushButton_4_clicked()
{
    codex::vision::image img(this->_image.width() , this->_image.height());
    codex::vision::histogram_equation(this->_image,img);
    codex::vision::image orig_hist;
    codex::vision::image euquation_hist;
    codex::vision::histogram_graph(this->_image,orig_hist);
    codex::vision::histogram_graph(img,euquation_hist);

    //QTConvinience::bind(ui->label , img);
    //QTConvinience::bind(ui->label_2 , orig_hist);
    //QTConvinience::bind(ui->label_3 , euquation_hist);
}

void MainWindow::on_pushButton_5_clicked()
{
    codex::vision::image img(320,240);
    codex::vision::line_to(img,codex::vision::point{0,0}
                           , codex::vision::point{160,120}
                           , 255 );
   // QTConvinience::bind(ui->label , img);
}

void MainWindow::on_pushButton_6_clicked()
{
    codex::vision::image ori = codex::vision::load_from(path() +"freedive.bmp");
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
    gray += 100;
   // QTConvinience::bind(ui->label , gray);
}

void MainWindow::on_pushButton_7_clicked()
{
    QTime startTime = QTime::currentTime();
    codex::vision::image ori = codex::vision::load_from(path() +"freedive.bmp");
    qDebug()<< "Load " << startTime.elapsed(); startTime = QTime::currentTime();
    codex::vision::image gray = codex::vision::gray_scale(ori);
    qDebug()<< "Gray " << startTime.elapsed(); startTime = QTime::currentTime();
    codex::vision::image sample( gray.width() , gray.height());
    qDebug()<< "newImg " << startTime.elapsed(); startTime = QTime::currentTime();

    codex::vision::image_base<double> middle( sample.width() ,sample.height());

    codex::vision::detail::filter(gray,sample , codex::vision::laplacian ,[]( double val ) -> uint8_t {
        //if ( val < 0 )
            return codex::vision::operation< uint8_t , double >::clip( val + 128 );
        //return codex::vision::operation< uint8_t , double >::clip( val );

    });
    codex::vision::image hist( sample.width(),sample.height());
    codex::vision::histogram_equation( sample , hist);
   // QTConvinience::bind(ui->label ,  hist);

    codex::vision::detail::filter(gray, middle , codex::vision::laplacian ,[]( double val ) -> double {
        return val;
    });

    qDebug()<< "conv " << startTime.elapsed(); startTime = QTime::currentTime();

    codex::vision::detail::normalize( middle , sample );
    codex::vision::image hist( sample.width(),sample.height());
    codex::vision::histogram_equation( sample , hist);
    QTConvinience::bind(ui->label ,  hist);

    codex::vision::image hist( sample.width(),sample.height());
    codex::vision::histogram_equation( sample , hist);
    qDebug()<< "hist " << startTime.elapsed(); startTime = QTime::currentTime();
    auto result = gray - hist;
    gray -= hist;
    QTConvinience::bind(ui->label ,  result);
    qDebug()<< "bind " << startTime.elapsed(); startTime = QTime::currentTime();
    */
//}

/*
void MainWindow::on_pushButton_8_clicked()
{
    qDebug() << codex::vision::sqrt(13);
    qDebug() << std::sqrt(13);
    codex::vision::image ori = codex::vision::load_from(path() +"freedive.bmp");
    codex::vision::image gray = codex::vision::gray_scale(ori);
    codex::vision::image sample( gray.width() , gray.height());
    codex::vision::sobel(gray,sample);
    codex::vision::image hist( sample.width(),sample.height());
    codex::vision::histogram_equation( sample , hist);
    QTConvinience::bind(ui->label , hist);
}

void MainWindow::on_pushButton_9_clicked()
{
    this->_bsdiag->show();
}
*/
void MainWindow::showEvent(QShowEvent *ev)
{
    QMainWindow::showEvent(ev);
    emit sigShowEvent();
}

void MainWindow::slotShowEvent()
{

    //ui->label->setPixmap( QPixmap::fromImage( *_base_images[0] ));
   // ui->label->setScaledContents( true );
    //ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

void MainWindow::on_image_file_list_clicked(const QModelIndex &index)
{
    index.data().toString();
    _base_image = std::make_shared<QImage>(
                ":/res/images/" + index.data().toString()
                );
    ui->image_label->setPixmap(QPixmap::fromImage( *_base_image ));
    ui->image_label->setScaledContents(true);
    ui->image_label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

void MainWindow::on_to_gray_button_clicked()
{
    int channel = -1;
    switch ( _base_image->format() ){
    case QImage::Format_RGB32:
    case QImage::Format_ARGB32:
        channel = 4; break;
    case QImage::Format_Mono:
    case QImage::Format_Grayscale8:
        channel = 1; break;
    default:
        qDebug() << "Format:" << _base_image->format();
        break;
    }

    if ( channel == -1 )
        return;


    _image = codex::vision::image( _base_image->width() , _base_image->height() , channel );
    for ( int r = 0 ; r < _base_image->height() ; ++r ) {
        memcpy( _image.ptr(r) , _base_image->scanLine(r) , _image.stride() );
    }
    if ( _image.channel() != 1 ) {
        _image =  codex::vision::gray_scale( _image );
    }
    QTConvinience::bind( ui->image_label , _image );
}

void MainWindow::on_hist_button_clicked()
{
    HistogramDialog* dlg = new HistogramDialog(this , _image );
    dlg->show();
}

void MainWindow::on_Gaussian_clicked()
{
    codex::vision::image gauss( _image.width() , _image.height() , _image.channel() );
    codex::vision::gaussian( _image , gauss , 5 , 1 );
    QTConvinience::bind( ui->image_label , gauss );
}

void MainWindow::on_mean_button_clicked()
{
    codex::vision::image mean( _image.width() , _image.height() , _image.channel() );
    codex::vision::mean( _image , mean , 3 );
    QTConvinience::bind( ui->image_label , mean );
}

void MainWindow::on_median_button_clicked()
{
    codex::vision::image median( _image.width() , _image.height() , _image.channel() );
    codex::vision::median( _image , median , 3 );
    QTConvinience::bind( ui->image_label , median );
}

void MainWindow::on_pushButton_clicked()
{
     codex::vision::image filtered( _image.width() , _image.height() , _image.channel() );
    if ( ui->filter_combobox->currentText() == "prewittX" ) {
        codex::vision::detail::filter( _image , filtered , codex::vision::prewitt_x ,[]( double val ) -> uint8_t {
            return codex::vision::operation< uint8_t , double >::clip( val + 128 );
        });
    }
    if ( ui->filter_combobox->currentText() == "prewittY" ) {
        codex::vision::detail::filter( _image , filtered , codex::vision::prewitt_y ,[]( double val ) -> uint8_t {
            return codex::vision::operation< uint8_t , double >::clip( val + 128 );
        });
    }
    if ( ui->filter_combobox->currentText() ==  "sobleX" ) {
        codex::vision::detail::filter( _image , filtered , codex::vision::sobel_x ,[]( double val ) -> uint8_t {
            return codex::vision::operation< uint8_t , double >::clip( val + 128 );
        });
    }
    if ( ui->filter_combobox->currentText() == "sobleY" ) {
        codex::vision::detail::filter( _image , filtered , codex::vision::sobel_y ,[]( double val ) -> uint8_t {
            return codex::vision::operation< uint8_t , double >::clip( val + 128 );
        });
    }
    if ( ui->filter_combobox->currentText() ==  "emboss1" ) {
        codex::vision::detail::filter( _image , filtered , codex::vision::emboss1 ,[]( double val ) -> uint8_t {
            return codex::vision::operation< uint8_t , double >::clip( val + 128 );
        });
    }
    if ( ui->filter_combobox->currentText() ==   "emboss2" ) {
        codex::vision::detail::filter( _image , filtered , codex::vision::emboss2 ,[]( double val ) -> uint8_t {
            return codex::vision::operation< uint8_t , double >::clip( val + 128 );
        });
    }
    if ( ui->filter_combobox->currentText() ==   "laplacian4" ) {
        codex::vision::detail::filter( _image , filtered , codex::vision::laplacian ,[]( double val ) -> uint8_t {
            return codex::vision::operation< uint8_t , double >::clip( val + 128 );
        });
    }
    if ( ui->filter_combobox->currentText() ==   "laplacian8" ) {
        codex::vision::detail::filter( _image , filtered , codex::vision::laplacian8 ,[]( double val ) -> uint8_t {
            return codex::vision::operation< uint8_t , double >::clip( val + 128 );
        });
    }
    if ( ui->filter_combobox->currentText() ==   "unsharp4" ) {
        codex::vision::detail::filter( _image , filtered , codex::vision::unsharp ,[]( double val ) -> uint8_t {
            return codex::vision::operation< uint8_t , double >::clip( val );
        });
    }
    if ( ui->filter_combobox->currentText() ==  "unsharp8" ) {
        codex::vision::detail::filter( _image , filtered , codex::vision::unsharp8 ,[]( double val ) -> uint8_t {
            return codex::vision::operation< uint8_t , double >::clip( val );
        });
    }
    QTConvinience::bind( ui->image_label , filtered );
}
