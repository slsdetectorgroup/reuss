#pragma once
#include <QGraphicsView>
#include <fmt/core.h>
#include <QImage>
// namespace reuss {

namespace Ui {
class ImagePlot;
}

class ImagePlot : public QGraphicsView {
    Q_OBJECT

    QImage *image;
    QGraphicsPixmapItem* item;
    QVector<QRgb> ctable;
    void load_ctable();

    double cmin=0.;
    double cmax = 10.;
  public:
    ImagePlot(QWidget *parent = nullptr);

    int say_hi(){fmt::print("Using ImagePlot\n"); return 0;}
    void set_clim(double min, double max);
    void Fit();

    void set_data(int i);
};

// } // namespace reuss

