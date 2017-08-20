#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include <codex/codex.hpp>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  codex::universe();
}

MainWindow::~MainWindow()
{
  delete ui;
}
