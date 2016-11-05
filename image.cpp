#include "image.h"

void getImage(Image* image, string file) {
	string cmd = string("convert -compress none ") + file + string(" temp.ppm");
	system(cmd.c_str());
	FILE *fp_in = fopen("temp.ppm","r");
	fscanf(fp_in,"%s",image->temp);
	fscanf(fp_in, "%d%d%d", &(image->width), &(image->height), &(image->maxlevel) );
	int n = image->width * image->height;
	image->rgb = new RGB[n];
	for(int i=0;i<n;i++) {
		fscanf(fp_in, "%lf%lf%lf", &(image->rgb[i].R), &(image->rgb[i].G), &(image->rgb[i].B) );
	}
	fclose(fp_in);
	system("rm temp.ppm");
	return;
}

void convertRGB2XYZ(const RGB& src, XYZ& dest) {
	double r = src.R / 255;
	double g = src.G / 255;
	double b = src.B / 255;
	if ( r > 0.04045 ) r = pow( ( r + 0.055 ) / 1.055 , 2.4);
	else r = r / 12.92;
	if ( g > 0.04045 ) g = pow( ( g + 0.055 ) / 1.055 , 2.4);
	else g = g / 12.92;
	if ( b > 0.04045 ) b = pow( ( b + 0.055 ) / 1.055 , 2.4);
	else b = b / 12.92;
	r = r*100;
	g = g*100;
	b = b*100;
	dest.X = r * 0.4124 + g * 0.3576 + b * 0.1805;
	dest.Y = r * 0.2126 + g * 0.7152 + b * 0.0722;
	dest.Z = r * 0.0193 + g * 0.1192 + b * 0.9505;
}

void convertXYZ2LAB(const XYZ& src, LAB& dest) {
	double x = src.X / 95.047;
	double y = src.Y / 100.000;
	double z = src.Z / 108.883;
	if(x > 0.008856) x = pow(x, 1.0/3.0);
	else x = (7.787 * x) + (16.0 / 116.0); 
	if(y > 0.008856) y = pow(y, 1.0/3.0);
	else y = (7.787 * y) + (16.0 / 116.0); 
	if(z > 0.008856) z = pow(z, 1.0/3.0);
	else z = (7.787 * z) + (16.0 / 116.0); 
	dest.L = 116*y - 16;
	dest.A = 500*(x-y);
	dest.B = 200*(y-z);
}

/*xDL /= WHT-L
xDC /= WHT-C * xSC
xDH /= WHT-H * xSH*/

double dist(const LAB& C1, const LAB& C2) {
	double xC1 = sqrt( ( C1.A * C1.A ) + ( C1.B * C1.B ) );
	double xC2 = sqrt( ( C2.A * C2.A ) + ( C2.B * C2.B ) );
	double xDL = C2.L - C1.L;
	double xDC = xC2 - xC1;
	double xDE = sqrt( ( ( C1.L - C2.L ) * ( C1.L - C2.L ) )
          + ( ( C1.A - C2.A ) * ( C1.A - C2.A ) )
          + ( ( C1.B - C2.B ) * ( C1.B - C2.B ) ) );
	double xDH;
	if ( sqrt( xDE ) > ( sqrt( abs( xDL ) ) + sqrt( abs( xDC ) ) ) ) {
   		xDH = sqrt( ( xDE * xDE ) - ( xDL * xDL ) - ( xDC * xDC ) );
	}
	else {
   		xDH = 0;
	}
	double xSC = 1.0 + ( 0.045 * xC1 );
	double xSH = 1.0 + ( 0.015 * xC1 );
    xDC /= xSC;
    xDH /= xSH;
    return sqrt(xDL*xDL + xDC*xDC + xDH*xDH);
}

double dist(const RGB& col1, const RGB& col2) {
	return abs(col1.R-col2.R) + abs(col1.G-col2.G) + abs(col1.B-col2.B);
}
