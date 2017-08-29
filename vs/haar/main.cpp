#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <iostream>
#include <fstream>

using namespace cv;
using namespace std;

int getHaarValue(const int sat[][24], int x, int y, int w, int h, int type);
typedef unsigned char uchar;

int main(int arvc, char **argv)
{
	const int width = 24;
	const int height = 24;
	Mat image = imread("../../data/lena.jpg");
	//imshow("xxx", image);

	int x, y, w, h;
	int sat[24][24] = { {0} };
	int rsat[24][24] = { {0} };

	ofstream fout;
	fout.open("data_original.txt", ios::trunc);
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			fout << setw(6) << (int)image.at<uchar>(Point(x, y)) << " ";
		}
		fout << endl;
	}
	fout.close();
	//compute sat
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
	//rotation = 0
	for (h = 2; h <= height; h++)
		for (w = 2; w <= width; w++)
			for (y = 0; y <= height - h; y++)
				for (x = 0; x <= width - w; x++)
				{
					int tmp = getHaarValue(sat, x, y, w, h, 0);
					int tmp2 = tmp ;
				}
	//compute rsat
	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
		{
			uchar pixel = image.at<uchar>(Point(x, y));
			if(x == 0)
				rsat[y][x] = pixel;
			else if (y == 0)
				rsat[y][x] = rsat[y][x-1] + pixel;
			else if(x == 1)
				rsat[y][x] = rsat[y-1][x-1] + rsat[y][x-1] + pixel;
			else
				rsat[y][x] = rsat[y-1][x-1] + rsat[y][x-1] - rsat[y-1][x-2] + pixel;
		}

	for (y = height-2; y >= 0; y--)
		for (x = width-1; x >= 0; x--)
		{
			if (x == 0)
				rsat[y][x] = rsat[y][x];
			else if (x == 1)
				rsat[y][x] += rsat[y + 1][x - 1];
			else
				rsat[y][x] += (rsat[y + 1][x - 1] - rsat[y][x-2]);
		}

	fout.open("data_sat.txt", ios::trunc);
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			fout << setw(6) << sat[y][x] << " ";
		}
		fout << endl;
	}
	fout.close();

	fout.open("data_rsat.txt", ios::trunc);
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			fout << setw(6) << rsat[y][x] << " ";
		}
		fout << endl;
	}
	fout.close();

	waitKey();
}

int getHaarValue(const int sat[][24], int x, int y, int w, int h, int type)
{
	int res = 0;
	if (type == 0)//0 degree
		//res = sat[y - 1][x - 1] + sat[y + h - 1][x + w - 1] - sat[y + h - 1][x - 1] - sat[y - 1][x + w - 1];
	{
		if (x == 0 && y == 0)
			res = sat[y + h - 1][x + w - 1];
		else if (x == 0)
			res = sat[y + h - 1][x + w - 1] - sat[y - 1][x + w - 1];
		else if (y == 0)
			res = sat[y + h - 1][x + w - 1] - sat[y + h - 1][x - 1];
		else
			res = sat[y - 1][x - 1] + sat[y + h - 1][x + w - 1] - sat[y + h - 1][x - 1] - sat[y - 1][x + w - 1];

	}
	else if(type == 1)//45 degree
		res = sat[y][x] + sat[y + h][x + w] - sat[y + h][x] - sat[y][x + w];
	return res;
}