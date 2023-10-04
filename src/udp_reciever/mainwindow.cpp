#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QByteArray>
#include <QNetworkDatagram>
#include <QUdpSocket>
#include <thread>

#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  CreateActions();
  PreparePlot();
  PrepareLayout();
  CreateInfoWidget();
  FillLayout();
}

MainWindow::~MainWindow() {
  delete ui;
  delete info_widget_;
}

void MainWindow::ShowPlot() {
  plot_->show();
  CreateSocket();
}

void MainWindow::ShowInfo() { info_widget_->show(); }

void MainWindow::ReadData() {
  QByteArray datagram;
  while (socket_->hasPendingDatagrams()) {
    datagram.resize(static_cast<int>(socket_->pendingDatagramSize()));
    socket_->readDatagram(datagram.data(), datagram.size());

    QDataStream in_stream(datagram);
    short number = 0;
    double legend = (x_vector_.isEmpty()) ? 1 : x_vector_.last();

    while (!in_stream.atEnd()) {
      in_stream >> number;
      y_vector_.push_back(static_cast<double>(number));
      x_vector_.push_back(++legend);
    }
    plot_->graph(0)->data()->clear();
    plot_->graph(0)->setData(x_vector_, y_vector_);
    plot_->replot();
  }
}

void MainWindow::CreateActions() {

  menu_ = new QMenu(tr("Tools"), this);
  QAction *plot = new QAction(tr("Wave Viewer"), this);
  QAction *about = new QAction(tr("About"), this);
  menu_->addAction(plot);
  menu_->addAction(about);
  QMenuBar *menu_bar = menuBar();
  menu_bar->addMenu(menu_);
  connect(plot, &QAction::triggered, this, &MainWindow::ShowPlot);
  connect(about, &QAction::triggered, this, &MainWindow::ShowInfo);
}

void MainWindow::PreparePlot() {
  plot_ = new QCustomPlot(this);
  plot_->addGraph();
  plot_->graph(0)->setLineStyle(QCPGraph::lsLine);
  plot_->graph(0)->setScatterStyle(
      QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
  plot_->setInteraction(QCP::iRangeDrag, true);
  plot_->setInteraction(QCP::iRangeZoom, true);
  plot_->xAxis->setLabel("x");
  plot_->yAxis->setLabel("y");
  plot_->hide();
}

void MainWindow::PrepareLayout() {
  setCentralWidget(new QWidget(this));
  setWindowTitle(tr("UDP reciever"));
  layout_ = new QGridLayout(centralWidget());
  layout_->setRowStretch(0, 2);
  layout_->setColumnStretch(0, 2);
}

void MainWindow::FillLayout() {
  if (layout_ == nullptr || plot_ == nullptr) {
    throw "";
  }
  layout_->addWidget(plot_, 0, 0, 1, 1);
}

void MainWindow::CreateInfoWidget() {
  info_widget_ = new QWidget();
  info_widget_->setWindowTitle(tr("info"));
  info_widget_->setWindowModality(Qt::NonModal);
  QGridLayout layout(info_widget_);
  QLabel info_lbl(info_widget_);
  info_lbl.setText("abc");
  layout.addWidget(&info_lbl, 0, 0, 1, 1, Qt::AlignCenter);
}

void MainWindow::CreateSocket() {
  socket_ = new QUdpSocket(this);
  socket_->bind(QHostAddress::LocalHost, 10002);
  connect(socket_, &QUdpSocket::readyRead, this, &MainWindow::ReadData);
}
