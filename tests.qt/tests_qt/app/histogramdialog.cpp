#include "histogramdialog.hpp"
#include "ui_histogramdialog.h"
#include "qtconvinience.hpp"
#include <codex/vision/image_proc.hpp>
#include <codex/vision/image_draw.hpp>

#include <QDebug>

HistogramDialog::HistogramDialog(QWidget *parent , const codex::vision::image& img ) :
    QDialog(parent),
    ui(new Ui::HistogramDialog),
    _image(img)
{
    ui->setupUi(this);

    connect(this
            , SIGNAL(sigShowEvent())
            , this
            , SLOT(slotShowEvent()));
}

HistogramDialog::~HistogramDialog()
{
    delete ui;
}


void HistogramDialog::showEvent(QShowEvent *ev)
{
    QDialog::showEvent(ev);
    emit sigShowEvent();
}

void HistogramDialog::slotShowEvent()
{
    codex::vision::image hist( _image.width() , _image.height() , _image.channel());
    codex::vision::equalize_hist( _image , hist );
    QTConvinience::bind(ui->origin , _image);
    QTConvinience::bind(ui->hist , hist );

    struct hist_graph {
        static codex::vision::image proc( const codex::vision::image& src ) {
            int hist[256] = {0};
            double max_hist = 0;
            src.for_each( [&]( std::size_t  , std::size_t  , std::size_t l , uint8_t val ){
                if ( l == 0 ) {
                    hist[val] += 1;
                    max_hist = std::max( max_hist , static_cast<double>(hist[val]));
                }
            });

            codex::vision::image graph( 256 , 300 );
            if ( max_hist == 0 ) {
                return graph;
            }
            for ( int i = 0 ; i < 256 ; ++i ) {
                codex::vision::line_to( graph
                                        , codex::vision::point{  i , 200 }
                                        , codex::vision::point{  i , 200 - static_cast<int>( (hist[i] / max_hist) * 200 )}
                                        , 255 );
            }

            for ( int i = 0 ; i < 256 ; ++i ){
                for ( int j = 0 ; j < 50 ; ++j ) {
                    graph.at( i , j + 220 ) = i;
                }
            }
            return graph;
        }
    };
    auto oh = hist_graph::proc(_image);
    QTConvinience::bind(ui->origin_graph ,oh );
    auto hh = hist_graph::proc(hist);
    QTConvinience::bind(ui->hist_graph , hh );
}
