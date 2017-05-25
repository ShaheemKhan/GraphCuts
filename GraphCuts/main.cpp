//-------------------------------------------------------------//
//
// * Graph Cuts Segmentation with OpenCV
//   In "Efficient Graph-Based Image Segmentation" 
//   by Pedro Felzenszwalb & Daniel P. Huttenlocher (2004)
//
//-------------------------------------------------------------//

#pragma once
#include "Graphcuts.h"


int main(void){


	// for blurring, threshold
	// default values are: sigma=0.8, k=100, smallThr=100
	// you can change values for desired results

	double sigma = 0.8;
	double k = 100;

	// small components removing
	int smallThr = 100;
	int segs = 0;
	
	Mat src = imread(".\GraphCuts\img.jpg");
	Mat dst(src.rows, src.cols, CV_8UC3);
	
	segs = graphCuts(src, dst, sigma, k, smallThr);

	printf("Segmented to %d parts...\n", segs);

	imshow("Input Image", src);
	imshow("Segmented Image", dst);
	imwrite(".\GraphCuts\result.jpg", dst);
	waitKey(0);
	
	return 0;
}