//-------------------------------------------------------------//
//
// * Graph Cuts Segmentation with OpenCV
//   In "Efficient Graph-Based Image Segmentation" 
//   by Pedro Felzenszwalb & Daniel P. Huttenlocher (2004)
//
//-------------------------------------------------------------//

/* Graphcuts.cpp: Graph process and segmentation */

#include "Graphcuts.h"

// edge compare
bool operator<(const edge &a, const edge &b){
	return a.w < b.w;
}

// compute dissimilarity between two pixels
static inline double dissim(Mat &red, Mat &green, Mat &blue,
				int x1, int y1, int x2, int y2) {
	std::cout << "1:" << blue.at<double>(0, 0) << std::endl;
	std::cout << "2:" << blue.at<double>(0, 1) << std::endl;
	std::cout << "3:" << blue.at<double>(1, 0) << std::endl;
	std::cout << "4:" << blue.at<double>(1, 1) << std::endl;
	return sqrt(square(red.at<double>(y1, x1) - red.at<double>(y2, x2)) +
				square(green.at<double>(y1, x1) - green.at<double>(y2, x2)) +
				square(blue.at<double>(y1, x1) - blue.at<double>(y2, x2)));
}

// graph cuts processing
// c is k value
int graphCuts(Mat& src, Mat& dst, double sigma, double c, int smallThr){

	Mat _src = src.clone();
	_src.convertTo(_src, CV_64FC3);

	int w = _src.cols;
	int h = _src.rows;

	std::vector<Mat> rgb;
	split(_src, rgb);

	// individual channel blurring
	GaussianBlur(rgb[2], rgb[2], Size(5, 5), sigma);
	GaussianBlur(rgb[1], rgb[1], Size(5, 5), sigma);
	GaussianBlur(rgb[0], rgb[0], Size(5, 5), sigma);

	// graph building
	edge *edges = new edge[w*h * 4];
	int num = 0;

	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w;x++)
		{
			if (x < w-1)
			{
				edges[num].a = y * w + x;
				edges[num].b = y*w + (x + 1);
				edges[num].w = dissim(rgb[2], rgb[1], rgb[0], x, y, x + 1, y);
				num++;
			}
			if (y < h - 1) 
			{
				edges[num].a = y * w + x;
				edges[num].b = (y + 1) * w + x;
				edges[num].w = dissim(rgb[2], rgb[1], rgb[0], x, y, x, y + 1);
				num++;
			}
			if ((x < w - 1) && (y < h - 1)) 
			{
				edges[num].a = y * w + x;
				edges[num].b = (y + 1) * w + (x + 1);
				edges[num].w = dissim(rgb[2], rgb[1], rgb[0], x, y, x + 1, y + 1);
				num++;
			}

			if ((x < w - 1) && (y > 0)) {
				edges[num].a = y * w + x;
				edges[num].b = (y - 1) * w + (x + 1);
				edges[num].w = dissim(rgb[2], rgb[1], rgb[0], x, y, x + 1, y - 1);
				num++;
			}
		}
	}
	for (int i = 0; i < 2; i++)
	for (int j = 0; j < 2; j++)
	{
		std::cout << (int)src.at<Vec3b>(i, j)[0] << std::endl;
	}

	// release vectors
	rgb[0].release();
	rgb[1].release();
	rgb[2].release();
	rgb.erase(rgb.begin(), rgb.end());
	
	// segment graphs
	disJoint *d = segGraph(w*h, num, edges, c);

	for (int i = 0; i < w*h; i++)
	{
		std::cout << i << ":" << d->size(i) << std::endl;
	}

	// merging small components 
	for (int i = 0; i < num; i++)
	{
		int a = d->find(edges[i].a);
		int b = d->find(edges[i].b);
		if ((a != b) && ((d->size(a) < smallThr) || (d->size(b) < smallThr)))
		{
			d->join(a, b);
		}
	}

	delete[] edges;
	int segnum = d->num_sets();

	Mat output(h, w, CV_8UC3);

	// random-color palette
	// 3-by-wh 2D array
	uchar **randClr;
	randClr = new uchar*[w*h];
	for (int i = 0; i < w*h; i++)
	{
		randClr[i] = new uchar[3];
		memset(randClr[i], 0, sizeof(uchar)* 3);
	}

	// palette color set
	for (int i = 0; i < w*h; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			randClr[i][j] = (uchar)rand();
		}
	}

	// color assignment to components
	for (int y = 0; y < h; y++)
	{
		for (int x = 0; x < w; x++)
		{
			int comp = d->find(y * w + x);
			for (int k = 0; k < 3;k++)
			{
				output.at<Vec3b>(y, x)[k] = randClr[comp][k];
			}
		}
	}
	dst = output.clone();
	return segnum;
}

disJoint* segGraph(int num_vertices, int num_edges, edge* edges, double c){

	std::sort(edges, edges + num_edges);

	disJoint *d = new disJoint(num_vertices);

	

	// small component thresholding
	double *thresh = new double[num_vertices];
	for (int i = 0; i < num_vertices; i++)
	{
		thresh[i] = 0.00;
	}

	for (int i = 0; i < num_edges; i++)
	{
		edge *pedge = &edges[i];

		int a = d->find(pedge->a);
		int b = d->find(pedge->b);
		if (a!=b)
		{
			if ( (pedge->w <=thresh[a]) &&
				(pedge->w <= thresh[b]) )
			{
				d->join(a, b);
				a = d->find(a);
				thresh[a] = pedge->w + THRESHOLD(d->size(a), c);
			}
		}
		
	}

	delete thresh;
	return d;
}

