#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
Mat CalculerContour(Mat& mat1);
int main ( int argc,char **argv ) {
	Mat mat1 = imread("im.jpg", 1);
	char d[12] = {255,0,0,0,255,0,0,0,255,0,0,0};
	Mat mat2(2,2,CV_8UC3,d);
	Mat res = CalculerContour(mat1);
	imwrite("img.jpg",res);
}
Mat CalculerContour(Mat& mat1)
{
	unsigned char * input = (unsigned char *)(mat1.data);
	int x_min = 9999;
	int x_max = -1;
	int y_min = 9999;
	int y_max = -1;
	int b,g,r;
	for (int i = 0 ; i < mat1.rows ; i++) { //960
   		for (int j = 0 ; j < 3*mat1.cols ; j = j + 3) { //1280
			b = (int)input[3*mat1.cols*i+j];
			g = (int)input[3*mat1.cols*i+j+1];
			r = (int)input[3*mat1.cols*i+j+2];
			if ((b < 60) && (r < 60) && (g < 60))
			{
			    x_min = ((x_min > j/3)? j/3 : x_min) ;
			    x_max = ((x_max < j/3)? j/3 : x_max) ;
			    y_min = ((y_min > i)? i : y_min) ;
			    y_max = ((y_max < i)? i : y_max) ;
			}
	   	}
	}
	std::cout << mat1 << '\n';
	std::cout << "x_min :" << x_min << " x_max :" << x_max << " y_min : " << y_min << " y_max : " << y_max << '\n';
	for (int i = y_min ; i < y_max ; i++) { // 960
		for (int j = x_min ; j < 3*x_max ; j = j + 3) { // 1280
			input[3*mat1.cols*i+j] = 0;
			input[3*mat1.cols*i+j+1] = 0;
			input[3*mat1.cols*i+j+2] = 255;
   		}
	}
	Mat ret(Size(1280,960),CV_8UC3, input);
	return (ret);
}
