#include "fftdialog.hpp"
#include "ui_fftdialog.h"
#include <codex/vision/image_proc.hpp>
#include "qtconvinience.hpp"

FFTDialog::FFTDialog(QWidget *parent , codex::vision::image& img) :
    QDialog(parent), _orig(img) ,
    ui(new Ui::FFTDialog)
{
    ui->setupUi(this);
    /*
    _fft_re = codex::vision::image_base<double>(
                codex::vision::fft_size( _orig.width() )
                , codex::vision::fft_size( _orig.height())
                , 1 );
    _fft_im = codex::vision::image_base<double>(
                codex::vision::fft_size( _orig.width() )
                , codex::vision::fft_size( _orig.height())
                , 1 );
    _fft_re.reset(0);
    _fft_im.reset(0);
*/
    connect(this
            , SIGNAL(sigShowEvent())
            , this
            , SLOT(slotShowEvent()));
}

FFTDialog::~FFTDialog()
{
    delete ui;
}

void FFTDialog::showEvent(QShowEvent *ev)
{
    QDialog::showEvent(ev);
    emit sigShowEvent();
}

void FFTDialog::slotShowEvent()
{

    QTConvinience::bind(ui->image_label,_orig);
}

void FFTDialog::on_pushButton_clicked()
{
  /*
    _fft_re.reset(0);
    _fft_im.reset(0);
    _fft_re.put_channnel( 0 , _orig , 0 );
    for ( std::size_t r = 0 ; r < _fft_re.height() ; ++r ) {
        codex::vision::fft1d( _fft_re.ptr(r) , _fft_im.ptr(r) , _fft_re.width() , 1 );
    }
    _fft_re = _fft_re.transpose();
    _fft_im = _fft_im.transpose();
    for ( std::size_t r = 0 ; r < _fft_re.height() ; ++r ) {
        codex::vision::fft1d( _fft_re.ptr(r) , _fft_im.ptr(r) , _fft_re.width() , 1 );
    }
    _fft_re = _fft_re.transpose();
    _fft_im = _fft_im.transpose();
*/
    codex::vision::fft( _orig , _fft_re , _fft_im );
    codex::vision::image fft_img( _fft_re.width(), _fft_re.height() );
    codex::vision::image fft_img_phs( _fft_re.width(), _fft_re.height() );

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    codex::vision::fft_mag_image( _fft_re , _fft_im , fft_img);
    codex::vision::fft_phs_image( _fft_re , _fft_im , fft_img_phs);

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    QTConvinience::bind(ui->image_fftre,fft_img);
    QTConvinience::bind(ui->image_fftim,fft_img_phs);
}

void FFTDialog::on_pushButton_2_clicked()
{
    for ( std::size_t r = 0 ; r < _fft_re.height() ; ++r ) {
        codex::vision::fft1d( _fft_re.ptr(r) , _fft_im.ptr(r) , _fft_re.width() , -1 );
    }
    _fft_re = _fft_re.transpose();
    _fft_im = _fft_im.transpose();
    for ( std::size_t r = 0 ; r < _fft_re.height() ; ++r ) {
        codex::vision::fft1d( _fft_re.ptr(r) , _fft_im.ptr(r) , _fft_re.width() , -1 );
    }
    _fft_re = _fft_re.transpose();
    _fft_im = _fft_im.transpose();

    codex::vision::image ifft_img( _fft_re );

    QTConvinience::bind(ui->image_iftt,ifft_img);
}

void FFTDialog::on_pushButton_3_clicked()
{
    double radius = _fft_re.width() * 0.1;
    double radius2 = radius * radius;
    for ( std::size_t r = 0 ; r < radius ; ++r ) {
        for ( std::size_t c = 0 ; c < radius ; ++c ) {
            if ( r*r + c*c < radius2 ) {
                double* lt = _fft_re.ptr(r) + c;
                double* rt = _fft_re.ptr(r) + _fft_re.stride() - c;
                double* lb = _fft_re.ptr( _fft_re.height() - r - 1 ) + c;
                double* rb = _fft_re.ptr( _fft_re.height() - r - 1 ) + _fft_re.stride() - c;
                *lt = *rt = *lb = *rb = 0;

               lt = _fft_im.ptr(r) + c;
               rt = _fft_im.ptr(r) + _fft_im.stride() - c;
               lb = _fft_im.ptr( _fft_im.height() - r - 1 ) + c;
               rb = _fft_im.ptr( _fft_im.height() - r - 1 ) + _fft_im.stride() - c;
                *lt = *rt = *lb = *rb = 0;
            }
        }
    }

    codex::vision::image fft_img( _fft_re.width(), _fft_re.height() );
    codex::vision::image fft_img_phs( _fft_re.width(), _fft_re.height() );

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    codex::vision::fft_mag_image( _fft_re , _fft_im , fft_img);
    codex::vision::fft_phs_image( _fft_re , _fft_im , fft_img_phs);

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    QTConvinience::bind(ui->image_fftre,fft_img);
    QTConvinience::bind(ui->image_fftim,fft_img_phs);
}

