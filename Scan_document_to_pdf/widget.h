#ifndef WIDGET_H
#define WIDGET_H

#include <iostream>
#include <math.h>

#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/objdetect.hpp"

using namespace cv;
using namespace std;

#include <QWidget>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>
#include <QFileDialog>
#include <QString>
#include <QImage>
#include <QPoint>
#include <QPageLayout>
#include <QPainter>
#include <QPrinter>
#include <QRect>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public:
    Mat tranform(Mat src);
    void exportPDF(Mat matImageProcess);
    void readQRcode(Mat source);

private:
    Ui::Widget *ui;
    QCamera *mCamera;
    QCameraViewfinder *mCameraViewFinder;
    QCameraImageCapture *mCameraImageCapture;
    QVBoxLayout *mLayout;
    QMenu *mOptionMenu;
    QAction *mTurnOnAction;
    QAction *mTurnOffAction;
    QAction *mCaptureAction;
};
#endif // WIDGET_H
