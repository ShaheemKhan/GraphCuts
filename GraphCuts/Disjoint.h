//-------------------------------------------------------------//
//
// * Graph Cuts Segmentation with OpenCV
//   In "Efficient Graph-Based Image Segmentation" 
//   by Pedro Felzenszwalb & Daniel P. Huttenlocher (2004)
//
//-------------------------------------------------------------//

/* Disjoint.h: Graph nodes segmentation class */

typedef struct {
	int rank;
	int p;
	int size;
}disjElem;

class disJoint{
public:
	disJoint(int elements);
	~disJoint();
	int find(int x);
	void join(int x, int y);
	int size(int x) const { return elts[x].size; }
	int num_sets() const { return num; }

private:
	disjElem *elts;
	int num;
};