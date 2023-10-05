#include <QApplication>
#include <QByteArray>
#include <QNetworkDatagram>
#include <QUdpSocket>
#include <functional>
#include <iostream>
#include <random>

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QUdpSocket *output_socket_ = new QUdpSocket();
  output_socket_->bind(QHostAddress::LocalHost, 10003);
  std::random_device rd;
  std::default_random_engine engine(rd());
  auto gen = std::bind(std::uniform_int_distribution<>(0, 1000), engine);
  while (true) {
    QByteArray data;
    for (int i = 0; i < 4096; ++i) {
      short num = static_cast<short>(gen());
      data.push_back(num);
    }
    QNetworkDatagram out_datagram;
    out_datagram.setDestination(QHostAddress::LocalHost, 10002);
    out_datagram.setData(data);
    auto count = output_socket_->writeDatagram(out_datagram);
    std::cout << count << std::endl;
  }
  return a.exec();
}
