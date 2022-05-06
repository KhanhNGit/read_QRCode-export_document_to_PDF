#pragma once

#include "widget.h"

// convert mat to qImage
QImage MatToQImage(Mat mat){
    // 8-bits unsigned, NO. OF CHANNELS=1
    if(mat.type()==CV_8UC1){
        // Set the color table (used to translate colour indexes to qRgb values)
        QVector<QRgb> colorTable;
        for (int i=0; i<256; i++){
            colorTable.push_back(qRgb(i,i,i));
        }
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
        img.setColorTable(colorTable);
        return img;
    }
    // 8-bits unsigned, NO. OF CHANNELS=3
    if(mat.type()==CV_8UC3){
        // Copy input Mat
        const uchar *qImageBuffer = (const uchar*)mat.data;
        // Create QImage with same dimensions as input Mat
        QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        return img.rgbSwapped();
    }
    return QImage();
}

// export image-processed to PDF
void Widget::exportPDF(Mat matImageProcess){
    auto filename = QFileDialog::getSaveFileName(this, "Save As", "/", "PDF(*.pdf)");
    if(filename.isEmpty()){
        return;
    }
    QImage mImageProcess = MatToQImage(matImageProcess);
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setFullPage(true);
    printer.setOutputFileName(filename);
    QPainter painter;
    painter.begin(&printer);
    QRect rect = painter.viewport();
    double ratioWidth = rect.width() / mImageProcess.width();
    double ratioHeight = rect.height() / mImageProcess.height();
    double ratio = ratioWidth < ratioHeight ? ratioWidth : ratioHeight;
    painter.scale(ratio,ratio);
    double x = (rect.width()/ratio - mImageProcess.width())/2;
    double y = (rect.height()/ratio - mImageProcess.height())/2;
    painter.drawImage(QPoint(x, y), mImageProcess);
    painter.end();
}