void FFTDialog::on_pushButton_4_clicked()
{
    double radius = _fft_re.width() * 0.2;
    double radius2 = radius * radius;
    for ( std::size_t r = 0 ; r < _fft_re.height()  / 2 ; ++r ) {
        for ( std::size_t c = 0 ; c < _fft_re.width()  / 2 ; ++c ) {
            if ( r*r + c*c > radius2 ) {
                double* lt = _fft_re.ptr(r) + c;
                double* rt = _fft_re.ptr(r) + _fft_re.stride() - c;
                double* lb = _fft_re.ptr( _fft_re.height() - r - 1 ) + c;
                double* rb = _fft_re.ptr( _fft_re.height() - r - 1 ) + _fft_re.stride() - c;
                *lt = *rt = *lb = *rb = 0;

               lt = _fft_im.ptr(r) + c;
               rt = _fft_im.ptr(r) + _fft_im.stride() - c;
               lb = _fft_im.ptr( _fft_im.height() - r - 1 ) + c;
               rb = _fft_im.ptr( _fft_im.height() - r - 1 ) + _fft_im.stride() - c;
                *lt = *rt = *lb = *rb = 0;
            }
        }
    }

    codex::vision::image fft_img( _fft_re.width(), _fft_re.height() );
    codex::vision::image fft_img_phs( _fft_re.width(), _fft_re.height() );

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    codex::vision::fft_mag_image( _fft_re , _fft_im , fft_img);
    codex::vision::fft_phs_image( _fft_re , _fft_im , fft_img_phs);

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    QTConvinience::bind(ui->image_fftre,fft_img);
    QTConvinience::bind(ui->image_fftim,fft_img_phs);
}

void FFTDialog::on_pushButton_5_clicked()
{
    double radius = 32;
    double radius2 = radius * radius;
    for ( std::size_t r = 0 ; r < _fft_re.height()  / 2 ; ++r ) {
        for ( std::size_t c = 0 ; c < _fft_re.width()  / 2 ; ++c ) {
            double dist = r*r + c*c;
            double denom = 2 * radius2;
            double w = exp( -dist / denom );
            double* lt = _fft_re.ptr(r) + c;
            double* rt = _fft_re.ptr(r) + _fft_re.stride() - c;
            double* lb = _fft_re.ptr( _fft_re.height() - r - 1 ) + c;
            double* rb = _fft_re.ptr( _fft_re.height() - r - 1 ) + _fft_re.stride() - c;
            double v = *lt; *lt = v * w;
            v = *rt; *rt = v * w;
            v = *lb; *lb = v * w;
            v = *rb; *rb = v * w;


           lt = _fft_im.ptr(r) + c;
           rt = _fft_im.ptr(r) + _fft_im.stride() - c;
           lb = _fft_im.ptr( _fft_im.height() - r - 1 ) + c;
           rb = _fft_im.ptr( _fft_im.height() - r - 1 ) + _fft_im.stride() - c;
           v = *lt; *lt = v * w;
           v = *rt; *rt = v * w;
           v = *lb; *lb = v * w;
           v = *rb; *rb = v * w;
        }
    }

    codex::vision::image fft_img( _fft_re.width(), _fft_re.height() );
    codex::vision::image fft_img_phs( _fft_re.width(), _fft_re.height() );

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    codex::vision::fft_mag_image( _fft_re , _fft_im , fft_img);
    codex::vision::fft_phs_image( _fft_re , _fft_im , fft_img_phs);

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    QTConvinience::bind(ui->image_fftre,fft_img);
    QTConvinience::bind(ui->image_fftim,fft_img_phs);
}

void FFTDialog::on_pushButton_6_clicked()
{
    double radius = 32;
    double radius2 = radius * radius;
    for ( std::size_t r = 0 ; r < _fft_re.height()  / 2 ; ++r ) {
        for ( std::size_t c = 0 ; c < _fft_re.width()  / 2 ; ++c ) {
            double dist = r*r + c*c;
            double denom = 2 * radius2;
            double w = 1.0 - exp( -dist / denom );
            double* lt = _fft_re.ptr(r) + c;
            double* rt = _fft_re.ptr(r) + _fft_re.stride() - c;
            double* lb = _fft_re.ptr( _fft_re.height() - r - 1 ) + c;
            double* rb = _fft_re.ptr( _fft_re.height() - r - 1 ) + _fft_re.stride() - c;
            double v = *lt; *lt = v * w;
            v = *rt; *rt = v * w;
            v = *lb; *lb = v * w;
            v = *rb; *rb = v * w;


           lt = _fft_im.ptr(r) + c;
           rt = _fft_im.ptr(r) + _fft_im.stride() - c;
           lb = _fft_im.ptr( _fft_im.height() - r - 1 ) + c;
           rb = _fft_im.ptr( _fft_im.height() - r - 1 ) + _fft_im.stride() - c;
           v = *lt; *lt = v * w;
           v = *rt; *rt = v * w;
           v = *lb; *lb = v * w;
           v = *rb; *rb = v * w;
        }
    }

    codex::vision::image fft_img( _fft_re.width(), _fft_re.height() );
    codex::vision::image fft_img_phs( _fft_re.width(), _fft_re.height() );

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    codex::vision::fft_mag_image( _fft_re , _fft_im , fft_img);
    codex::vision::fft_phs_image( _fft_re , _fft_im , fft_img_phs);

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    QTConvinience::bind(ui->image_fftre,fft_img);
    QTConvinience::bind(ui->image_fftim,fft_img_phs);
}
