#include <bits/stdc++.h> 
using namespace std;

const double eps = 1e-5;
const double threshold = 30.0;
const int max_segments = 10;
const double tolerance = 0.001;

struct RGB {
	double R, G, B;
	void operator+=(const RGB& x) {
		R+=x.R;
		G+=x.G;
		B+=x.B;
	}
	void operator/=(int y) {
		if(y!=0) {
			R/=y;
			G/=y;
			B/=y;
		}
	} 
};

struct XYZ {
	double X, Y, Z;
};

struct LAB {
	double L, A, B;
};

struct Image {
	RGB* rgb;
	char temp[3];
	int width, height, maxlevel;
};

void getImage(Image* image, string file);

void convertRGB2XYZ(const RGB& src, XYZ& dest);

void convertXYZ2LAB(const XYZ& src, LAB& dest);

double dist(const LAB& C1, const LAB& C2);

double dist(const RGB& col1, const RGB& col2);

