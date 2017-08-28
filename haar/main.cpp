#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

typedef unsigned char uchar;

int main(int arvc, char **argv)
{
	Mat image = imread("../data/lena.jpg");
	int cols = image.cols;
	int rows = image.rows;
	int x, y;
	Mat s = Mat::zeros(image.size(), CV_32S);
	Mat ii = Mat::zeros(image.size(), CV_32S);
	for (y = 0; y < rows; y++)
	{ 
		uchar *ptrImage = image.ptr<uchar>(y);
		int *ptrSUpper = nullptr;
		if(y != 0)
			ptrSUpper = s.ptr<int>(y-1);
		int *ptrS = s.ptr<int>(y);
		for (x = 0; x < cols; x++)
		{
			if (y == 0)
				ptrS[x] = ptrImage[x];
			else
				ptrS[x] = ptrSUpper[x] + ptrImage[x];
		}
	}
	for (y = 0; y < rows; y++)
	{ 
		int *ptrS = s.ptr<int>(y);
		int *ptrI = ii.ptr<int>(y);
		for (x = 0; x < cols; x++)
		{
			if (x == 0)
				ptrI[x] = ptrS[x];
			else
				ptrI[x] = ptrI[x-1] + ptrS[x];
		}
	}
	const string cascadeFileName = "D:/opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml";
	CascadeClassifier casecade;
	if (!casecade.load(cascadeFileName))
	{
		cout << "casecase init failed" << endl;
	}
	Mat imageGray;
	cvtColor(image, imageGray, COLOR_BGR2GRAY);
	equalizeHist(imageGray, imageGray);
	vector<Rect> faces, faces2;
	casecade.detectMultiScale(imageGray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	flip(imageGray, imageGray, 1);
	casecade.detectMultiScale(imageGray, faces2, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	cout << "人脸个数:" << faces.end() - faces.begin();
	int n = 0;
	for (vector<Rect>::const_iterator i = faces.begin(); i <faces.end(); i++, n++)
	{
		Point center;
		int radius;
		center.x = cvRound((i->x + i->width * 0.5));
		center.y = cvRound((i->y + i->height * 0.5));
		radius = cvRound((i->width + i->height) * 0.25);
		circle(image, center, radius, 2);
	}
	namedWindow("人脸识别");
	imshow("人脸识别", image);


	waitKey();
}
