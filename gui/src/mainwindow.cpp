#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>

#include <QImage>
#include <QMessageBox>
#include <QPixmap>
#include <QScreen>
#include <QTextStream>


#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), timer(new QTimer(this))
{
    load_ctable();
    ui->setupUi(this);
    setup_image();
    receiver.set_zmq_hwm(1);
    receiver.set_timeout(10);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::update_image));

}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::load_ctable() {
    QFile file(":/cmap/black-body-table-byte-0256.csv");
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::information(0, "error", file.errorString());
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList values = line.split(",");
        ctable.append(
            qRgb(values[1].toInt(), values[2].toInt(), values[3].toInt()));
    }
    file.close();
}

void MainWindow::setup_image() {
    int width = 1024;
    int height = 512;
    image = new QImage(width, height, QImage::Format_Indexed8);
    image->setColorTable(ctable);
    item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
    QGraphicsScene *scene = new QGraphicsScene;
    ui->view->setScene(scene);
    
    scene->addItem(item);
    item->setPos(0, 0);
    ui->view->show();
    ui->view->fitInView(item, Qt::KeepAspectRatio);
    buffer.resize(512*1024);
    
}

void MainWindow::update_image(){

    //read the last image from zmq
    int64_t frame_number;
    receiver.receive_into(1, &frame_number, reinterpret_cast<std::byte*>(buffer.data()));

    //convert and copy
    uint8_t* data = image->bits();
    for (int i = 0; i!=buffer.size(); ++i)
        *data++ = buffer[i];

    // memcpy(image->bits(), raw_data.data(), image->sizeInBytes());


    item->setPixmap(QPixmap::fromImage(*image));
}

void MainWindow::on_startButton_clicked() {

    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    receiver.connect();
    timer->start(100); //1000 -> 1s

}

void MainWindow::on_stopButton_clicked() {
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    timer->stop();
    receiver.disconnect();
}

void MainWindow::on_actionButton_clicked() {
    // for (int i=0; i<35; ++i){
    //     update_data();
    // }
        
}


void MainWindow::resizeEvent(QResizeEvent *event) {
    ui->view->fitInView(item, Qt::KeepAspectRatio);
    std::cout << "resize\n";
}
