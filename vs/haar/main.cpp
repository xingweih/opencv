#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

typedef unsigned char uchar;

int main(int arvc, char **argv)
{
	const int width = 24;
	const int height = 24;
	Mat image = imread("../../data/lena.jpg");
	//imshow("xxx", image);

	int x, y, w, h;
	int sat[24][24] = { {0} };

	ofstream fout;
	fout.open("data1.txt", ios::trunc);
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			fout << setw(6) << (int)image.at<uchar>(Point(x, y)) << " ";
		}
		fout << endl;
	}
	fout.close();

	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
		{
			uchar pixel = image.at<uchar>(Point(x, y));
			if ((x == 0) && (y == 0))
				sat[y][x] = pixel;
			else if (x == 0)
				sat[y][x] = sat[y-1][x] + pixel;
			else if(y == 0)
				sat[y][x] = sat[y][x-1] + pixel;
			else
				sat[y][x] = sat[y][x-1] + sat[y-1][x] - sat[y-1][x-1] + pixel;
		}

	for (h = 2; h < height - 1; h++)
		for (w = 2; w < width - 1; w++)
			for (y = 0; y < height - h; y++)
				for (x = 0; x < width - w; x++)
				{
					int tmp = sat[y][x] + sat[y + h][x + w] - sat[y + h][x] - sat[y][x + w];
					int tmp2 = tmp ;
					//recSat[y][x] = sat[y][x-1] + sat[y-1][x] - sat[y-1][x-1] + pixel;
				}

	fout.open("data.txt", ios::trunc);
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			fout << setw(6) << sat[y][x] << " ";
		}
		fout << endl;
	}
	fout.close();

	waitKey();
}

