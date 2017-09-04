#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "inc.h"

using namespace std;

void adaboostTrain(const feature fea, classfier *clf, int numOfWeakClassfier)
{
	int numOfData = fea.numOfData;
	int i;
	int *tmp;
	tmp = (int *)malloc(2 * sizeof(int));
	float *weight = (float *)malloc(numOfData * sizeof(float));

	for(i = 0; i < numOfData; i++)
		*(weight + i) = 1.0f / numOfData;

	for(i = 0; i < numOfWeakClassfier; i++)
	{
		weakClassfier(fea, weight, *(clf + i));
	}
	free(weight);
	weight = NULL;
}

void weakClassfier(const feature fea, float *weight, classfier &clf)
{
	int numOfData = fea.numOfData;
	int numOfFeature = fea.numOfFeature;
	int *featureData = fea.featureData;
	int *label = fea.label;
	float &alpha = clf.alpha;
	int &featureSel = clf.featureSel;
	int &thresh = clf.thresh;
	int max, min;
	int col, row;
	int i, j;
	float error = 0;
	float errorMin = 10000.0f;
	float weightSum = 0.0;
	int *labelTmp;

	labelTmp = (int *)malloc(numOfData * sizeof(int));
	for(col = 0; col < numOfFeature; col++)
	{
		max = *(featureData + col);
		min = *(featureData + col);
		for(row = 0; row < numOfData; row++)//find max, min value of every column
		{
			int current = *(featureData + row * numOfFeature + col);
			if(current > max)
				max = current;
			if(current < min)
				min = current;
		}
		int numOfSplit = numOfData;//can be smaller to save computing time
		int stepSize = (max - min) / numOfSplit;
		for(i = 0; i < numOfSplit; i++)
		{
			int threshTmp = min + i * stepSize + stepSize / 2;//different value for classfication
			error = 0;
			for(j = 0; j < numOfData; j++)
			{
				labelTmp[j] = 0;
				if((*(featureData + j * numOfFeature + col) <= threshTmp) &&
				   (*(label + j) == 1) || 
				   (*(featureData + j * numOfFeature + col) > threshTmp) &&
				   (*(label + j) == -1) )
				{
					error += (*(weight + j));
					labelTmp[j] = 1;
				}
			}
			if(error < errorMin && error < 0.5)
			{
				errorMin = error;
				thresh = threshTmp;
				featureSel = col;
			}
			if(errorMin < 1e-3)//error is small enough. Break current feature
				break;
		}
		if(errorMin < 1e-4)//error is small enough. Break all features
			break;
	}
	alpha = 0.5f * log((1 - errorMin) / errorMin);
	for(i = 0; i < numOfData; i++)
		weightSum += *(weight + i);
	for(i = 0; i < numOfData; i++)
	{
		if(labelTmp[i] == 1)//error
			*(weight + i) /= weightSum * exp(alpha);
		else
			*(weight + i) /= weightSum * exp(-alpha);
	}
	free(labelTmp);
}

int ababoostDetect(int *feaData, classfier *clf, int numOfWeakClassfier)
{
	classfier clfTmp;
	float alpha;
	int featureSel;
	int thresh;
	float pred;
	for(int i = 0; i < numOfWeakClassfier; i++)
	{
		pred = 0;
		clfTmp = *(clf+ i);
		alpha = clfTmp.alpha;
		featureSel = clfTmp.featureSel;
		thresh = clfTmp.thresh;
		if(*(feaData + featureSel) <= thresh)//prediction result is -1
			pred -= alpha;
		else//prediction result is 1
			pred += alpha;
	}
	return (pred > 0 ? 1 : -1);
}
