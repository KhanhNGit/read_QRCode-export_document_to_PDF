#pragma once

#include "widget.h"

// read qrCode
void Widget::readQRcode(Mat source){
    QRCodeDetector qrDet;
    vector<Point> vertices;
    bool detection_result = qrDet.detect(source, vertices);
    Mat straightQRCode;
    String data, display;
    if(detection_result){
        data = qrDet.decode(source, vertices, straightQRCode);
        if(data.length()==0){
            data=qrDet.decodeCurved(source, vertices, straightQRCode);
        }
    }
    if(data.length()>0){
        display = "QRCode is: " + data;
    }else{
        display = "Can not detect or read QRCode";
    }
    QMessageBox::information(this, "QRCode Reader", display.data());
}
