#include <jni.h>
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/photo/photo.hpp"
#include <stdio.h>
#include <vector>
#include <stdio.h>

using namespace std;
using namespace cv;

int getDisp(Mat g1, Mat g2, Mat &disp);
int getThreshold(Mat img, Point p1, int range, Mat &foreground);
int segmentForeground(Mat img, Mat &foreground, Mat &background);
int getBlurMaskedImage(Mat img, Mat &foreground);
int getMaskedImage(Mat img, Mat &foreground);
int addFgBg(Mat foreground, Mat background, Mat &img);
int getBlurMaskedGrayImage(Mat img, Mat &foreground);
int getMaskedGrayImage(Mat img, Mat &foreground);
int deFocus(Mat img, Mat& foreground, int size, int radius);
int deFocus8(Mat img, Mat& foreground, int size, int w, int h);
int doBokeh(Mat disp, Mat img);
int doBokehImg(Mat disp, Mat img, Mat& foreground);
int getSepia(Mat img, Mat &foreground);
int doBokehImgRelative(Mat disp, Mat img, Mat& foreground, Point p1);
int getGaussianBlur(Mat img, Mat& retVal, int ksize);
int getThresh(Mat img, Mat& retVal, int l1, int l2, int h1, int h2);
int stackUp(vector<Mat>& layers, Mat& retVal);
int doMultiBlur(Mat img, Mat& retVal, Mat disp, Point p1);
int doCircBlur(Mat img, Mat& retVal, int radius);
int getDisparity(Mat g1, Mat g2, Mat &disp);
int segmentBlurs(Mat img, Mat &foreground);
int doOilPaint(Mat src, Mat& foreground);
int doGraySingle(Mat img, Mat& retVal, Mat disp, Point p1);
int getRange(Mat disp, Point p1);

