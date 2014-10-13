#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

int main ( int argc,char **argv ) {
Mat mat1 = imread("im.jpg", 1);
char d[12] = {255,0,0,0,255,0,0,0,255,0,0,0};
Mat mat2(2,2,CV_8UC3,d);
imwrite("img.jpg",mat2);
std::cout << mat2<< '\n' << '\n';
unsigned char * input = (unsigned char *)(mat2.data);
for (int i = 0;i < mat2.rows; i++) {
   for (int j = 0 ; j < mat2.cols; j++) {
	std::cout << "b = " << (int)input[mat2.step*j+i] << ",";
	std::cout << "g = " << (int)input[mat2.step*j+i+1] << ",";
	std::cout << "r = " << (int)input[mat2.step*j+i+2] << '\n';
   }
}


}
