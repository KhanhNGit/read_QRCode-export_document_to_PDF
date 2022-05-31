#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    // setup camera
    ui->setupUi(this);
    mCamera = new QCamera(this);
    mCameraViewFinder = new QCameraViewfinder(this);
    mCameraImageCapture = new QCameraImageCapture(mCamera,this);

    //create layout
    mLayout = new QVBoxLayout;
    mOptionMenu = new QMenu("Options",this);
    mTurnOnAction = new QAction("Turn on",this);
    mTurnOffAction = new QAction("Turn off",this);
    mCaptureAction = new QAction("Capture",this);

    //operation layout
    mOptionMenu->addActions({mTurnOnAction, mTurnOffAction, mCaptureAction});
    ui->optionPushButton->setMenu(mOptionMenu);
    mCamera->setViewfinder(mCameraViewFinder);
    mLayout->addWidget(mCameraViewFinder);
    mLayout->setMargin(0);
    ui->scrollArea->setLayout(mLayout);

    // start camera
    connect(mTurnOnAction, &QAction::triggered, [&](){
        mCamera->start();
    });

    // stop camera
    connect(mTurnOffAction, &QAction::triggered, [&](){
        mCamera->stop();
    });

    // capture the image to buffer for the next-processing
    connect(mCaptureAction, &QAction::triggered, [&](){
        mCameraImageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
        QImageEncoderSettings imageEncoderSettings;
        imageEncoderSettings.setCodec("image/jpg");
        mCameraImageCapture->setEncodingSettings(imageEncoderSettings);
        mCameraImageCapture->setBufferFormat(QVideoFrame::Format_RGB32);
        mCamera->setCaptureMode(QCamera::CaptureStillImage);
        mCamera->start();
        mCamera->searchAndLock();
        mCameraImageCapture->capture();
        mCamera->unlock();
    });

    // filter the image captured and export it to PDF
    connect(mCameraImageCapture, &QCameraImageCapture::imageCaptured, [=](int id,QImage mImage){
        Mat matImage = Mat(mImage.height(), mImage.width(), CV_8UC4, mImage.bits(), mImage.bytesPerLine());
        Widget::readQRcode(matImage);
        Mat matImageProcess = Widget::tranform(matImage);
        Widget::exportPDF(matImageProcess);
    });
}

Widget::~Widget()
{
    delete ui;
}

