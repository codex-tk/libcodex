#ifndef KMEANDIALOG_H
#define KMEANDIALOG_H

#include <QDialog>
#include "qtconvinience.hpp"
#include "mouseeventsignaler.h"

namespace Ui {
class KMeanDialog;
}

class KMeanDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KMeanDialog( QWidget *parent ,
                          const codex::vision::image& img );
    ~KMeanDialog();
protected:
    void showEvent(QShowEvent *ev);

signals:
    void sigShowEvent();

private slots:
    void slotShowEvent();
    void on_pushButton_clicked();

private:
    Ui::KMeanDialog *ui;
    MouseEventSignaler _signaler;
    codex::vision::image _image;
    int index;
    std::vector< std::vector<uint8_t> > means;
    std::vector< QLabel* > labels;
};

#endif // KMEANDIALOG_H
