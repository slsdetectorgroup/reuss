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
    : QMainWindow(parent), ui(new Ui::MainWindow), imageLabel(new QLabel),
      scrollArea(new QScrollArea) {
    load_ctable();

    // imageLabel->setBackgroundRole(QPalette::Base);
    // imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    // imageLabel->setScaledContents(true);

    // scrollArea->setBackgroundRole(QPalette::Dark);
    // scrollArea->setWidget(imageLabel);
    // scrollArea->setVisible(false);
    // setCentralWidget(scrollArea);
    // resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
    

    // scrollArea->setVisible(false);

    ui->setupUi(this);
    setup_image();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&MainWindow::update_data));
    
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

    // std::vector<uint8_t> raw_data(width * height);
    // for (int i = 0; i != raw_data.size(); ++i)
    //     raw_data[i] = i;
    // for (int y = 0; y < image->height(); y++) {
    //     memcpy(image->scanLine(y), &raw_data[y * image->width()], image->bytesPerLine());
    // }

    

    item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
    QGraphicsScene *scene = new QGraphicsScene;
    ui->view->setScene(scene);
    
    scene->addItem(item);
    item->setPos(0, 0);
    ui->view->show();
    ui->view->fitInView(item, Qt::KeepAspectRatio);
    // update_data();
    buffer.resize(512*1024);
}

void MainWindow::update_data(){
    //read the last image from zmq
    int64_t frame_number;
    receiver.receive_into(1, &frame_number, reinterpret_cast<std::byte*>(buffer.data()));
    //scale the image 

    //copy and update
    // std::vector<uint8_t> raw_data(image->width() * image->height());
    uint8_t j = start_value;
    for (int i = 0; i != buffer.size(); ++i)
        buffer[i] = j++;

    uint8_t* data = image->bits();
    for (int i = 0; i!=buffer.size(); ++i)
        *data++ = buffer[i];

    // memcpy(image->bits(), raw_data.data(), image->sizeInBytes());


    item->setPixmap(QPixmap::fromImage(*image));
    start_value++;
}

void MainWindow::on_startButton_clicked() {

    ui->startButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
    timer->start(10); //1000 -> 1s

    // 
}

void MainWindow::on_stopButton_clicked() {
    ui->startButton->setEnabled(true);
    ui->stopButton->setEnabled(false);
    timer->stop();
}

void MainWindow::on_actionButton_clicked() {
    for (int i=0; i<35; ++i){
        update_data();
    }
        
}


void MainWindow::resizeEvent(QResizeEvent *event) {
    ui->view->fitInView(item, Qt::KeepAspectRatio);
    std::cout << "resize\n";
}
