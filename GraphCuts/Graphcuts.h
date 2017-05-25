//-------------------------------------------------------------//
//
// * Graph Cuts Segmentation with OpenCV
//   In "Efficient Graph-Based Image Segmentation" 
//   by Pedro Felzenszwalb & Daniel P. Huttenlocher (2004)
//
//-------------------------------------------------------------//

/* Graphcuts.h: Graph process and segmentation */
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <vector>
#include <opencv2/opencv.hpp>
#include "Disjoint.h"

using namespace cv;

#define THRESHOLD(size, smallThr) (smallThr/size)

template <class T>
inline T square(const T &x) { return x*x; };

typedef unsigned char uchar;

typedef struct {
	double w;
	int a, b;
} edge;

bool operator<(const edge &a, const edge &b);



int graphCuts(Mat& src, Mat& dst, double sigma, double c, int smallThr);
static inline double dissim(Mat &red, Mat &green, Mat &blue,
							int x1, int y1, int x2, int y2);
disJoint *segGraph(int num_vertices, int num_edges, edge* edges, double c);