int getGray(Mat& img)
{
  cvtColor(img, img, CV_BGR2GRAY);
  return 1;
}
extern "C" {
JNIEXPORT void JNICALL Java_com_tesseract_refocus_MainActivity_getThreshold(JNIEnv*, jobject, jlong addrBgr, jlong addrDisp, jlong finalImage, jint ji1, jint ji2,jint mode);
JNIEXPORT void JNICALL Java_com_tesseract_refocus_MainActivity_getDisparity(JNIEnv*, jobject, jlong addrRgba, jlong finalImage);
JNIEXPORT void JNICALL Java_com_tesseract_refocus_MainActivity_crop5(JNIEnv*, jobject, jlong addrRgba, jlong finalImage);

JNIEXPORT void JNICALL Java_com_tesseract_refocus_MainActivity_crop5(JNIEnv*, jobject, jlong addrRgba, jlong finalImage)
{
  Mat& img = *(Mat*)addrRgba;
  Mat& retVal = *(Mat*)finalImage;

  Mat img1(img, Rect(70, 100, 500, 500));
  img1.copyTo(retVal);

}
JNIEXPORT void JNICALL Java_com_tesseract_refocus_MainActivity_getDisparity(JNIEnv*, jobject, jlong addrRgba, jlong finalImage)
{
    Mat& img = *(Mat*)addrRgba;
    Mat g1, g2;
    Mat& disp = *(Mat*)finalImage;
    cvtColor(img, img, CV_RGBA2BGR);
    Mat img1(img, Rect(0, 0, img.cols/2, img.rows));
    Mat img2(img, Rect(img.cols/2, 0, img.cols/2, img.rows));
    cvtColor(img1, g1, CV_BGR2GRAY);
    cvtColor(img2, g2, CV_BGR2GRAY);
    getDisp(g1, g2, disp);
    imwrite("/mnt/sdcard/SimpleImageCapture/disp.jpg", disp);
    Mat newDisp(disp, Rect(70, 100, 500, 500));
    Mat inpaintmask;
    threshold(newDisp, inpaintmask, 20, 255, THRESH_BINARY_INV);
    inpaint(newDisp, inpaintmask, disp, 10, INPAINT_NS);
    imwrite("/mnt/sdcard/SimpleImageCapture/inpaintdisp.jpg", disp);
    //cvtColor(finImg, finImg, CV_BGR2RGBA);
    return;
}
JNIEXPORT void JNICALL Java_com_tesseract_refocus_MainActivity_getThreshold(JNIEnv*, jobject, jlong addrBgr, jlong addrDisp, jlong finalImage, jint ji1, jint ji2,jint currentMode)
{
  Mat& img = *(Mat*)addrBgr;
  Mat& disp = *(Mat*)addrDisp;
  Mat background;
  Mat& finImg = *(Mat*)finalImage;
  Mat img1(img, Rect(0, 0, img.cols/2, img.rows));
    Mat img2(img, Rect(img.cols/2, 0, img.cols/2, img.rows));
    Mat g1, g2;
    cvtColor(img1, g1, CV_BGR2GRAY);
    cvtColor(img2, g2, CV_BGR2GRAY);
  Point point1;
    int x, y;
    x = ji1;
    y = ji2;
    point1 = Point(x, y); // to get from android
    //getDisparity(g1, g2, disp);
    Mat newImg1(img1, Rect(70, 100, 500, 500));
    Mat newDisp;
    disp.copyTo(newDisp);

    if(currentMode == 1)
     {
         doMultiBlur(newImg1, finImg, newDisp, point1);
     }
     else
     {
     	doGraySingle(newImg1, finImg, newDisp, point1);
     }

    //Mat newDisp(disp, Rect(70, 100, 500, 500));
    //doMultiBlur(newImg1, finImg, newDisp, point1);

    //imwrite("multGaussBokeh.jpg", finImg);

    /////////getThreshold(newDisp, point1, 10, finImg);
    //Mat newDisp(disp, Rect(70, 100, 500, 500));
    ///////////segmentForeground(newImg1, finImg, background);
    //getMaskedGrayImage(img1, background);
    //getSepia(img1, background);
    //doBokehImg(disp, img1, background);

//    if(currentMode==1)
//    doBokehImgRelative(newDisp, newImg1, background, point1);
//    else if(currentMode==2)
//	doOilPaint(newImg1, background);
//    else if(currentMode==3)
//	getMaskedGrayImage(newImg1,background);
//    else if(currentMode==4)
//	 getSepia(newImg1, background);

    // getMaskedImage(newImg1, finImg);
    imwrite("/mnt/sdcard/SimpleImageCapture/img_fg.jpg", finImg);
    //imwrite("/mnt/sdcard/SimpleImageCapture/img_bg.jpg", background);


    //getMaskedImage(newImg1, finImg);

    //addFgBg(finImg, background, finImg);
  //  imwrite("/mnt/sdcard/SimpleImageCapture/img_fin.jpg", finImg);
    //resize(finImg, finImg, Size(finImg.cols*2, finImg.rows));
    return;
}
}


int getDisp(Mat g1, Mat g2, Mat &disp)
{
    Mat disp16;
    StereoSGBM sbm;
    sbm.SADWindowSize = 5; // 5
    sbm.numberOfDisparities = 192;
    sbm.preFilterCap = 4;
    sbm.minDisparity = -32; // -64
    sbm.uniquenessRatio = 1; // 1
    sbm.speckleWindowSize = 150; //150
    sbm.speckleRange = 2;
    sbm.disp12MaxDiff = 10; // 10
    sbm.fullDP = false;
    sbm.P1 = 600;
    sbm.P2 = 2400;
    /*
    sbm.SADWindowSize = 5; // 5
    sbm.numberOfDisparities = 112;
    sbm.preFilterCap = 61;
    sbm.minDisparity = -39; // -64
    sbm.uniquenessRatio = 1; // 1
    sbm.speckleWindowSize = 180; //150
    sbm.speckleRange = 2;
    sbm.disp12MaxDiff = 20; // 10
    sbm.fullDP = false;
    sbm.P1 = 600;
    sbm.P2 = 2400;
    */
    sbm(g1, g2, disp16);
    normalize(disp16, disp, 0, 255, CV_MINMAX, CV_8U);
    if (disp.cols > 0 && disp.rows > 0)
    {
        return 1;
    }
    return 0;
}

