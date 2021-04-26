
#include "ImagePlot.h"
#include <QWidget>
#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QGraphicsPixmapItem>

#include <opencv2/core/mat.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

// namespace reuss {


ImagePlot::ImagePlot(QWidget *parent){
    load_ctable();
    int pixels_x = 1024;
    int pixels_y = 512;
    image = new QImage(pixels_x, pixels_y, QImage::Format_Indexed8);
    image->setColorTable(ctable);
    item = new QGraphicsPixmapItem(QPixmap::fromImage(*image));
    QGraphicsScene *scene = new QGraphicsScene;

    setScene(scene);
    scene->addItem(item);
    item->setPos(0, 0);
}

void ImagePlot::load_ctable() {
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

void ImagePlot::Fit(){
    fitInView(item, Qt::KeepAspectRatio);
}

void ImagePlot::set_clim(double min, double max){
    cmin = min;
    cmax = max;
}

void ImagePlot::set_data(int i){
    //normalize on interval 0-256

    // //copy
    // uint8_t* dst = image->bits();
    // for (int i = 0; i!=(512*1024); ++i){
    //     *dst++ = val;
    // }
    cv::Mat img_in(500, 1000, CV_8UC1, cv::Scalar(i));
    cv::Mat img_color;

    cv::applyColorMap(img_in, img_color, cv::COLORMAP_JET);
    QImage img = QImage((uchar*)img_color.data, img_color.cols, img_color.rows, img_color.step, QImage::QImage::Format_Indexed8);
        

    item->setPixmap(QPixmap::fromImage(img));
}

// } // namespace reuss