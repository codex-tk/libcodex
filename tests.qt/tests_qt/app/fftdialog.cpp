#include "fftdialog.hpp"
#include "ui_fftdialog.h"
#include <codex/vision/image_proc.hpp>
#include "qtconvinience.hpp"

FFTDialog::FFTDialog(QWidget *parent , codex::vision::image& img) :
    QDialog(parent), _orig(img) ,
    ui(new Ui::FFTDialog)
{
    ui->setupUi(this);
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

    codex::vision::image fft_img( _fft_re.width(), _fft_re.height() );
    codex::vision::image fft_img_phs( _fft_re.width(), _fft_re.height() );

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    codex::vision::fft_mag_image( _fft_re , _fft_im , fft_img);
    codex::vision::fft_phs_image( _fft_re , _fft_im , fft_img_phs);

    codex::vision::fft_shift(_fft_re);
    codex::vision::fft_shift(_fft_im);

    QTConvinience::bind(ui->image_label,_orig);
    QTConvinience::bind(ui->image_fftre,fft_img);
    QTConvinience::bind(ui->image_fftim,fft_img_phs);

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
