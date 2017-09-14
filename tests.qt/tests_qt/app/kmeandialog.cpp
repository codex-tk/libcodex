#include "kmeandialog.h"
#include "ui_kmeandialog.h"
#include <codex/vision/image_proc.hpp>

KMeanDialog::KMeanDialog(QWidget *parent ,
                         const codex::vision::image& img )
    : QDialog(parent),
    ui(new Ui::KMeanDialog),
    _image(img)
{
    ui->setupUi(this);

    index = 0;

    _signaler.installOn(ui->label);
    QObject::connect(&_signaler, &MouseEventSignaler::mouseButtonEvent,
      [this](QWidget*, QMouseEvent * event) {
        if (event->type() == QEvent::MouseButtonPress)
        {
            if ( index >= 4 )
                return;
            codex::vision::image sample( 5,5,_image.channel());
            double xr =  event->pos().x() / (double)(ui->label->width());
            double yr =  event->pos().y() / (double)(ui->label->height());
            int x =  static_cast<int>( _image.width() * xr );
            int y =  static_cast<int>( _image.height() * yr );
            std::vector<uint8_t> values;
            for ( std::size_t l = 0 ; l < _image.channel() ; ++l ) {
                values.push_back( _image.at(x,y,l));
            }
            sample.reset_channels(values);
            means.push_back(values);
            QTConvinience::bind(labels[index++],sample);
            //ui->label->setText(posText);
        }
    });

    connect(this
            , SIGNAL(sigShowEvent())
            , this
            , SLOT(slotShowEvent()));
}

KMeanDialog::~KMeanDialog()
{
    delete ui;
}

void KMeanDialog::showEvent(QShowEvent *ev)
{
    QDialog::showEvent(ev);
    emit sigShowEvent();
}

void KMeanDialog::slotShowEvent()
{
    QTConvinience::bind(ui->label,_image);

    labels.push_back(ui->label_2);
    labels.push_back(ui->label_3);
    labels.push_back(ui->label_4);
    labels.push_back(ui->label_5);

}

void KMeanDialog::on_pushButton_clicked()
{
    codex::vision::image new_img( _image.width() ,_image.height() , _image.channel());

    codex::vision::kmeans(
                _image , new_img , means
                );

    QTConvinience::bind(ui->label,new_img);
}
