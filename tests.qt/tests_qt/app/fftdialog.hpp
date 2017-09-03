#ifndef FFTDIALOG_HPP
#define FFTDIALOG_HPP

#include <QDialog>
#include <codex/vision/image.hpp>

namespace Ui {
class FFTDialog;
}

class FFTDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FFTDialog(QWidget *parent , codex::vision::image& orig);
    ~FFTDialog();


protected:
    void showEvent(QShowEvent *ev);

signals:
    void sigShowEvent();

private slots:
    void slotShowEvent();

private:
    codex::vision::image _orig;
    Ui::FFTDialog *ui;
    codex::vision::image_base<double> _fft_re;
    codex::vision::image_base<double> _fft_im;
};

#endif // FFTDIALOG_HPP
