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
Mat trouverContour(Mat& img);
#define DEBUG
//Mat CalculerContour(Mat& mat1);
float tab0[10][9];
float tab1[10][9];
float tab2[10][9];
float tab3[10][9];
float tab4[10][9];
float tab5[10][9];
float tab6[10][9];
float tab7[10][9];
float tab8[10][9];
float tab9[10][9];
float tabPlus[10][9];
float tabMoins[10][9];
float tabFois[10][9];
float tabDivise[10][9];
std::vector<std::vector<Point> > contours_filt;
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
	//Mat res;		  	    // Image labelisée
	//Mat rect;			    // Image labelisée avec enveloppe rectangulaire
	//Mat subImage;			    // Image rectangulaire contenant un unique symbole

	int i = 0, j = 0, k = 0;
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

	//string name_img = "env_rect_symb_.jpg"; // Nom (à compléter par la valeur) d'une image contenant un symbole
	//string tmp_str;				// Chaine de caractère temporaire pour traitement
	//std::ostringstream convert;		// Variable utilisée pour la conversion d'un entier en chaine de caractère (pour insertion dans le nom de l'image afin de numéroter les images contenant les symboles)

	//Point pt1; // Point en haut à droite d'un rectangle correspondant à une enveloppe rectangulaire
	//Point pt2; // Idem en bas à gauche

	//Scalar color; // Couleur aléatoire, permettant de visualiser la labelisation

	std::vector<std::vector<Point> > contours; 	// Contient les points servant à définir les contours des symboles
	std::vector<std::vector<Point> > contours_filt; // Contient les points servant à définir les contours de symboles après filtrage (1 contour = 1 symboles, sauf pour "=")

//	std::vector<Vec4i> hierarchy; 		   // Vecteur permettant la hiérarchisation des contours. Non utilisé.
	// Les 5 vecteurs suivants sont des vecteurs temporaires utilisés pour condenser le signe "=" en un seul symbole
	std::vector<Point> cont_eq;
	std::vector<Point> cont_eq_1;
	std::vector<Point> cont_eq_2;
	std::vector<int> vect_x_1;
	std::vector<int> vect_x_2;


	//RNG rng(1); // Tirage aléatoire pour les couleurs


	if(!img.empty())
		threshold(img,bin,0,255,THRESH_BINARY_INV+THRESH_OTSU); // Seuil via algo OTSU : convertit une image en une image binaire ("vrai" noir et blanc)
	else
		std::cout << "img is empty" << '\n';
	#ifdef DEBUG
		imwrite("threshold_im.jpg",bin);
	#endif
	bin2 = bin.clone(); // On clone bin car elle va être modifier avec la recherche de contour et on souhaite la conserver pour extraire les enveloppes rectangulaires des symboles et les stocker dans de nouvelles imgaes

	if(!bin.empty())
		findContours(bin,contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1); // Recherche des contours des composantes connexes de l'image
	else
		std::cout << "bin is empty" << '\n';

	// Initialisation des matrices res et rect sous forme de matrice en couleur. Sans ces lignes, elles seront uniquement en nuances de gris, ce qui est peu pratique pour visualiser la labelisation.
	//res = Mat::zeros(bin.size(), CV_8UC3);
	//rect = Mat::zeros(bin.size(), CV_8UC3);

	// La recherche de contours repère plein de pixels ou groupe de pixels isolés qui ne sont pas des symboles et qu'il faut donc filtrer.
	for(i=0; i<contours.size(); i++)
	{
		if (contours[i].size() > 25) // La valeur de 100 est ARBITRAIRE et fonctionne pour l'image utilisée pour test. A revoir.
		{
			contours_filt.push_back(contours[i]);
		}
	}
	std::cout << "taille contour " << contours.size() << " taille contour_filt " << contours_filt.size() << std::endl;
	/*
	Algo de recherche du signe "=".
	On cherche deux contours dont la moyenne des abscisses de l'un est compris entre l'abscisse min et l'abscisse max de l'autre.
	Une fois trouvés, on les place à la fin du vecteur afin de les fusionner.
	*/

	/*t = (double)getTickCount();

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
			endv1 = vect_x_1[vect_x_1.size()-1];*/

				//std::cout << "i : " << i << '\n';
				//std::cout << "k : " << k << '\n';
				//std::cout << "medv2 : " << medv2 << '\n';
				//std::cout << "debv1 : " << debv1 << '\n';
				//std::cout << "endv1 : " << endv1 << '\n';*/
			/*if(medv2 > debv1 && medv2 < endv1)
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
	std::cout << "sortie" << '\n';*/
	// On a placé les deux contours correspondants aux deux barres du signe "=" à la fin du vecteur "contours_filt".
	//On fusionne donc les deux derniers éléments de ce vecteur.
	/*cont_eq_1 = contours_filt[size_cont-2];
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
	contours_filt.push_back(cont_eq);*/


	/*// On effectue désormais divers traitements sur chacun des contours trouvés. Chaque contour correspond désormais à un symbole( y compris pour "=").
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
		std::cout << name_img << '\n';
		// Extrait de l'image binaire la partie correspondant à l'enveloppe rectangulaire du symbole étudié et la stocke dans une nouvelle image.
		subImage = Mat(bin2,Rect(pt1,pt2));
		imwrite (name_img, subImage);
			//std::cout << contours[i] << '\n';
			//std::cout << i << '\n';
			//std::cout << "couleur : " << color[0]<< " " << color[1]<< " " << color[2]<< '\n';

		detecCavite(subImage);
	}*/

	t = ((double)getTickCount()-t)/getTickFrequency();
	std::cout << "temps du programme : " << t << '\n';
}


