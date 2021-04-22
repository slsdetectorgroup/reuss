#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QRgb>
#include <vector>
#include <QRect>
#include <QPoint>
#include <QResizeEvent>
#include <QImage>
#include <QLabel>
#include <QScrollArea>
#include <QGraphicsPixmapItem>
#include <QTimer>

#include "reuss/ZmqReceiver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startButton_clicked();
    void on_stopButton_clicked();
    void on_actionButton_clicked();
    

private:
    Ui::MainWindow *ui;

    void setup_image();
    void update_image();

    QImage *image;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor = 1;
    QGraphicsPixmapItem *item;
    
    QVector<QRgb> ctable;
    void load_ctable();


    uint8_t start_value = 0;
    QTimer* timer;


    reuss::ZmqReceiver receiver{"tcp://localhost:4545"};


    void resizeEvent(QResizeEvent *event);
    std::vector<uint16_t>buffer;
    
};

#endif // MAINWINDOW_H
