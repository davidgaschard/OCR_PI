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
float * detecCavite(Mat& envRect);


			//Mat CalculerContour(Mat& mat1);
int main ( int argc,char **argv ) {
			/*Mat mat1 = imread("image.jpg", 1);
			char d[12] = {255,0,0,0,255,0,0,0,255,0,0,0};
			Mat mat2(2,2,CV_8UC3,d);
			Mat res = CalculerContour(mat1);
			imwrite("img.jpg",res);*/

	//Déclaration des variables

	Mat img = imread("pic_init.jpg",0); // Image initiale
	Mat bin;			    // Image binaire (Noir et blanc après seuil)
	Mat bin2;			    // Clone de bin afin de la sauvegarder avant la recherche de contours
	Mat res;		  	    // Image labelisée
	Mat rect;			    // Image labelisée avec enveloppe rectangulaire
	Mat subImage;			    // Image rectangulaire contenant un unique symbole

	int i = 0;	// Variable de boucle
	int j = 0;	// Variable de boucle
	int k = 0;	// Variable de boucle
	int size_cont;  // Nombre de contours après filtrage (i.e. nombre de symboles en comptant le signe "=" comme deux)
	//Les 5 prochaines variables sont utilisées pour condenser le signe "=" en un seul symbole
	int size_1;	// Nombre de points d'un contour
	int size_2;	// Nombre de points d'un autre contour
	int medv2;	// Abscisse du milieu du deuxième contour
	int debv1;	// Abscisse de début du premier contour
	int endv1;	// Abscisse de début du premier contour
	int b = 0;	// Booléen de sortie de boucle

	double t;

	t = (double)getTickCount();

	string name_img = "env_rect_symb_.jpg"; // Nom (à compléter par la valeur) d'une image contenant un symbole
	string tmp_str;				// Chaine de caractère temporaire pour traitement
	std::ostringstream convert;		// Variable utilisée pour la conversion d'un entier en chaine de caractère (pour insertion dans le nom de l'image afin de numéroter les images contenant les symboles)

	Point pt1; // Point en haut à droite d'un rectangle correspondant à une enveloppe rectangulaire
	Point pt2; // Idem en bas à gauche

	Scalar color; // Couleur aléatoire, permettant de visualiser la labelisation

	std::vector<std::vector<Point> > contours; 	// Contient les points servant à définir les contours des symboles
	std::vector<std::vector<Point> > contours_filt; // Contient les points servant à définir les contours de symboles après filtrage (1 contour = 1 symboles, sauf pour "=")

//	std::vector<Vec4i> hierarchy; 		   // Vecteur permettant la hiérarchisation des contours. Non utilisé.
	// Les 5 vecteurs suivants sont des vecteurs temporaires utilisés pour condenser le signe "=" en un seul symbole
	std::vector<Point> cont_eq;
	std::vector<Point> cont_eq_1;
	std::vector<Point> cont_eq_2;
	std::vector<int> vect_x_1;
	std::vector<int> vect_x_2;


	RNG rng(1); // Tirage aléatoire pour les couleurs
	t = ((double)getTickCount()-t)/getTickFrequency();
	std::cout << "temps init : " << t << '\n';

	t = (double)getTickCount();

	if(!img.empty())
		threshold(img,bin,0,255,THRESH_BINARY_INV+THRESH_OTSU); // Seuil via algo OTSU : convertit une image en une image binaire ("vrai" noir et blanc)
	else
		std::cout << "img is empty" << '\n';
	t = ((double)getTickCount()-t)/getTickFrequency();
	std::cout << "temps OTSU : " << t << '\n';

	t = (double)getTickCount();

	imwrite("threshold_im.jpg",bin);
	bin2 = bin.clone(); // On clone bin car elle va être modifier avec la recherche de contour et on souhaite la conserver pour extraire les enveloppes rectangulaires des symboles et les stocker dans de nouvelles imgaes

	t = ((double)getTickCount()-t)/getTickFrequency();
	std::cout << "temps write + clone : " << t << '\n';

	t = (double)getTickCount();

	if(!bin.empty())
				//findContours(res,contours,hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE); // Recherche des contours des composantes connexes de l'image avec hiérarchie (donc plus long)
		findContours(bin,contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1); // Recherche des contours des composantes connexes de l'image
	else
		std::cout << "bin is empty" << '\n';

				/*for(i=0;i>=0;i = hierarchy[i][0]) //Remplissage des contours avec hiérarchie (donc plus long)
				{
					Scalar color (rand()%255, rand()%255, rand()%255);
					drawContours(res,contours,i,color,CV_FILLED, 8, hierarchy);

				}*/

	t = ((double)getTickCount()-t)/getTickFrequency();
	std::cout << "temps find contours : " << t << '\n';

	t = (double)getTickCount();

	// Initialisation des matrices res et rect sous forme de matrice en couleur. Sans ces lignes, elles seront uniquement en nuances de gris, ce qui est peu pratique pour visualiser la labelisation.
	res = Mat::zeros(bin.size(), CV_8UC3);
	rect = Mat::zeros(bin.size(), CV_8UC3);

	// La recherche de contours repère plein de pixels ou groupe de pixels isolés qui ne sont pas des symboles et qu'il faut donc filtrer.
	for(i=0; i<contours.size(); i++)
	{
		if (contours[i].size() > 100) // La valeur de 100 est ARBITRAIRE et fonctionne pour l'image utilisée pour test. A revoir.
		{
			contours_filt.push_back(contours[i]);
		}
	}

	t = ((double)getTickCount()-t)/getTickFrequency();
	std::cout << "temps filtrage : " << t << '\n';

	/*
	Algo de recherche du signe "=".
	On cherche deux contours dont la moyenne des abscisses de l'un est compris entre l'abscisse min et l'abscisse max de l'autre.
	Une fois trouvés, on les place à la fin du vecteur afin de les fusionner.
	*/

	t = (double)getTickCount();

	size_cont = contours_filt.size();
	i = 0;
	while (i<size_cont && b == 0)
	{
		for(j=0; j<contours_filt[i].size(); j++)
		{
			vect_x_1.push_back(contours_filt[i][j].x);
			std::sort(vect_x_1.begin(),vect_x_1.end());
		}
		std::cout << "b1" << '\n';
		k = i+1;
		while(k<size_cont && b == 0)
		{
						std::cout << contours_filt[k].size() << '\n';
			for(j=0; j<contours_filt[k].size(); j++)
			{
				vect_x_2.push_back(contours_filt[k][j].x);
				std::sort(vect_x_2.begin(),vect_x_2.end());
			}
			std::cout << "b2" << '\n';

			medv2 = vect_x_2[vect_x_2.size()/2];
			debv1 = vect_x_1[0];
			endv1 = vect_x_1[vect_x_1.size()-1];

				/*std::cout << "i : " << i << '\n';
				std::cout << "k : " << k << '\n';
				std::cout << "medv2 : " << medv2 << '\n';
				std::cout << "debv1 : " << debv1 << '\n';
				std::cout << "endv1 : " << endv1 << '\n';*/
			if(medv2 > debv1 && medv2 < endv1)
			{
				contours_filt[k].swap(contours_filt[size_cont-2]);
				contours_filt[i].swap(contours_filt[size_cont-1]);
				b = 1;
			}
			k++;
		}
		std::cout << "b3" << '\n';

		i++;
	}
	std::cout << "sortie" << '\n';
	// On a placé les deux contours correspondants aux deux barres du signe "=" à la fin du vecteur "contours_filt".
	//On fusionne donc les deux derniers éléments de ce vecteur.
	cont_eq_1 = contours_filt[size_cont-2];
	cont_eq_2 = contours_filt[size_cont-1];
	size_1 = cont_eq_1.size();
	size_2 = cont_eq_2.size();
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

	t = ((double)getTickCount()-t)/getTickFrequency();
	std::cout << "temps égal : " << t << '\n';

	t = (double)getTickCount();

	// On effectue désormais divers traitements sur chacun des contours trouvés. Chaque contour correspond désormais à un symbole( y compris pour "=").
	for(i=0; i<contours_filt.size(); i++)
	{
		color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255)); // Calcul du couleur aléatoire pour visualiser la labelisation

		// Remplissage des symboles avec la couleur générée pour les images labelisées et labelisées avec enveloppe rectangulaire
		drawContours(res,contours_filt,i,color,CV_FILLED, 8);
		drawContours(rect,contours_filt,i,color,CV_FILLED, 8);

		// Calcul deux points remarquables du rectangle entourant le symbole étudié
		pt1 = boundingRect(contours_filt[i]).tl();
		pt2 = boundingRect(contours_filt[i]).br();

		rectangle(rect,pt1,pt2,color,2,8,0); // Trace le rectangle en question avec la couleur générée

		// Convertit "i" en chaine de caractère pour numéroter les images extraites
		convert.str("");
		convert << i;
		tmp_str = convert.str();

		// Complète le nom de l'image avec son numéro (i)
		name_img.replace(name_img.begin()+13,name_img.begin()+14,tmp_str);
			//std::cout << name_img << '\n';
		// Extrait de l'image binaire la partie correspondant à l'enveloppe rectangulaire du symbole étudié et la stocke dans une nouvelle image.
		subImage = Mat(bin2,Rect(pt1,pt2));
		imwrite (name_img, subImage);
			//std::cout << contours[i] << '\n';
			//std::cout << i << '\n';
			//std::cout << "couleur : " << color[0]<< " " << color[1]<< " " << color[2]<< '\n';
	t = ((double)getTickCount()-t)/getTickFrequency();
	std::cout << "temps traitement : " << t << '\n';


	t = (double)getTickCount();

		detecCavite(subImage);
	t = ((double)getTickCount()-t)/getTickFrequency();
	std::cout << "temps cav : " << t << '\n';


	}


	// Stockage des matrices calculées sous forme d'image afin de visualiser
