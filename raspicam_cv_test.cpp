#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;
//Mat CalculerContour(Mat& mat1);
int main ( int argc,char **argv ) {
	/*Mat mat1 = imread("image.jpg", 1);
	char d[12] = {255,0,0,0,255,0,0,0,255,0,0,0};
	Mat mat2(2,2,CV_8UC3,d);
	Mat res = CalculerContour(mat1);
	imwrite("img.jpg",res);*/

	Mat img = imread("pic_init.jpg",0);
	Mat bin;
	Mat res;
	int i = 0;
	int j = 0;
	int k = 0;
	double col;
	int size_cont;
	int size_1;
	int size_2;
	int size_tot;
	int medv2;
	int debv1;
	int endv1;
	int b = 0;
	RNG rng(12345);
	if(!img.empty())
		threshold(img,bin,0,255,THRESH_BINARY_INV+THRESH_OTSU); // Seuil via algo OTSU : convertit une image en une image binaire ("vrai" noir et blanc)
	else
		std::cout << "img is empty" << '\n';
	imwrite("threshold_im.jpg",bin);
	std::vector<std::vector<Point> > contours;
	std::vector<Vec4i> hierarchy;
	std::vector<Point> cont_eq;
	std::vector<Point> cont_test;
	std::vector<Point> cont_eq_1;
	std::vector<Point> cont_eq_2;
	std::vector<int> vect_x_1;
	std::vector<int> vect_x_2;
	if(!bin.empty())
//		findContours(res,contours,hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Recherche des contours des composantes connexes de l'image avec hiérarchie (donc plus long)
		findContours(bin,contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1); // Recherche des contours des composantes connexes de l'image
	else
		std::cout << "bin is empty" << '\n';
/*	for(i=0;i>=0;i = hierarchy[i][0]) //Remplissage des contours avec hiérarchie (donc plus long)
	{
		Scalar color (rand()%255, rand()%255, rand()%255);
		drawContours(res,contours,i,color,CV_FILLED, 8, hierarchy);

	}*/
	res = Mat::zeros(bin.size(), CV_8UC3);
	std::vector<std::vector<Point> > contours_filt;
	for(i=0; i<contours.size(); i++)
	{
		if (contours[i].size() > 100)
		{
			contours_filt.push_back(contours[i]);
		}
	}
	size_cont = contours_filt.size();
	for(i=0; i<size_cont; i++)
	{
	if(b == 0)
	{
		for(j=0; j<contours_filt[i].size(); j++)
		{
			vect_x_1.push_back(contours_filt[i][j].x);
			std::sort(vect_x_1.begin(),vect_x_1.end());
		}
		for(k=i+1;k<size_cont;k++)
		{
		if(b==0)
		{
			for(j=0; j<contours_filt[k].size(); j++)
			{
				vect_x_2.push_back(contours_filt[k][j].x);
				std::sort(vect_x_2.begin(),vect_x_2.end());
			}
			medv2 = vect_x_2[vect_x_2.size()/2];
			debv1 = vect_x_1[0];
			endv1 = vect_x_1[vect_x_1.size()-1];

				std::cout << "i : " << i << '\n';
				std::cout << "k : " << k << '\n';
				std::cout << "medv2 : " << medv2 << '\n';
				std::cout << "debv1 : " << debv1 << '\n';
				std::cout << "endv1 : " << endv1 << '\n';
			if(medv2 > debv1 && medv2 < endv1)
			{
				contours_filt[k].swap(contours_filt[size_cont-2]);
				contours_filt[i].swap(contours_filt[size_cont-1]);
				b = 1;
			}
		}
		}
	}
	}
	cont_eq_1 = contours_filt[size_cont-2];
	cont_eq_2 = contours_filt[size_cont-1];
	size_1 = cont_eq_1.size();
	size_2 = cont_eq_2.size();
	size_tot = size_1+size_2;
	for(i=0;i<size_1;i++)
	{
		cont_eq.push_back(cont_eq_1.back());
		cont_eq_1.pop_back();
	}
	for(i=0;i<size_2;i++)
	{
		cont_eq.push_back(cont_eq_2.back());
		cont_eq_2.pop_back();
	}
	contours_filt.pop_back();
	contours_filt.pop_back();
	contours_filt.push_back(cont_eq);
	std::cout << contours_filt.size() << '\n';
	for(i=0; i<contours_filt.size(); i++) // Remplissage des contours
	{
		//Rect r = boundingRect(contours[i]); // Rectangle englobant une composante connexe (fonctionnement bizarre)
		Scalar color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255));
		//std::cout << contours[i] << '\n';
		//std::cout << i << '\n';
		//col = rand()&255;
		std::cout << "couleur : " << color[0]<< " " << color[1]<< " " << color[2]<< '\n';
		drawContours(res,contours_filt,i,color,CV_FILLED, 8);
	}

//	cvtColor(img,res,CV_BGR2GRAY);
	/*Canny(img,res,100,200,3);
	imwrite("threshold_im.jpg",res);
	RNG rng(12345);
	findContours(res,contours,hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0,0)); // Recherche des contours des composantes connexes de l'image
	Mat drawing = Mat::zeros(res.size(), CV_8UC3);
	for(i=0; i<contours.size(); i++) // Remplissage des contours
	{
		Scalar color = Scalar(rand()&255, rand()&255, rand()&255);
		drawContours(drawing,contours,i,color,2,8,hierarchy,0,Point() );
	}
*/

	imwrite("connected_comp.jpg", res);
}
/*
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
*/
