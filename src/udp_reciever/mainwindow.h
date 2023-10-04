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

  void CreateActions();
  void PreparePlot();
  void PrepareLayout();
  void FillLayout();
  void CreateInfoWidget();
  void CreateSocket();
  void RedrawPlot();
};

#endif // MAINWINDOW_H