void CalculSymbole()
{
	Scalar color;
	RNG rng(1);
	Mat res, rect, subImage;
	Point pt1; // Point en haut à droite d'un rectangle correspondant à une enveloppe rectangulaire
	Point pt2; // Idem en bas à gauche
	std::ostringstream convert;		// Variable utilisée pour la conversion d'un entier en chaine de caractère (pour insertion dans le nom de l'image afin de numéroter les images contenant les symboles)
	string tmp_str;				// Chaine de caractère temporaire pour traitement
	string name_img = "env_rect_symb_.jpg"; // Nom (à compléter par la valeur) d'une image contenant un symbole
	res = Mat::zeros(bin.size(), CV_8UC3);
	rect = Mat::zeros(bin.size(), CV_8UC3);
	// On effectue désormais divers traitements sur chacun des contours trouvés. Chaque contour correspond désormais à un symbole( y compris pour "=").
	for(int i=0; i<contours_filt.size(); i++)
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
		std::cout << name_img << '\n';
		// Extrait de l'image binaire la partie correspondant à l'enveloppe rectangulaire du symbole étudié et la stocke dans une nouvelle image.
		subImage = Mat(bin2,Rect(pt1,pt2));
		imwrite (name_img, subImage);
		detecCavite(subImage);
	}
	#ifdef DEBUG
		// Stockage des matrices calculées sous forme d'image afin de visualiser
		imwrite("connected_comp.jpg", res);
		imwrite("connected_comp_rect.jpg", rect);
	#endif
}








float * detecCavite(Mat& envRect)
{
	float res[9];
	int a =0;
	int val;
	std::ofstream fichier;
	float sizeMat = envRect.rows*envRect.cols;
	bool g=0,b=0,d=0,h=0,c=0;
	float gauche = 0, droite = 0, bas = 0, haut = 0, central =0, nord_ouest = 0, sud_ouest = 0, nord_est = 0, sud_est = 0;
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
				else if (g == 0 && d == 1 && b == 1 && h == 0)
				{
					nord_ouest++;
//					std::cout << "haut: " << haut << '\n';
				}
				else if (g == 1 && d == 0 && b == 1 && h == 0)
				{
					nord_est++;
//					std::cout << "haut: " << haut << '\n';
				}
				else if (g == 0 && d == 1 && b == 0 && h == 1)
				{
					sud_ouest++;
//					std::cout << "haut: " << haut << '\n';
				}
				else if (g == 1 && d == 0 && b == 0 && h == 1)
				{
					sud_est++;
//					std::cout << "haut: " << haut << '\n';
				}
			}
			g=0;d=0;
			b=0;h=0;
	   	}
	}
	res[0] = gauche/sizeMat;
	res[1] = droite/sizeMat;
	res[2] = haut/sizeMat;
	res[3] = bas/sizeMat;
	res[4] = central/sizeMat;
	res[5] = nord_ouest/sizeMat;
	res[6] = nord_est/sizeMat;
	res[7] = sud_ouest/sizeMat;
	res[8] = sud_est/sizeMat;
	std::cout << " gauche " << res[0] << " droite " << res[1] << " haut " << haut << " bas " << bas << " nord_ouest " << nord_ouest << " nord_est " << nord_est << " sud_ouest " << sud_ouest << " sud_est " << sud_est << " central " << central << "total " << a << "\n";
	fichier.open("test0.txt", std::ios_base::app);
	for (int z = 0 ; z < 9 ; z++) {
		fichier << res[z] << " ; ";
	}
	fichier << std::endl;
	fichier.close();
	return res;
}
/*Mat trouverContour(Mat& image)
{
	Mat subImage;
	long sommeLigne[img.rows];
	long sommeColonne[img.cols];
	int moyColonne = 0;
	int moyLigne = 0;
	int x_min = 0, x_max = 0, y_min = 0, y_max = 0;
	int x_min_tmp, x_max_tmp, y_min_tmp, y_max_tmp;
	Point hg, hd;
	for (int i = 0 ; i < img.rows ; i++) {
		for (int j = 0 ; j < img.cols ; j++) {
			sommeLigne[i] += img[i][j];
			sommeColonne[j] += img[i][j];
		}
		sommeLigne[i] /= img.rows;
	}
	for (int j = 0 ; j < img.cols ; j++) {
		sommeColonne[j] /= img.cols;
		moyColonne += sommeColonne[j];
	}
	moyColonne /= img.cols;
	for (int j = 0 ; j < img.rows ; j++) {
		sommeLigne[j] /= img.rows;
		moyLigne[j] += sommeLigne[j];
	}
	moyLigne /= img.rows;
	for (int j = 0 ; j < img.rows ; j++) {
		if (sommeLigne[j] > moyLigne) {
			x_min_tmp = sommeLigne[j];
			while (j < img.rows && sommeLigne[j] > moyLigne)
				j++;
			x_max_tmp = j--;
			if (x_max_tmp - x_min_tmp > x_max - x_min) {
				x_max = x_max_tmp;
				x_min = x_min_tmp;
			}
		}
	}
	for (int j = 0 ; j < img.cols ; j++) {
		if (sommeColonne[j] > moyColonne) {
			y_min_tmp = sommeColonne[j];
			while (j < img.cols && sommeColonne[j] > moyColonne)
				j++;
			y_max_tmp = j--;
			if (y_max_tmp - y_min_tmp > y_max - y_min) {
				y_max = y_max_tmp;
				y_min = y_min_tmp;
			}
		}
	}
	hg = (x_min, y_max);
	bd = (x_max, y_min);
	subImage = Mat(img,Rect(pt1,pt2));
	imwrite("sub_image.jpg", subImage);
}*/
