typedef unsigned char uchar;
typedef struct classfier
{
	float alpha;
	int featureSel;
	int thresh;
} classfier;

typedef struct feature
{
	int *featureData;
	int *label;
	int numOfFeature;
	int numOfData;
} feature;

int getHaarValue(const int sat[][24], int x, int y, int w, int h, int type);
int getHaarCount(int W, int H, int w, int h);
int test(int W, int H);
void adaboostTrain(const feature fea, classfier *clf, int numOfWeakClassfier);
void weakClassfier(const feature fea, float *weight, classfier &clf);
int ababoostDetect(int *fea, classfier *clf, int numOfWeakClassfier);