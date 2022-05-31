#pragma once

#include "widget.h"

// use in contours sorting
bool compareContourArea(vector<Point> contour1, vector<Point> contour2) {
    return fabs(contourArea(contour1)) > fabs(contourArea(contour2));
}

// use in corners sorting
double max_two_element(double element1, double element2) {
    return element1 > element2 ? element1 : element2;
}
double max_four_element(double element[4]) {
    double max1 = max_two_element(element[0], element[1]);
    double max2 = max_two_element(element[2], element[3]);
    return max_two_element(max1, max2);
}
double min_two_element(double element1, double element2) {
    return element1 < element2 ? element1 : element2;
}
double min_four_element(double element[4]) {
    double min1 = min_two_element(element[0], element[1]);
    double min2 = min_two_element(element[2], element[3]);
    return min_two_element(min1, min2);
}

// use in corners sorting
vector<Point> corners_sorted(vector<Point> corners) {
    double sum_of_coordinate[4];
    int id_max_sum, id_min_sum, id_max_diff, id_min_diff;
    for (int i = 0;i < corners.size();i++) {
        sum_of_coordinate[i] = corners[i].x + corners[i].y;
    }
    for (int i = 0;i < corners.size();i++) {
        if (corners[i].x + corners[i].y == max_four_element(sum_of_coordinate)) {
            id_max_sum = i;
            continue;
        }
        if (corners[i].x + corners[i].y == min_four_element(sum_of_coordinate)) {
            id_min_sum = i;
        }
    }
    vector<Point> two_left_points;
    for (int i = 0;i < corners.size();i++) {
        if (i != id_max_sum && i != id_min_sum) {
            two_left_points.push_back(corners[i]);
        }
    }
    if (two_left_points[0].x - two_left_points[0].y > two_left_points[1].x - two_left_points[1].y) {
        id_max_diff = 0;
        id_min_diff = 1;
    }
    else {
        id_max_diff = 1;
        id_min_diff = 0;
    }
    vector<Point> corners_temp({ corners[id_min_sum],two_left_points[id_max_diff],
                                 corners[id_max_sum],two_left_points[id_min_diff] });
    return corners_temp;
}

// use to determine the dstHeight
double maxHeight(vector<Point> vertices) {
    double distanceHeight1 = sqrt(pow(vertices[0].x - vertices[3].x, 2) + pow(vertices[0].y - vertices[3].y, 2));
    double distanceHeight2 = sqrt(pow(vertices[1].x - vertices[2].x, 2) + pow(vertices[1].y - vertices[2].y, 2));
    return (distanceHeight1 + distanceHeight2) / 2;
}

// use tp determine the dstWidth
double maxWidth(vector<Point> vertices) {
    double distanceWidth1 = sqrt(pow(vertices[0].x - vertices[1].x, 2) + pow(vertices[0].y - vertices[1].y, 2));
    double distanceWidth2 = sqrt(pow(vertices[2].x - vertices[3].x, 2) + pow(vertices[2].y - vertices[3].y, 2));
    return (distanceWidth1 + distanceWidth2) / 2;
}

Mat Widget::tranform(Mat src) {
    // Edge detection
    Mat src_resize, src_gray, src_canny;
    double height = src.size().height > src.size().width ? src.size().height : src.size().width;
    double resize_height;
    if (height > 1200) {
        resize_height = 360;
    }
    else {
        resize_height = 400;
    }
    if (height < resize_height) {
        resize_height = height;
    }
    double zoom = height / resize_height;
    cv::resize(src, src_resize, Size(), 1 / zoom, 1 / zoom);
    cvtColor(src_resize, src_gray, COLOR_BGR2GRAY);
    GaussianBlur(src_gray, src_gray, Size(7, 7), 0);
    Canny(src_gray, src_canny, 70, 200);

    //Finding Contours
    vector<vector<Point>> contours;
    findContours(src_canny, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);
    sort(contours.begin(), contours.end(), compareContourArea);

    // Find Corners
    vector<Point> corners;
    for (int i = 0;i < contours.size();i++) {
        double perimeter = arcLength(contours[i], true);
        vector<Point> approxCurve;
        approxPolyDP(contours[i], approxCurve, perimeter * 0.02, true);
        if (approxCurve.size() == 4) {
            corners = approxCurve;
            break;
        }
    }

    // Sort corners order by clockwise from top-left
    vector<Point> vertices = corners_sorted(corners);
    vector<Point> vertices_origin;
    for (int i = 0;i < vertices.size();i++) {
        vertices_origin.push_back(Point(vertices[i].x * zoom, vertices[i].y * zoom));
    }

    // Set the output height and width
    double dstHeight = maxHeight(vertices_origin);
    double dstWidth = maxWidth(vertices_origin);

    //getPerspectiveTranform and warpPerspective
    vector<Point>dstCorners({ Point(0,0),Point(dstWidth - 1,0),Point(dstWidth - 1,dstHeight - 1),Point(0,dstHeight - 1) });
    Mat persTrans = findHomography(vertices_origin, dstCorners);
    Mat dst = Mat(Size(dstWidth, dstHeight), CV_8UC3);
    warpPerspective(src, dst, persTrans, Size(dstWidth, dstHeight));

    // Rotate and resize the dst (adjust final result)
    if (dst.size().width > dst.size().height) {
        rotate(dst, dst, ROTATE_90_CLOCKWISE);
    }
    /*
    cvtColor(dst, dst, COLOR_BGR2GRAY);
    Scalar m = mean(dst);
    int threshold_max = 200, threshold_min = 60;
    double threshold_ = m[0];
    if (threshold_ > threshold_max)
        threshold_ = threshold_max;
    if (threshold_ < threshold_min)
        threshold_ = threshold_min;
    threshold(dst, dst, threshold_, 255, THRESH_TOZERO);
    */
    return dst;
}
