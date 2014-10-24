#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"

using namespace cv;
Mat CalculerContour(Mat& mat1);
int main ( int argc,char **argv ) {
	/*Mat mat1 = imread("image.jpg", 1);
	char d[12] = {255,0,0,0,255,0,0,0,255,0,0,0};
	Mat mat2(2,2,CV_8UC3,d);
	Mat res = CalculerContour(mat1);
	imwrite("img.jpg",res);*/

	Mat img = imread("pic_init.jpg",0);
	Mat res;
	int i = 0;
	if(!img.empty())
		threshold(img,res,0,255,THRESH_BINARY_INV+THRESH_OTSU); // Seuil via algo OTSU : convertit une image en une image binaire ("vrai" noir et blanc)
	else
		std::cout << "img is empty" << '\n';
	imwrite("threshold_im.jpg",res);
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;
	if(!res.empty())
//		findContours(res,contours,hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Recherche des contours des composantes connexes de l'image avec hiérarchie (donc plus long)
		findContours(res,contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE); // Recherche des contours des composantes connexes de l'image
	else
		std::cout << "res is empty" << '\n';
/*	for(i=0;i>=0;i = hierarchy[i][0]) //Remplissage des contours avec hiérarchie (donc plus long)
	{
		Scalar color (rand()%255, rand()%255, rand()%255);
		drawContours(res,contours,i,color,CV_FILLED, 8, hierarchy);

	}*/
	for(i=0; i<contours.size(); i++) // Remplissage des contours
	{
//		Rect r = boundingRect(contours[i]); // Rectangle englobant une composante connexe (fonctionnement bizarre)
		Scalar color (rand()%255, rand()%255, rand()%255);
		drawContours(res,contours,i,color,CV_FILLED, 8);

	}
	imwrite("connected_comp.jpg", res);
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
			if ((b < 90) && (r < 90) && (g < 90))
			{
			    x_min = ((x_min > j/3)? j/3 : x_min) ;
			    x_max = ((x_max < j/3)? j/3 : x_max) ;
			    y_min = ((y_min > i)? i : y_min) ;
			    y_max = ((y_max < i)? i : y_max) ;
			}
	   	}
	}
//	std::cout << mat1 << '\n';
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
