#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QByteArray>
#include <QNetworkDatagram>
#include <QUdpSocket>
#include <algorithm>
#include <numeric>

#include "qcustomplot.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), checker(0) {
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
  start_->show();
  stop_->show();
  refresh_->show();
  CreateSocket();
}

void MainWindow::ShowInfo() { info_widget_->show(); }

void MainWindow::ReadData() {
  QByteArray datagram;
  if (checker && socket_->hasPendingDatagrams()) {
    datagram.resize(static_cast<int>(socket_->pendingDatagramSize()));
    socket_->readDatagram(datagram.data(), datagram.size());
    QDataStream in_stream(datagram);
    short number = 0;
    double legend = (x_vector_.isEmpty()) ? 1 : x_vector_.last();
    QVector<double> buff_y_vector;
    while (!in_stream.atEnd()) {
      in_stream >> number;
      buff_y_vector.push_back(static_cast<double>(number));
      x_vector_.push_back(++legend);
    }

    ComputeAverage(buff_y_vector);
    y_vector_.append(buff_y_vector);
    avg_x_vector_[1] = x_vector_.back();
    RedrawPlot();
  }
}

void MainWindow::StartRecieving() {
  checker = 1;
  ReadData();
}

void MainWindow::StopRecieving() { checker = 0; }

void MainWindow::Refresh() {
  y_vector_.clear();
  x_vector_.clear();
  avg_[0] = 0;
  avg_[1] = 0;
  plot_->graph(0)->data()->clear();
  plot_->graph(1)->data()->clear();
  plot_->replot();
}

void MainWindow::CreateActions() {
  // actions
  menu_ = new QMenu(tr("Tools"), this);
  QAction *plot = new QAction(tr("Wave Viewer"), this);
  QAction *about = new QAction(tr("About"), this);
  menu_->addAction(plot);
  menu_->addAction(about);
  QMenuBar *menu_bar = menuBar();
  menu_bar->addMenu(menu_);
  connect(plot, &QAction::triggered, this, &MainWindow::ShowPlot);
  connect(about, &QAction::triggered, this, &MainWindow::ShowInfo);

  // buttons
  start_ = new QPushButton(tr("start"), this);
  stop_ = new QPushButton(tr("stop"), this);
  refresh_ = new QPushButton(tr("refresh"), this);
  start_->hide();
  stop_->hide();
  refresh_->hide();
  connect(start_, &QPushButton::clicked, this, &MainWindow::StartRecieving);
  connect(stop_, &QPushButton::clicked, this, &MainWindow::StopRecieving);
  connect(refresh_, &QPushButton::clicked, this, &MainWindow::Refresh);
}

void MainWindow::PreparePlot() {
  plot_ = new QCustomPlot(this);
  plot_->addGraph();
  plot_->graph(0)->setLineStyle(QCPGraph::lsLine);
  plot_->graph(0)->setScatterStyle(
      QCPScatterStyle(QCPScatterStyle::ssCircle, 1));
  plot_->graph(0)->setName("data");

  plot_->addGraph();
  plot_->graph(1)->setPen(QPen(Qt::red));
  plot_->graph(1)->setName("average");

  plot_->setInteraction(QCP::iRangeDrag, true);
  plot_->setInteraction(QCP::iRangeZoom, true);
  plot_->xAxis->setLabel("x");
  plot_->yAxis->setLabel("y");
  plot_->hide();

  avg_.resize(2);
  avg_x_vector_.resize(2);
  avg_x_vector_[0] = 0;
}

void MainWindow::PrepareLayout() {
  setCentralWidget(new QWidget(this));
  setWindowTitle(tr("UDP reciever"));
  layout_ = new QGridLayout(centralWidget());
  layout_->setRowStretch(0, 2);
}

void MainWindow::FillLayout() {
  if (layout_ == nullptr || plot_ == nullptr) {
    throw "";
  }
  layout_->addWidget(plot_, 0, 0, 1, 3);
  layout_->addWidget(start_, 1, 0, 1, 1, Qt::AlignCenter);
  layout_->addWidget(stop_, 1, 1, 1, 1, Qt::AlignCenter);
  layout_->addWidget(refresh_, 1, 2, 1, 1, Qt::AlignCenter);
}

void MainWindow::CreateInfoWidget() {
  info_widget_ = new QWidget();
  info_widget_->setWindowTitle(tr("info"));
  info_widget_->setWindowModality(Qt::NonModal);
  QGridLayout layout(info_widget_);
  QLabel *info_lbl = new QLabel(tr("There could be your text"), info_widget_);
  layout.addWidget(info_lbl, 0, 0, 1, 1, Qt::AlignCenter);
}

void MainWindow::CreateSocket() {
  socket_ = new QUdpSocket(this);
  socket_->bind(QHostAddress::LocalHost, 10002);
  connect(socket_, &QUdpSocket::readyRead, this, &MainWindow::ReadData);
}

void MainWindow::RedrawPlot() {
  plot_->xAxis->setRange(
      *(std::min_element(x_vector_.begin(), x_vector_.end())),
      *(std::max_element(x_vector_.begin(), x_vector_.end())));
  plot_->yAxis->setRange(
      3 * *(std::min_element(y_vector_.begin(), y_vector_.end())),
      3 * *(std::max_element(y_vector_.begin(), y_vector_.end())));
  plot_->graph(0)->data()->clear();
  plot_->graph(0)->setData(x_vector_, y_vector_);
  plot_->graph(1)->data()->clear();
  plot_->graph(1)->setData(avg_x_vector_, avg_);

  plot_->replot();
}

void MainWindow::ComputeAverage(const QVector<double> &buff_vector) {
  double temp_avg =
      std::reduce(buff_vector.begin(), buff_vector.end()) / buff_vector.size();
  avg_[0] = (avg_.at(1) + temp_avg) / 2;
  avg_[1] = avg_.at(0);
}