int getThreshold(Mat img, Point p1, int range, Mat &foreground)
{
    int disval;
    disval = img.at<uchar>(p1.y, p1.x);
    inRange(img, disval - range, disval + range, foreground);
    medianBlur(foreground, foreground, 9);
    return 1;
}

int segmentForeground(Mat img, Mat &foreground, Mat &background)
{
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat drawing, kernel;
    int size=3;
    drawing = Mat::zeros(img.size(), CV_8UC3);
    findContours(foreground.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    for (int i=0; i<contours.size(); i++)
    {
        if (contourArea(contours[i]) > 15000)
        {
            drawContours(drawing, contours, i, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy, 0, Point());
        }
    }

    kernel = getStructuringElement(MORPH_ELLIPSE, Size(2*size+1, 2*size+1), Point(size, size));
    erode(drawing, drawing, kernel, Point(-1, -1), 2);
    Mat temp;
    dilate(drawing, temp, kernel, Point(-1, -1), 1);
    drawing = Mat::zeros(img.size(), CV_8UC3);
    contours.clear();
    hierarchy.clear();
    cvtColor(temp, temp, CV_BGR2GRAY);
    findContours(temp.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    for (int i=0; i<contours.size(); i++)
    {
        if (contourArea(contours[i]) > 15000)
        {
            drawContours(drawing, contours, i, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy, 0, Point());
        }
    }
    dilate(drawing, drawing, kernel, Point(-1, -1), 1);
    foreground = drawing.clone();
    background = Scalar(255, 255, 255) - foreground;
    return 1;
}

int getBlurMaskedGrayImage(Mat img, Mat &foreground)
{
    Mat blur, blurGray;
    cvtColor(img, blurGray, CV_BGR2GRAY);
    GaussianBlur(blurGray, blurGray, Size(9, 9), 11, 11);
    vector<Mat> gray;
    gray.push_back(blurGray);
    gray.push_back(blurGray);
    gray.push_back(blurGray);
    merge(gray, blur);
    bitwise_and(blur, foreground, foreground);
    //bilateralFilter(foreground, newImg, 20, 100.0, 150.0, BORDER_DEFAULT);
    //GaussianBlur(newImg, foreground, Size(9, 9), 11, 11);
    //foreground = newImg.clone();
    return 1;
}
int getBlurMaskedImage(Mat img, Mat &foreground)
{
    Mat blur;
    GaussianBlur(img, blur, Size(13, 13), 15, 15);
    bitwise_and(blur, foreground, foreground);
    return 1;
}
int getMaskedImage(Mat img, Mat &foreground)
{
    bitwise_and(img, foreground, foreground);
    return 1;
}

int getMaskedGrayImage(Mat img, Mat &foreground)
{
    Mat g, bitwise;
    cvtColor(img, g, CV_BGR2GRAY);
    vector<Mat> gray;
    gray.push_back(g);
    gray.push_back(g);
    gray.push_back(g);
    merge(gray, bitwise);
    bitwise_and(bitwise, foreground, foreground);
    return 1;
}

int addFgBg(Mat foreground, Mat background, Mat &img)
{
    Mat tempImg;
    add(foreground, background, img);
    return 1;
}

int deFocus(Mat img, Mat& foreground, int size, int radius)
{
    Mat filter, blur;
    float totalSum;
    filter = Mat::zeros(size, size, CV_64F);
    circle(filter, Point(size/2, size/2), radius, Scalar(1, 1, 1), -1);
    totalSum = sum(filter)[0];
    filter = filter/totalSum;
    filter2D(img, blur, -1, filter);
    bitwise_and(blur, foreground, foreground);
    return 1;
}

int deFocus8(Mat img, Mat& foreground, int size, int w, int h)
{
    Mat filter1, filter2, rotMat, blur;
    float totalSum;
    filter1 = Mat::zeros(size, size, CV_64F);
    rectangle(filter1, Point(size/2-w/2, size/2-h/2),
              Point(size/2+w/2, size/2+h/2),
              Scalar(255, 255, 255), -1);
    rotMat = getRotationMatrix2D(Point(size/2, size/2), 45.0, 1.0);
    warpAffine(filter1, filter2, rotMat, Size(size, size));
    bitwise_or(filter1, filter2, filter1);
    filter1 = filter1/255.0;
    totalSum = sum(filter1)[0];
    filter1 = filter1/totalSum;
    filter2D(img, blur, -1, filter1);
    bitwise_and(blur, foreground, foreground);
    return 1;

}

int doBokeh(Mat disp, Mat img)
{
    int i, j, disval, size=30, dia;
    float tSum;
    Mat cImg;
    Mat bImg, filter;
    cImg = img.clone();
    bImg = img.clone();

    for(i=0; i<img.rows-size; i+=size/2)
    {
        for(j=0; j<img.cols-size; j+=size/2)
        {
            Mat subDisp = disp(Range(i, i+size), Range(j, j+size));
            Mat subImg = cImg(Range(i, i+size), Range(j, j+size));
            disval = sum(subDisp)[0]/(size*size);
            dia = 13 - disval/20;
            if (dia < 2)
                continue;
            filter = Mat::zeros(15, 15, CV_64F);
            printf("(%d, %d) %d %d\n", i, j, dia, disval);
            circle(filter, Point(7, 7), dia/2+1, (1, 1, 1), -1);
            tSum = sum(filter)[0];
            filter = filter/tSum;
            filter2D(subImg, subImg, -1, filter);
            //subImg.copyTo(bImg(Rect(i, j, size, size)));
            subImg.copyTo(bImg.colRange(j, j+size).rowRange(i, i+size));
            //bImg(Range(i, i+size), Range(j, j+size)) = subImg;
        }
    }
    imshow("bokeh", bImg);
    return 1;
}

int doBokehImg(Mat disp, Mat img, Mat& foreground)
{
    int i, j, disval, size=30, dia;
    float tSum;
    Mat cImg;
    Mat bImg, filter;
    cImg = img.clone();
    bImg = img.clone();

    for(i=0; i<img.rows-size; i+=size/2)
    {
        for(j=0; j<img.cols-size; j+=size/2)
        {
            Mat subDisp = disp(Range(i, i+size), Range(j, j+size));
            Mat subImg = cImg(Range(i, i+size), Range(j, j+size));
            disval = sum(subDisp)[0]/(size*size);
            dia = 13 - disval/20;
            if (dia < 2)
                continue;
            filter = Mat::zeros(15, 15, CV_64F);
            //printf("(%d, %d) %d %d\n", i, j, dia, disval);
            circle(filter, Point(7, 7), dia/2+1, (1, 1, 1), -1);
            tSum = sum(filter)[0];
            filter = filter/tSum;
            filter2D(subImg, subImg, -1, filter);
            //subImg.copyTo(bImg(Rect(i, j, size, size)));
            subImg.copyTo(bImg.colRange(j, j+size).rowRange(i, i+size));
            //bImg(Range(i, i+size), Range(j, j+size)) = subImg;
        }
    }
    //imshow("bokeh", bImg);
    bitwise_and(bImg, foreground, foreground);
    return 1;
}

int getSepia(Mat img, Mat &foreground)
{
    Mat cImg, kernel;
    kernel = (cv::Mat_<float>(3,3) <<  0.272, 0.534, 0.131,
                                        0.349, 0.686, 0.168,
                                        0.393, 0.769, 0.189);//,
                                        //0, 0, 0, 1);
    transform(img, cImg, kernel);
    bitwise_and(cImg, foreground, foreground);
    return 1;
}

int doBokehImgRelative(Mat disp, Mat img, Mat& foreground, Point p1)
{
    int dispVal;
    dispVal = disp.at<uchar>(p1.y, p1.x);
    int i, j, disval, size=30, dia, diff;
    float tSum;
    Mat cImg;
    Mat bImg, filter;
    cImg = img.clone();
    bImg = img.clone();

    for(i=0; i<img.rows-size; i+=size/2)
    {
        for(j=0; j<img.cols-size; j+=size/2)
        {
            Mat subDisp = disp(Range(i, i+size), Range(j, j+size));
            Mat subImg = cImg(Range(i, i+size), Range(j, j+size));
            disval = sum(subDisp)[0]/(size*size);
            diff = abs(dispVal - disval);
            if (diff < 12)
                continue;
            dia = diff/10;
            if (dia < 2)
                continue;
            filter = Mat::zeros(15, 15, CV_64F);
            //printf("(%d, %d) %d %d\n", i, j, dia, disval);
            circle(filter, Point(7, 7), dia/2+1, (1, 1, 1), -1);
            tSum = sum(filter)[0];
            filter = filter/tSum;
            filter2D(subImg, subImg, -1, filter);
            //subImg.copyTo(bImg(Rect(i, j, size, size)));
            subImg.copyTo(bImg.colRange(j, j+size).rowRange(i, i+size));
            //bImg(Range(i, i+size), Range(j, j+size)) = subImg;
        }
    }
    //bImg.copyTo(foreground);
    //imshow("bokeh", bImg);
    imwrite("bokehImageRelative.jpg", bImg);
    bitwise_and(bImg, foreground, foreground);
    return 1;
}

int doMultiBlur(Mat img, Mat& retVal, Mat disp, Point p1)
{
    int dispval, range, i, lval, hval;
    int l1, l2, h1, h2;
    vector<Mat> layers, blurs, finLayers;
    //Mat thresh, blur, bitwiseImg;
    int threshVal;

    dispval = disp.at<uchar>(p1.y, p1.x);
    range = dispval/10;
    //printf("%d %d\n", range, dispval);
    lval = dispval+1;
    hval = dispval-1;
    for(i=1; i<100; i++)
    {
        l1 = lval - range;
        l2 = lval;
        h1 = hval;
        h2 = hval + range;
        //printf("%d %d %d %d\n", l1, l2, h1, h2);
        Mat thresh;
        Mat seg;
        threshVal = getThresh(disp, thresh, l1, l2, h1, h2);
        if (!threshVal)
        {
            //printf("break\n");
            break;
        }
        thresh.copyTo(seg);
        segmentBlurs(thresh, seg);
        //imshow("thresh", thresh);
        //imshow("seg", seg);
        //waitKey(0);
        layers.push_back(seg);

        lval = l1;
        hval = h2;
        range*=2;
    }

    blurs.push_back(img);
    for(i=1; i<layers.size(); i++)
    {
        Mat blur;
        GaussianBlur(img, blur, Size(19, 19), 2*i);
        //doCircBlur(img, blur, 3*i);
        //imshow("blur", blur);
        //waitKey(0);
        blurs.push_back(blur);
    }
    int sigma = 2*i;
    Mat backLayer;
    backLayer = Mat::zeros(img.cols, img.rows, CV_8UC3);
    for(i=0; i<layers.size(); i++)
    {
        Mat bitwiseImg;
        bitwise_and(layers[i], blurs[i], bitwiseImg);
        add(backLayer, layers[i], backLayer);
        //imshow("thresh", layers[i]);
        //imshow("bitwiseImg", bitwiseImg);
        //waitKey(0);
        finLayers.push_back(bitwiseImg);
    }

    Mat blurImage;
    backLayer = Scalar(255, 255, 255) - backLayer;
    //imshow("backLayer", backLayer);
    //waitKey(0);
    GaussianBlur(img, blurImage, Size(19, 19), sigma);
    //imshow("bluriamge", blurImage);
    bitwise_and(blurImage, backLayer, backLayer);
    //imshow("backLayer", backLayer);
    //waitKey(0);
    finLayers.push_back(backLayer);
    //blurs.push_back(blurImage);

    stackUp(finLayers, retVal);

    return 1;


}

int getThresh(Mat img, Mat& retVal, int l1, int l2, int h1, int h2)
{
    Mat thresh1, thresh2, thresh;
    if (l2 < 0 && h1 > 255)
    {
        printf("return 0\n");
        return 0;
    }

    if (l1 < 0 && l2 < 0)
    {
        thresh1 = Mat::zeros(img.size(), CV_8U);
    }
    else if(l1 < 0)
    {
        l1 = 0;
        inRange(img, l1, l2-1, thresh1);
    }
    else
    {
        inRange(img, l1, l2-1, thresh1);
    }

    if (h2 > 255 && h1 > 255)
    {
        thresh2 = Mat::zeros(img.size(), CV_8U);
    }
    else if (h2 > 255)
    {
        h2 = 255;
        inRange(img, h1+1, h2, thresh2);
    }
    else
    {
        inRange(img, h1+1, h2, thresh2);
    }

    //imshow("thresh1", thresh1);
    //imshow("thresh2", thresh2);
    bitwise_or(thresh1, thresh2, thresh);

    vector<Mat> threshLayers;
    threshLayers.push_back(thresh);
    threshLayers.push_back(thresh);
    threshLayers.push_back(thresh);

    merge(threshLayers, retVal);

    if (retVal.size() == img.size())
    {
        return 1;
    }
    return 0;
}

int getGaussianBlur(Mat img, Mat& retVal, int ksize)
{
    GaussianBlur(img, retVal, Size(ksize, ksize), 0);
    if (retVal.size() == img.size())
    {
        return 1;
    }
    return 0;
}

int stackUp(vector<Mat>& layers, Mat& retVal)
{
    int i;
    retVal = Mat::zeros(layers[i].size(), CV_8UC3);
    for(i=0; i<layers.size(); i++)
    {
        add(retVal, layers[i], retVal);
    }
    if (retVal.size() == layers[0].size())
    {
        return 1;
    }
    return 0;
}

int doCircBlur(Mat img, Mat& retVal, int radius)
{
    Mat circ;
    int tSum;

    circ = Mat::zeros(31, 31, CV_64F);
    circle(circ, Point(15, 15), radius, (1, 1, 1), -1);
    tSum = sum(circ)[0];
    circ = circ/tSum;

    filter2D(img, retVal, -1, circ);
    if (retVal.size() == img.size())
    {
        return 1;
    }
    return 0;
}

int getDisparity(Mat g1, Mat g2, Mat &disp)
{
    Mat disp16;
    StereoSGBM sbm;
    sbm.SADWindowSize = 7; // 5
    sbm.numberOfDisparities = 128; //192
    sbm.preFilterCap = 4;
    sbm.minDisparity = -39; // -64
    sbm.uniquenessRatio = 9; // 1
    sbm.speckleWindowSize = 180; //150
    sbm.speckleRange = 2;
    sbm.disp12MaxDiff = 20; // 10
    sbm.fullDP = false;
    sbm.P1 = 600;
    sbm.P2 = 2400;
    sbm(g1, g2, disp16);
    normalize(disp16, disp, 0, 255, CV_MINMAX, CV_8U);
    //GaussianBlur(disp, disp, Size(5,5), 0);
    //Mat equalizeDisp;

    //equalizeHist(disp, equalizeDisp);
    //equalizeHist(disp, disp);

    //imshow("disparity",disp);
    //imshow("equalizeDisp", equalizeDisp);
    if (disp.cols > 0 && disp.rows > 0)
    {
        return 1;
    }
    return 0;
}

int segmentBlurs(Mat img, Mat &foreground)
{
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat drawing, kernel;
    int size=3;
    drawing = Mat::zeros(img.size(), CV_8UC3);
    Mat fg;
    cvtColor(foreground, fg, CV_BGR2GRAY);
    findContours(fg.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    for (int i=0; i<contours.size(); i++)
    {
        if (contourArea(contours[i]) > 1000)
        {
            printf("%lf\n", contourArea(contours[i]));
            drawContours(drawing, contours, i, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy, 0, Point());
        }
    }

    kernel = getStructuringElement(MORPH_ELLIPSE, Size(2*size+1, 2*size+1), Point(size, size));
    //erode(drawing, drawing, kernel, Point(-1, -1), 2);
    Mat temp;
    //dilate(drawing, temp, kernel, Point(-1, -1), 1);
    temp = drawing.clone();
    drawing = Mat::zeros(img.size(), CV_8UC3);
    contours.clear();
    hierarchy.clear();
    cvtColor(temp, temp, CV_BGR2GRAY);
    findContours(temp.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    for (int i=0; i<contours.size(); i++)
    {
        if (contourArea(contours[i]) > 2000)
        {
            printf("%lf\n", contourArea(contours[i]));
            drawContours(drawing, contours, i, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy, 0, Point());
        }
    }
    //dilate(drawing, drawing, kernel, Point(-1, -1), 1);
    foreground = drawing.clone();
    //imshow("contours", drawing);
    //background = Scalar(255, 255, 255) - foreground;

    drawing = Mat::zeros(img.size(), CV_8UC3);
    contours.clear();
    hierarchy.clear();
    //bitwise_and(img, foreground, temp);
    //imwrite("checkblobs.jpg", temp);
    /*
    Mat grabcutimg;
    //doGrabCut(temp, grabcutimg, Point(360, 75), Point(450, 620));
    doGrabCut(temp, grabcutimg, Point(204, 96), Point(340, 542));
    //rectangle(grabcutimg, Point(204, 96), Point(340, 542), Scalar(255, 0, 0), 2);
    Mat threshGrabcut;
    threshold(grabcutimg, threshGrabcut, 180, 255, THRESH_BINARY);
    imshow("grabcut threshold", threshGrabcut);
    vector<Mat> grabcut3;
    grabcut3.push_back(threshGrabcut);
    grabcut3.push_back(threshGrabcut);
    grabcut3.push_back(threshGrabcut);
    merge(grabcut3, foreground);
    */
    //background = Scalar(255, 255, 255) - foreground;

    /*
    cvtColor(temp, temp, CV_BGR2GRAY);
    Canny(temp, temp, 100, 200);
    imshow("temp", temp);
    findContours(temp.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    printf("number of contours = %ld\n", contours.size());
    for (int i=0; i<contours.size(); i++)
    {
        drawContours(drawing, contours, i, Scalar(255, 0, 255), 1, 8, hierarchy, 0, Point());
        //imshow("contours in the making", drawing);
        //waitKey(0);
    }
    imshow("contours on color", drawing);
    */
    return 1;
}

int doOilPaint(Mat src, Mat& foreground)
{
    Mat dst;
    int nRadius = 5;
    int fIntensityLevels = 20;

    //dst = src.clone();
    dst = Mat::zeros(src.size(), src.type());

    for(int nY = nRadius; nY < (src.rows - nRadius); nY++)
    {
        for(int nX = nRadius; nX < (src.cols - nRadius); nX++)
        {
            int nIntensityCount[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            int nSumB[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            int nSumG[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            int nSumR[21] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

            for(int nY_O = -nRadius; nY_O <= nRadius; nY_O++)
            {
                for(int nX_O = -nRadius; nX_O <= nRadius; nX_O++)
                {
                    int nB = src.at<Vec3b>((nY+nY_O), (nX+nX_O))[0];
                    int nG = src.at<Vec3b>((nY+nY_O), (nX+nX_O))[1];
                    int nR = src.at<Vec3b>((nY+nY_O), (nX+nX_O))[2];

                    int nCurIntensity = (((nB+nG+nR)/3.0)*fIntensityLevels)/255;
                    if(nCurIntensity > 255)
                    {
                        nCurIntensity = 255;
                    }
                    int i = nCurIntensity;
                    nIntensityCount[i]++;

                    nSumB[i] = nSumB[i] + nB;
                    nSumG[i] = nSumG[i] + nG;
                    nSumR[i] = nSumR[i] + nR;

                }
            }

            int nCurMax = 0;
            int nMaxIndex = 0;

            for(int nI = 0; nI<21; nI++)
            {
                if(nIntensityCount[nI] > nCurMax)
                {
                    nCurMax = nIntensityCount[nI];
                    nMaxIndex = nI;
                }
            }

            dst.at<Vec3b>(nY, nX)[0] = nSumB[nMaxIndex]/nCurMax;
            dst.at<Vec3b>(nY, nX)[1] = nSumG[nMaxIndex]/nCurMax;
            dst.at<Vec3b>(nY, nX)[2] = nSumR[nMaxIndex]/nCurMax;
        }
    }
    bitwise_and(dst, foreground, foreground);
    return 1;
}

int doGraySingle(Mat img, Mat& retVal, Mat disp, Point p1)
{
    int dispval, range;
    int l1, l2, h1, h2;
    dispval = disp.at<uchar>(p1.y, p1.x);
    range = getRange(disp, p1);
    //range = dispval/10;
    //printf("%d %d\n", range, dispval);
    Mat thresh;
    l1 = dispval - range;
    l2 = dispval;
    h1 = dispval;
    h2 = dispval + range;

    inRange(disp, dispval - range, dispval + range, thresh);
    vector<Mat> threshvec;
    threshvec.push_back(thresh);
    threshvec.push_back(thresh);
    threshvec.push_back(thresh);
    merge(threshvec, thresh);
    //getThresh(disp, thresh, l1, l2, h1, h2);
    bitwise_and(img, thresh, retVal);
    //printf("first bitwise done\n");
    thresh = Scalar(255, 255, 255) - thresh;

    Mat gray;
    cvtColor(img, gray, CV_BGR2GRAY);
    //printf("%d\n", gray.channels());
    vector<Mat> grayvec;
    grayvec.push_back(gray);
    grayvec.push_back(gray);
    grayvec.push_back(gray);
    merge(grayvec, gray);
    //printf("%d\n", gray.channels());
    //imshow("thresh", thresh);
    //imshow("graey", gray);
    //waitKey(0);
    bitwise_and(gray, thresh, gray);
    //printf("second bitwise done\n");
    add(gray, retVal, retVal);
	return 1;
}

int getRange(Mat disp, Point p1)
{
    int dispval, range;
    dispval = disp.at<uchar>(p1.y, p1.x);
    range = dispval/8;
    Mat thresh;

    inRange(disp, dispval - range, dispval + range, thresh);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat drawing, kernel;
    int size=3;
    drawing = Mat::zeros(thresh.size(), CV_8UC3);
    Mat fg;
    //cvtColor(foreground, fg, CV_BGR2GRAY);
    findContours(thresh.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    for (int i=0; i<contours.size(); i++)
    {
        if (contourArea(contours[i]) > 1000)
        {
            //printf("%lf\n", contourArea(contours[i]));
            drawContours(drawing, contours, i, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy, 0, Point());
        }
    }

    kernel = getStructuringElement(MORPH_ELLIPSE, Size(2*size+1, 2*size+1), Point(size, size));
    //erode(drawing, drawing, kernel, Point(-1, -1), 2);
    Mat temp;
    //dilate(drawing, temp, kernel, Point(-1, -1), 1);
    temp = drawing.clone();
    drawing = Mat::zeros(thresh.size(), CV_8UC1);
    contours.clear();
    hierarchy.clear();
    cvtColor(temp, temp, CV_BGR2GRAY);
    findContours(temp.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0));
    for (int i=0; i<contours.size(); i++)
    {
        if (contourArea(contours[i]) > 2000)
        {
            //printf("%lf\n", contourArea(contours[i]));
            drawContours(drawing, contours, i, Scalar(255, 255, 255), CV_FILLED, 8, hierarchy, 0, Point());
        }
    }
    //dilate(drawing, drawing, kernel, Point(-1, -1), 1);
    //foreground = drawing.clone();
    //imshow("contours", drawing);
    //background = Scalar(255, 255, 255) - foreground;

    Mat dispClone;
    bitwise_and(disp, drawing, dispClone);
    //imshow("dispClone", dispClone);
    Point minP, maxP;
    double minVal, maxVal;

    minMaxLoc(dispClone, &minVal, &maxVal, &minP, &maxP, thresh);
    //printf("%lf %lf\n", minVal, maxVal);

    //drawing = Mat::zeros(thresh.size(), CV_8UC3);
    contours.clear();
    hierarchy.clear();
    return (maxVal - dispval);
}
