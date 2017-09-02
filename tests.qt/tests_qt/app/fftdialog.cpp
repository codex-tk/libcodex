#include "fftdialog.hpp"
#include "ui_fftdialog.h"
#include <codex/vision/image_proc.hpp>

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

}
