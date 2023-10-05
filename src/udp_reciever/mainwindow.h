#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
public slots:
  void ShowPlot();
  void ShowInfo();
private slots:
  void ReadData();
  void StartRecieving();
  void StopRecieving();
  void Refresh();

private:
  Ui::MainWindow *ui;
  QMenu *menu_;
  QMenuBar *menu_bar_;
  QCustomPlot *plot_;
  QGridLayout *layout_;
  QWidget *info_widget_;
  QUdpSocket *socket_;
  QUdpSocket *output_socket_;

  QVector<double> y_vector_;
  QVector<double> x_vector_;
  QVector<double> avg_;
  QVector<double> avg_x_vector_;

  QPushButton *start_;
  QPushButton *stop_;
  QPushButton *refresh_;
  int checker;

  void CreateActions();
  void PreparePlot();
  void PrepareLayout();
  void FillLayout();
  void CreateInfoWidget();
  void CreateSocket();
  void RedrawPlot();
  void ComputeAverage(const QVector<double> &);
};

#endif // MAINWINDOW_H