//	imwrite("connected_comp.jpg", res);
//	imwrite("connected_comp_rect.jpg", rect);

}












float * detecCavite(Mat& envRect)
{
/*	char d[12] = {255,0,0,0,255,0,0,0,0,255,0,0};
	Mat bin;
	Mat mat1(2,2,CV_8UC3,d);
	threshold(mat1,bin,0,255,THRESH_BINARY_INV+THRESH_OTSU); // Seuil via algo OTSU : convertit une image en une image binaire ("vrai" noir et blanc)
	std::cout << bin << '\n';
	imwrite("nomdemerde.jpg", bin);*/
	float res[5];
	int a =0;
	int val;
	float sizeMat = envRect.rows*envRect.cols;
	bool g=0,b=0,d=0,h=0,c=0;
	float gauche = 0, droite = 0, bas = 0, haut = 0, central =0;
	for (int i = 0 ; i < envRect.rows ; i++) {
   		for (int j = 0 ; j < envRect.cols ; j++) {
			val = envRect.at<uchar>(i,j);
			if (val == 0)
			{
				a++;
				for(int k = j; k>=0 && g==0; k--)
				{
					if(255 == envRect.at<uchar>(i,k))
						g = 1;
				}
				for(int k = j; k < envRect.cols && d==0; k++)
				{
					if(255 == envRect.at<uchar>(i,k))
						d = 1;

				}
				for(int k = i; k>=0 && h==0; k--)
				{
					if(255 == envRect.at<uchar>(k,j))
						h = 1;
				}
				for(int k = i; k < envRect.rows && b==0; k++)
				{
					if(255 == envRect.at<uchar>(k,j))
						b = 1;
				}
//				std::cout << " gauche " << g << " droite " << d << " haut " << h << " bas " << b << "\n";

				if (g == 1 && d == 1 && b == 1 && h == 1)
				{
					central++;
//					std::cout << "central : " << central << '\n';
				}
				else if (g == 1 && d == 1 && h == 1 && b == 0)
				{
					bas++;
//					std::cout <<"bas: " << bas << '\n';
				}
				else if (b == 1 && d == 1 && h == 1 && g == 0)
				{
					gauche++;
//					std::cout << "gauche: " << gauche << '\n';
				}
				else if (g == 1 && b == 1 && h == 1 && d == 0)
				{
					droite++;
//					std::cout << "droite: " << droite << '\n';

				}
				else if (g == 1 && d == 1 && b == 1 && h == 0)
				{
					haut++;
//					std::cout << "haut: " << haut << '\n';

				}
			}
			g=0;d=0;
			b=0;h=0;
	   	}
	}
	std::cout << " gauche " << gauche << " droite " << droite << " haut " << haut << " bas " << bas << " central " << central << "a " << a << "\n";
	res[0] = gauche/sizeMat;
	res[1] = droite/sizeMat;
	res[2] = haut/sizeMat;
	res[3] = bas/sizeMat;
	res[4] = central/sizeMat;
	return res;
}

