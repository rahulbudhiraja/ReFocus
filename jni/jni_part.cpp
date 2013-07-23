#include <jni.h>
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/contrib/contrib.hpp"
#include <stdio.h>
#include <vector>
#include <stdio.h>
#include <android/log.h>

#define DEBUG_TAG "refocus"

#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,DEBUG_TAG,__VA_ARGS__)

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

int getGray(Mat& img)
{
	cvtColor(img, img, CV_BGR2GRAY);
	return 1;
}
extern "C" {



JNIEXPORT void JNICALL Java_com_tesseract_refocus_MainActivity_getDisparity(JNIEnv *env, jobject, jlong addrRgba, jlong finalImage, jint ji1, jint ji2);

JNIEXPORT void JNICALL Java_com_tesseract_refocus_MainActivity_getDisparity(JNIEnv *env, jobject obj, jlong addrRgba, jlong finalImage, jint ji1, jint ji2)
{
    // Will be accessing the static variable in Java,these structures will be required ..

////	 jclass cls = (*env)->GetObjectClass(env, obj);
//	 jfieldID fid;
//	 jint loading_progress;
//
////	 fid = (*env)->GetStaticFieldID(env, cls, "loading_progress", "I");
//	 si = (*env)->GetStaticIntField(env, cls, fid);
	// End of structures

	Mat& img = *(Mat*)addrRgba;
    Mat g1, g2, disp, foreground, background;
    Mat& finImg = *(Mat*)finalImage;
    Point point1;
    int x, y;

     LOGD("Start");

    x = ji1;
    y = ji2;
    point1 = Point(x, y); // to get from android



    LOGD("1");
//    (*env)->SetStaticIntField(env, cls, fid, 1);
//    loading_progress=1;
    cvtColor(img, img, CV_RGBA2BGR);
    Mat img1(img, Rect(0, 0, img.cols/2, img.rows));
    Mat img2(img, Rect(img.cols/2, 0, img.cols/2, img.rows));

    LOGD("Point 1");
    cvtColor(img1, g1, CV_BGR2GRAY);
    cvtColor(img2, g2, CV_BGR2GRAY);
//    loading_progress=2;
//    (*env)->SetStaticIntField(env, cls, fid, 2);
//    getDisp(g1, g2, disp);

//    (*env)->SetStaticIntField(env, cls, fid, 3);
//    loading_progress=3;

    LOGD("Point 2");
    getThreshold(disp, point1, 10, finImg);

//    (*env)->SetStaticIntField(env, cls, fid, 4);
//    loading_progress=4;
    LOGD("Point 3");
    segmentForeground(img1, finImg, background);
//    loading_progress=5;
//    (*env)->SetStaticIntField(env, cls, fid, 5);

    getMaskedGrayImage(img1, background);
    getMaskedImage(img1, finImg);

//    loading_progress=6;
//    (*env)->SetStaticIntField(env, cls, fid, 6);

    LOGD("Point 4");

    imwrite("/mnt/sdcard/SimpleImageCapture/img_fg.jpg", finImg);
    imwrite("/mnt/sdcard/SimpleImageCapture/img_bg.jpg", background);
    //getMaskedImage(img1, foreground);
    addFgBg(finImg, background, finImg);
    imwrite("/mnt/sdcard/SimpleImageCapture/img_fin.jpg", finImg);
    //cvtColor(finImg, finImg, CV_BGR2RGBA);
    return;
}
}


int getDisp(Mat g1, Mat g2, Mat &disp)
{
    Mat disp16;
    StereoSGBM sbm;
    sbm.SADWindowSize = 7; // 5
    sbm.numberOfDisparities = 192;
    sbm.preFilterCap = 4;
    sbm.minDisparity = -32; // -64
    sbm.uniquenessRatio = 9; // 1
    sbm.speckleWindowSize = 180; //150
    sbm.speckleRange = 2;
    sbm.disp12MaxDiff = 20; // 10
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
    GaussianBlur(img, blur, Size(9, 9), 11, 11);
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
