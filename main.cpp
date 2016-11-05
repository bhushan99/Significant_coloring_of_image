#include "image.h"

set<int> choose_centroids(LAB* lab, RGB* rgb, int num_pixels) {
	set<int> centroids;
	int curr = rand()%num_pixels;
	centroids.insert(curr);
	double *min_dist = new double[num_pixels];
	for(int i=0;i<num_pixels;i++) {
		min_dist[i] = dist(rgb[i], rgb[curr]);
	}
	min_dist[curr] = -1;
	cout<<"Centroids :\n";
	while(1) {
		double mx_dist = -1; 
		int mx_ind = -1;
		for(int i=0;i<num_pixels;i++) {
			if(min_dist[i] < 0) continue;
			if(mx_dist < 0 || ( abs(mx_dist - min_dist[i]) > eps && mx_dist < min_dist[i] )) {
				mx_dist = min_dist[i];
				mx_ind = i;
			}
		}
		centroids.insert(mx_ind);
		if(mx_ind == -1 || mx_dist < threshold || centroids.size()==max_segments) break;
		cout<< "colour : ("<<rgb[mx_ind].R<<","<<rgb[mx_ind].G<<","<<rgb[mx_ind].B<<")\t";
		cout<<"persistence : "<<mx_dist<<endl;
		for(int i=0;i<num_pixels;i++) {
			if(min_dist[i] < 0) continue; 
			min_dist[i] = min(min_dist[i], dist( rgb[mx_ind], rgb[i] ) );
		}
		min_dist[mx_ind] = -1;
	}
	cout<<endl;
	delete [] min_dist;
	return centroids;
}

pair<RGB *, int *> K_means(const set<int>& centroids, RGB* rgb, LAB* lab, int num_pixels) {
	int num_centroids = centroids.size();
	int *pix_count = new int[num_centroids];
	RGB* centroids_rgb = new RGB[num_centroids];
	RGB* centroids_lab = new RGB[num_centroids];
	int k=0;
	for(set<int>::iterator it = centroids.begin(); it!=centroids.end(); it++) {
		centroids_rgb[k] = rgb[*it];
		centroids_lab[k++] = rgb[*it];
	}

	int* assignment = new int[num_pixels];
	for(int i=0;i<num_pixels;i++) {
		assignment[i] = -1;
	}
	bool repeated = false;
	int iter = 0;
	cout<<"K-means : \n";
	while(!repeated) {
		iter++;
		int changes=0;
		repeated = true;
		for(int i=0; i<num_centroids; i++) 
			pix_count[i]=centroids_rgb[i].R=centroids_rgb[i].G=centroids_rgb[i].B=0.0;
		for(int pix = 0; pix<num_pixels; pix++) {
			int prev = assignment[pix];
			double min_dist = dist(rgb[pix], centroids_lab[0]);
			int min_ind = 0;
			for(int i=1; i<num_centroids;i++) {
				double curr_dist = dist(rgb[pix], centroids_lab[i]);
				if(abs(curr_dist-min_dist) > eps && curr_dist < min_dist) {
					min_dist = curr_dist;
					min_ind = i;
				}
			}
			if(min_ind != prev) repeated = false, changes++;
			assignment[pix] = min_ind;
			pix_count[min_ind]++;
			centroids_rgb[min_ind]+=rgb[pix];
		} break;
		for(int i=0; i < num_centroids; i++) {
			centroids_rgb[i] /= pix_count[i];
			// XYZ temp;
			centroids_lab[i] = centroids_rgb[i];
			// convertRGB2XYZ(centroids_rgb[i], temp);
			// convertXYZ2LAB(temp, centroids_lab[i]);
		}
		cout<<"iteration "<<iter<<" :\tassignment of "<<changes<<" pixels changed"<<endl;
		if(double(changes)/num_pixels < tolerance) break;
	}
	delete [] centroids_rgb;
	delete [] pix_count;
	return make_pair(centroids_lab, assignment);
}

inline int rounded(double d) {
	if(d-int(d) < 0.5 || int(d) == 255) return int(d);
	return int(d) + 1;
}

int main(int argc, char const *argv[])
{
	if(argc < 2) {
		cerr << "Please specify input file\n";
		return -1;
	}
	Image image;
	getImage(&image, argv[1]);

	int num_pixels = image.width * image.height;
	LAB* lab;
	// for(int i=0; i<num_pixels; i++) {
	// 	XYZ temp;
	// 	convertRGB2XYZ(image.rgb[i], temp);
	// 	convertXYZ2LAB(temp, lab[i]);
	// }

	set<int> centroids = choose_centroids(lab, image.rgb, num_pixels);

	pair<RGB *, int *> centroids_rgb_and_assignment = K_means(centroids, image.rgb, lab, num_pixels);
	RGB* centroids_rgb = centroids_rgb_and_assignment.first;
	int* assignment = centroids_rgb_and_assignment.second;
	
	string output_file;
	if(argc > 2) output_file = argv[2];
	else output_file = "output.ppm";

	FILE* fp_out = fopen(output_file.c_str(), "w");
	fprintf(fp_out, "%s\n%d %d\n%d\n",image.temp, image.width, image.height, image.maxlevel);
	for(int i=0; i < num_pixels; i++) {
		RGB temp = centroids_rgb[assignment[i]];
		fprintf(fp_out, "%d %d %d\n", rounded(temp.R), rounded(temp.G), rounded(temp.B));
	}
	fclose(fp_out);
	cout << "\nOutput file is " << output_file <<endl;

	delete [] assignment;
	delete [] centroids_rgb;
	delete [] image.rgb;
	delete [] lab;
	return 0;
}