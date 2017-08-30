#ifndef HISTOGRAMDIALOG_HPP
#define HISTOGRAMDIALOG_HPP

#include <QDialog>
#include <codex/vision/image.hpp>

namespace Ui {
class HistogramDialog;
}

class HistogramDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HistogramDialog(QWidget *parent  , const codex::vision::image& img );
    ~HistogramDialog();


protected:
    void showEvent(QShowEvent *ev);

signals:
    void sigShowEvent();

private slots:
    void slotShowEvent();

private:
    Ui::HistogramDialog *ui;
    codex::vision::image _image;
};

#endif // HISTOGRAMDIALOG_HPP
