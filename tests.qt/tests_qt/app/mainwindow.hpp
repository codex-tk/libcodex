#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStringListModel>

#include "app.hpp"

#include <codex/vision/image.hpp>
#include <codex/vision/bitmap.hpp>
#include "binarysampledialog.h"

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

protected:
    void showEvent(QShowEvent *ev);

signals:
    void sigShowEvent();

private slots:
    void slotShowEvent();
    void on_image_file_list_clicked(const QModelIndex &index);

    void on_to_gray_button_clicked();

    void on_hist_button_clicked();

    void on_Gaussian_clicked();

    void on_mean_button_clicked();

    void on_median_button_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_fft_button_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

private:
  Ui::MainWindow *ui;
  std::shared_ptr< QImage > _base_image;
  codex::vision::image _image;
};

#endif // MAINWINDOW_HPP
