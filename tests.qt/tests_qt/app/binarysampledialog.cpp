#include "binarysampledialog.h"
#include "ui_binarysampledialog.h"
#include "qtconvinience.hpp"
#include <codex/vision/bitmap.hpp>


BinarySampleDialog::BinarySampleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BinarySampleDialog)
{
    ui->setupUi(this);
    codex::vision::image ori = codex::vision::load_from(path() +"freedive.bmp");
    this->_image =  codex::vision::gray_scale( ori);

    codex::vision::image gray = codex::vision::gray_scale(ori);
    codex::vision::image sample( gray.width() , gray.height());
    codex::vision::detail::filter(gray,sample ,codex::vision::laplacian , []( double val ) -> uint8_t {
        return static_cast< uint8_t>(val + 128);
    });
    /*
    this->_image = codex::vision::image( sample.width(),sample.height());
    codex::vision::histogram_equation( sample , this->_image );

    this->_binary = codex::vision::image( this->_image.width()
                                    , this->_image.height()
                                    , this->_image.channel());
                                    */
    this->_image = sample;
    this->_binary = codex::vision::image( this->_image.width()
                                    , this->_image.height()
                                    , this->_image.channel());

}

BinarySampleDialog::~BinarySampleDialog()
{
    delete ui;
}

void BinarySampleDialog::on_horizontalSlider_valueChanged(int value)
{
    for ( std::size_t r = 0 ; r < _image.height() ; ++r  ) {
        uint8_t* src_ptr = _image.ptr(r);
        uint8_t* dst_ptr = _binary.ptr(r);
        for ( std::size_t c = 0 ; c < _image.width() ;  ++c ) {
            if ( src_ptr[c] > value ) {
                dst_ptr[c] = 0xff;
            }else{
                dst_ptr[c] = 0;
            }
        }
    }
    QTConvinience::bind(ui->label , this->_binary);
}

void BinarySampleDialog::on_BinarySampleDialog_accepted()
{

}
