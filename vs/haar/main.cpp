#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "inc.h"

using namespace cv;
using namespace std;

int main(int arvc, char **argv)
{
	const int width = 24;
	const int height = 24;
	int x, y, w, h;
	int count = 0;
	int sat[24][24] = { {0} };
	int rsat[24][24] = { {0} };
	//test adaboost
	int i;
	feature fea;
	classfier *clf;
	int numOfWeakClassfier = 10;
	int * &featureData = fea.featureData; 
	int * &label = fea.label;

	featureData = (int *)malloc(width * height * sizeof(int));
	label = (int *)malloc(height * sizeof(int));
	clf = (classfier *)malloc(numOfWeakClassfier * sizeof(classfier));

	for(i = 0; i < width * height; i++)
		*(featureData + i) = rand() % 100;
	for(i = 0; i < width * height; i++)
	{
		if(rand() % 2 == 1)
			*(label + i) = 1;
		else
			*(label + i) = -1;
	}
	fea.numOfData = height;
	fea.numOfFeature = width;

	adaboostTrain(fea, clf, numOfWeakClassfier);

	free(featureData);
	free(label);
	featureData = NULL;
	label = NULL;


	Mat image = imread("lena.jpg");
	//imshow("xxx", image);

	cout << test(24, 24) << endl;

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
	count = 0;
	vector<int> feature;
	for(x = 0; x < width; x++)
		for(y = 0; y < height; y++)
			for(w = 2; w <= width; w++)
				for(h = 2; h <= height; h++)
				{
					//1a
					if((x + 2 * w <= width) && (y + h <= height))
					{
						int left = getHaarValue(sat, x, y, w, h, 0);
						int right = getHaarValue(sat, x + w, y, w, h, 0);
						int res = left - right;
						feature.push_back(res);
					}
					//1b
					if((x + w <= width) && (y + 2 * h <= height))
					{
						int up = getHaarValue(sat, x, y, w, h, 0);
						int down = getHaarValue(sat, x, y + h, w, h, 0);
						int res = up - down;
						feature.push_back(res);
					}
					//2a
					if((x + 3 * w <= width) && (y + h <= height))
					{
						int left = getHaarValue(sat, x, y, w, h, 0);
						int mid = getHaarValue(sat, x + w, y, w, h, 0);
						int right = getHaarValue(sat, x + 2 * w, y, w, h, 0);
						int res = left - 2 * mid + right;
						feature.push_back(res);
					}
					//2c
					if((x + w <= width) && (y + 3 * h <= height))
					{
						int up = getHaarValue(sat, x, y, w, h, 0);
						int mid = getHaarValue(sat, x, y + h, w, h, 0);
						int down = getHaarValue(sat, x, y + 2 * h, w, h, 0);
						int res = up - 2 * mid + down;
						feature.push_back(res);
					}
				}
				cout << "feature number: " << feature.size() << endl;
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
	//rotate = 45
	for(x = 0; x < width; x++)
		for(y = 0; y < height; y++)
			for(w = 1; w <= width / 2; w++)
				for(h = 1; h <= height; h++)
					if((x - h >= 0) && (x + 2 * w + 1 < width) && (y + w + h + w + 1 < height))
					{
						int left = getHaarValue(rsat, x, y, w, h, 1);
						int right = getHaarValue(rsat, x + w, y + w, w, h, 1);
						int res = left - right;
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
		res = sat[y + w][x + w] + sat[y + h][x -h] - sat[y][x] - sat[y + w + h][x + w - h];
	return res;
}

int getHaarCount(int W, int H, int w, int h)
{
	int X = W / w;
	int Y = H / h;
	int i, j, k, l;
	int count = 0;
	for(i = 0; i < X; i++)
		for(j = 0; j < Y; j++)
			for(k = 0; k < W + 1 - w * (X / 2); k++)
				for(l = 0; l < H + 1 - h * (Y / 2); l++)
					count++;
	return count;
}

int test(int W, int H) 
{
	int w, h, x, y;
	int count = 0;
	/*
	for(x = 0; x < W; x++)
		for(y = 0; y < H; y++)
			for(w = 2; w <= W; w++)
				for(h = 2; h <= H; h++)
					if((x - 2 * h >= 0) && (x + w <= W) && (y + 2 * h + w <= H))
						count++;
						*/
	//1a, 1b
	///*
	for(x = 0; x < W; x++)
		for(y = 0; y < H; y++)
			for(w = 2; w <= W; w++)
				for(h = 2; h <= H; h++)
					if((x + 2 * w <= W) && (y + h <= H))
						count++;
						//*/
	//1c, 1d
	/*
	for(x = 0; x < W; x++)
		for(y = 0; y < H; y++)
			for(w = 1; w <= W; w++)
				for(h = 1; h <= H; h++)
					if((x - h >= 0) && (x + 2 * w + 1 < W) && (y + w + h + w + 1 < H))
						count++;
						*/
	//2a, 2c
	/*
	for(x = 0; x < W; x++)
		for(y = 0; y < H; y++)
			for(w = 1; w <= W; w++)
				for(h = 1; h <= H; h++)
					if((x + 3 * w <= W) && (y + h <= H))
						count++;
						*/
	//2b, 2d
	/*
	for(x = 0; x < W; x++)
		for(y = 0; y < H; y++)
			for(w = 1; w <= W; w++)
				for(h = 1; h <= H; h++)
					if((x + 4 * w <= W) && (y + h <= H))
						count++;
						*/
	//2e, 2g
	/*
	for(x = 0; x < W; x++)
		for(y = 0; y < H; y++)
			for(w = 2; w <= W; w++)
				for(h = 2; h <= H; h++)
					if((x - 3 * h + 1 >= 0) && (x + w - 1 < W) && (y + 3 * h + w - 2 < H))
						count++;
						*/
	//2f, 2h
	/*
	for(x = 0; x < W; x++)
		for(y = 0; y < H; y++)
			for(w = 2; w <= W; w++)
				for(h = 2; h <= H; h++)
					if((x - 4 * h + 1 >= 0) && (x + w - 1 < W) && (y + 4 * h + w - 2 < H))
					//if((x - 4 * h >= 0) && (x + w <= W) && (y + 4 * h + w <= H))
						count++;
						*/
	return count;
}