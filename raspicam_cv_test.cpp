#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "opencv2/core/core.hpp"

#define ch 99 // Paramètre filtrage
#define nbr_fichier 3 // Nombre de fichiers contenant des vecteurs de cavités (BDD)

using namespace cv;
void lectureFic(int num_fichier);
int effectuerCalcul();
void detecCavite(Mat& envRect);
int trouverMoyenne(Mat& img);
void CalculSymbole();
int getAbsMin();
float distEucl(float * cavite, int symbole);
void inserOrdre(int cf_size);
void compDistance(float * cavite);
void affichageConsole();
int maxSymbole();
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
Mat bin;			    // Image binaire (Noir et blanc après seuil)
Mat bin2;			    // Clone de bin afin de la sauvegarder avant la recherche de contours
int res_calcul;
std::vector<int> symbole;
std::vector<int> abs_ordre;
std::vector<int> abs_ordre_fin;
std::vector<Mat> symb_ordre;
std::vector<Mat> symb_ordre_fin;
int valeur_symbole[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
// Ouest - Est - Nord - Sud - Central - Nord Ouest - Nord Est - Sud Ouest - Sud est
// 0-9 - Plus - Div - Mult
float cav_moyenne[14][9] = {{0.0001,0.0009,0.0056,0,0.4585,0.1260,0.0252,0.0224,0.1106},{0.1319,0.0003,0.0098,0.0572,0.2487,0.0795,0.06313,0.0864},{0.1549,0.1599,0.0026,0.0070,0.1037,0.1813,0.1354,0.0176,0.0338},{0.3135,0.0476,0.0017,0.0008,0.0298,0.1149,0.0827,0.0607,0.1301},{0.0059,0.0553,0.0065,0.0039,0.0663,0.1047,0.0495,0.0760,0.0987},{0.1987,0.2014,0.0024,0.0005,0.0243,0.1682,0.0159,0.0416,0.1500},{0,0.1687,0,0.0003,0.1688,0.2111,0.0203,0.0246,0.2279},{0.2147,0.0499,0.0047,0.0043,0.0417,0.0771,0.0054,0.1062,0.3152},{0.0461,0.0569,0.0033,0.0009,0.2818,0.1565,0.0113,0.0167,0.1849},{0.1969,0.0210,0.0040,0.0019,0.2006,0.1467,0.0187,0.0733,0.1148},{0.0045,0.0404,0.0160,0.0168,0.0037,0.1739,0.1496,0.1770,0.2611},{0,0,0,0.0008,0,0.4056,0.0002,0.0002,0.5158},{0.0450,0.0963,0.1069,0.1029,0.0024,0.2173,0.0134,0.632,0.2088},{0.21,0.05,0,0.01,0.03,0.17,0.31,0,0}};
//float cav_moyenne[14][9];
std::vector<std::vector<Point> > contours_filt;
int main ( int argc,char **argv ) {
//Declaration des variables

	Mat img = imread("pic_init.jpg",1); // Image initiale
/*	Mat imgm10;
	Mat imgm1;
	Mat img0;
	Mat img1;
	Mat img2;
	Mat img3;
	Mat img4;
	Mat img5;
	Mat img6;
	Mat img7;
	Mat img8;
	Mat img9;
	Mat img10;
	Mat img11;
	Mat img12;
	Mat img13;
	Mat img14;
	Mat img15;*/

//	imwrite("test0.jpg", img);
	int i = 0, j = 0, k = 0;
	int size_cont;  // Nombre de contours après filtrage (i.e. nombre de symboles en comptant le signe "=" comme deux)
	//Les 5 prochaines variables sont utilisées pour condenser le signe "=" en un seul symbole
	int size_1;	// Nombre de points d'un contour
	int size_2;	// Nombre de points d'un autre contour
	int medv2;	// Abscisse du milieu du deuxième contour
	int debv1;	// Abscisse de début du premier contour
	int endv1;	// Abscisse de début du premier contour

	int b = 0;	// Booléen de sortie de boucle
	int seuil;      // Valeur du seuillage permettant de filtrer la gaussienne
	double t;
	t = (double)getTickCount();

	std::vector<std::vector<Point> > contours; 	// Contient les points servant à définir les contours des symboles

	// Les 5 vecteurs suivants sont des vecteurs temporaires utilisés pour condenser le signe "=" en un seul symbole
	std::vector<Point> cont_eq;
	std::vector<Point> cont_eq_1;
	std::vector<Point> cont_eq_2;
	std::vector<int> vect_x_1;
	std::vector<int> vect_x_2;
	cvtColor(img,img,CV_BGR2GRAY);
	#ifdef DEBUG
		imwrite("converted.jpg", img);
	#endif
/*	seuil = trouverMoyenne(img);
	threshold(img,img,seuil,255,THRESH_TRUNC);
	imwrite("trunc.jpg",img);*/
/*	threshold(img,img,101,255,THRESH_TOZERO);
	imwrite("tozero.jpg",img);*/
/*	threshold(img,img2,111,255,THRESH_TOZERO_INV);
	imwrite("tozeroinv.jpg",img2);*/


/*	medianBlur(img,img,9);
	#ifdef DEBUG
		imwrite("filtre_median.jpg", img);
	#endif*/

	// Filtrage gaussien ("mean") pour seuillage local
/*	adaptiveThreshold(img,img1,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,-10);
	adaptiveThreshold(img,img1,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,-1);
	adaptiveThreshold(img,img1,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,0);
	adaptiveThreshold(img,img1,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,1);
	adaptiveThreshold(img,img2,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,2);
	adaptiveThreshold(img,img3,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,3);
	adaptiveThreshold(img,img4,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,4);
	adaptiveThreshold(img,img5,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,5);
	adaptiveThreshold(img,img6,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,6);
	adaptiveThreshold(img,img7,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,7);*/
	adaptiveThreshold(img,img,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,8);
/*	adaptiveThreshold(img,img9,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,9);
	adaptiveThreshold(img,img10,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,10);
	adaptiveThreshold(img,img11,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,11);
	adaptiveThreshold(img,img12,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,12);
	adaptiveThreshold(img,img13,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,13);
	adaptiveThreshold(img,img14,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,14);
	adaptiveThreshold(img,img15,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,ch,15);*/
	#ifdef DEBUG
		imwrite("filtre_gaussien8.jpg",img);
	#endif
	std::cout << " filtre gaussien ok " << std::endl ;

/*	equalizeHist(img,img);
	#ifdef DEBUG
		imwrite("equalized.jpg", img);
	#endif*/
	if(!img.empty()) {
		threshold(img,bin,0,255,THRESH_BINARY_INV+THRESH_OTSU); // Seuil via algo OTSU : convertit une image en une image binaire ("vrai" noir et blanc)
	}
	else {
		std::cout << "img is empty" << '\n';
		exit(0);
	}
	#ifdef DEBUG
		imwrite("threshold_im.jpg",bin);
	#endif
	bin2 = bin.clone(); // On clone bin car elle va être modifier avec la recherche de contour et on souhaite la conserver pour extraire les enveloppes rectangulaires des symboles et les stocker dans de nouvelles imgaes

	if(!bin.empty()) {
		findContours(bin,contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_TC89_L1); // Recherche des contours des composantes connexes de l'image
	}
	else {
		std::cout << "bin is empty" << '\n';
		exit(0);
	}


	// La recherche de contours repère plein de pixels ou groupe de pixels isolés qui ne sont pas des symboles et qu'il faut donc filtrer. (Inutile désormais ?)
	for(i=0; i<contours.size(); i++) {
		if (contours[i].size() > 5) // La valeur est ARBITRAIRE
			contours_filt.push_back(contours[i]);
	}
	std::cout << "taille contour " << contours.size() << " taille contour_filt " << contours_filt.size() << std::endl;


// Gestion signe égal trop longue lorsque le nombre de symboles augmente. A voir plus tard.

	//Algo de recherche du signe "=".
	//On cherche deux contours dont la moyenne des abscisses de l'un est compris entre l'abscisse min et l'abscisse max de l'autre.
	//Une fois trouvés, on les place à la fin du vecteur afin de les fusionner. (SOLUTION ALTERNATIVE : ON LES SUPPRIME)

/*
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
		k = i+1;
		while(k<size_cont && b == 0)
		{
			for(j=0; j<contours_filt[k].size(); j++)
			{
				vect_x_2.push_back(contours_filt[k][j].x);
				std::sort(vect_x_2.begin(),vect_x_2.end());
			}

			medv2 = vect_x_2[vect_x_2.size()/2];
			debv1 = vect_x_1[0];
			endv1 = vect_x_1[vect_x_1.size()-1];

			if(medv2 > debv1 && medv2 < endv1)
			{
				// SOLUTION ALTERNATIVE : SUPPRIMER LES DEUX BARRES DU SIGNE EGAL
//				contours_filt[k].erase(); A verif
//				contours_filt[i].erase(); A verif
				contours_filt[k].swap(contours_filt[size_cont-2]);
				contours_filt[i].swap(contours_filt[size_cont-1]);
				contours_filt.pop_back();
				contours_filt.pop_back();
				b = 1;
			}
			k++;
		}
		i++;
	}*/
	// On a placé les deux contours correspondants aux deux barres du signe "=" à la fin du vecteur "contours_filt".
	//On fusionne donc les deux derniers éléments de ce vecteur.
/*	cont_eq_1 = contours_filt[size_cont-2];
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

	CalculSymbole();
	t = ((double)getTickCount()-t)/getTickFrequency();
	std::cout << "temps du programme : " << t << '\n';
	res_calcul = effectuerCalcul(); // Calcul du résultat de la ligne
	affichageConsole(); // Affichage en mode texte
}

void CalculSymbole()
{
	Scalar color;
	RNG rng(1);
	Mat res, rect, subImage;
	int i=0,j;
	Point pt1; // Point en haut à droite d'un rectangle correspondant à une enveloppe rectangulaire
	Point pt2; // Idem en bas à gauche
	std::ostringstream convert;		// Variable utilisée pour la conversion d'un entier en chaine de caractère (pour insertion dans le nom de l'image afin de numéroter les images contenant les symboles)
	string tmp_str;				// Chaine de caractère temporaire pour traitement
	string name_img = "env_rect_symb_.jpg"; // Nom (à compléter par la valeur) d'une image contenant un symbole
	int cf_size = contours_filt.size();
	res = Mat::zeros(bin.size(), CV_8UC3);
	rect = Mat::zeros(bin.size(), CV_8UC3);
	// On effectue désormais divers traitements sur chacun des contours trouvés. Chaque contour correspond désormais à un symbole( y compris pour "=").
	for(i=0; i<cf_size; i++)
	{
		color = Scalar(rng.uniform(0,255),rng.uniform(0,255),rng.uniform(0,255)); // Calcul du couleur aléatoire pour visualiser la labelisation

		// Remplissage des symboles avec la couleur générée pour les images labelisées et labelisées avec enveloppe rectangulaire
		drawContours(res,contours_filt,i,color,CV_FILLED, 8);
		drawContours(rect,contours_filt,i,color,CV_FILLED, 8);

		// Calcul deux points remarquables du rectangle entourant le symbole étudié
		pt1 = boundingRect(contours_filt[i]).tl();
		pt2 = boundingRect(contours_filt[i]).br();
//		std::cout << "Abscisse : " << pt1.x << std::endl;
		abs_ordre.push_back(pt1.x);
		rectangle(rect,pt1,pt2,color,2,8,0); // Trace le rectangle en question avec la couleur générée

		// Convertit "i" en chaine de caractère pour numéroter les images extraites
		convert.str("");
		convert << i;
		tmp_str = convert.str();

		// Complète le nom de l'image avec son numéro (i)
		name_img.replace(name_img.begin()+13,name_img.begin()+14,tmp_str);
		// Extrait de l'image binaire la partie correspondant à l'enveloppe rectangulaire du symbole étudié et la stocke dans une nouvelle image.
		subImage = Mat(bin2,Rect(pt1,pt2));
		symb_ordre.push_back(subImage);
		#ifdef DEBUG
//			imwrite (name_img, subImage);
		#endif
	}
	for(i=0;i<cf_size;i++)
	{
//		std::cout << "Abscisse avant tri : " << abs_ordre[i] << std::endl;
	}
	inserOrdre(cf_size);
	for(i=0;i<cf_size;i++)
	{
		std::cout << "Abscisse après tri : " << abs_ordre_fin[i] << std::endl;
	}
	for(i=0;i<cf_size;i++)
	{
		for(j=0;j<14;j++)
			valeur_symbole[j] = 0;
		detecCavite(symb_ordre_fin[i]);
		symbole.push_back(maxSymbole());
	}
	#ifdef DEBUG
		// Stockage des matrices calculées sous forme d'image afin de visualiser
		imwrite("connected_comp.jpg", res);
		imwrite("connected_comp_rect.jpg", rect);
	#endif
}
// Remise des symboles dans le bon ordre : le premier est celui dont l'abscisse est la plus faible
void inserOrdre(int cf_size)
{
	int i,j,min;
	for(i=0;i<cf_size;i++)
	{
		min = getAbsMin();
//		std::cout << "Min abs : " << min << std::endl;

		symb_ordre_fin.push_back(symb_ordre[min]);
		abs_ordre_fin.push_back(abs_ordre[min]);
		for(j=0;j<=abs_ordre.size();j++)
		{
//			std::cout << "Abscisses avant transfo : " << abs_ordre[j];
		}
//		std::cout<< " abs_ordre_min : " << abs_ordre[min] << " abs_ordre_size : " << abs_ordre[abs_ordre.size()] << std::endl;
		abs_ordre[min] = abs_ordre[abs_ordre.size()-1];
		symb_ordre[min] = symb_ordre[abs_ordre.size()-1];
		abs_ordre.pop_back();
		symb_ordre.pop_back();
		for(j=0;j<=abs_ordre_fin.size();j++)
		{
//			std::cout << "   Abscisses apres transfo : " << abs_ordre_fin[j];
		}


		/*for(j=i;j<cf_size;j++)
		{
			if(abs_ordre[i]>abs_ordre[j])
			{
				abs_ordre[i].swap(abs_ordre[j]);
				symb_ordre[i].swap(symb_ordre[j]);
			}
		}*/
	}
}

// Récupérer le placement actuel du symbole dont l'abscisse est la plus faible
int getAbsMin()
{
	int i;
	int num_min = 0;
	int min;
	int abs_size = abs_ordre.size();
	min = abs_ordre[0];
	for(i=0;i<abs_size;i++)
	{
//		std::cout << " abs_ordre_i : " << abs_ordre[i];
		if(abs_ordre[i] < min)
		{
//			std::cout << "Min avant : " << min  << " ";
			min = abs_ordre[i];
//			std::cout << "Min après : " << min <<  " ";
			num_min = i;
		}
//		std::cout << std::endl;
	}
	std::cout << "Min :" << min << std::endl;
	return num_min;
}

// Affichage en mode texte
void affichageConsole()
{
	int i;
	std::cout << "Vous avez ecrit : ";
	for(i=0;i<symbole.size();i++)
	{
		if(symbole[i] < 10)
		{
			std::cout << symbole[i];
		}
		else if(symbole[i] == 10)
		{
			std::cout << " + ";
		}
		else if(symbole[i] == 11)
		{
			std::cout << " / ";
		}
		else if(symbole[i] == 12)
		{
			std::cout << " x ";
		}
		else if(symbole[i] == 13)
		{
			std::cout << " - ";
		}
		else
		{
			std::cout << " = ";
		}
	}
	std::cout << "Le resultat est : " << res_calcul << std::endl;
}

// Calcul de la ligne
int effectuerCalcul()
{
	int nbr_operateurs = 0;
	int nbr1 = 0;
	int nbr2 = 0;
	int i = 0;
	int op;

	while(symbole[i] < 10)
	{
		nbr1*=10;
		nbr1+=symbole[i];
		i++;
	}
	//std::cout << "Nbr 1 : " << nbr1 <<  std::endl;
	if(i<symbole.size())
	{
		op = symbole[i];
		nbr_operateurs++;
		i++;
	}
	while(i<symbole.size())
	{
		while(i < symbole.size() && nbr_operateurs < 2)
		{
			if(symbole[i] < 10)
			{
				nbr2*=10;
				nbr2+=symbole[i];
				i++;
	//			std::cout << "Nbr 2 : " << nbr2 <<  std::endl;
			}
			else
			{
				nbr_operateurs++;
			}
		}
//		std::cout << "operateur : " << op << std::endl;
		switch(op)
		{
		case 10:
	//	std::cout << "Nbr 2 : " << nbr2 <<  std::endl;
	//	std::cout << "Nbr 1 : " << nbr1 <<  std::endl;
			nbr1+=nbr2;
	//	std::cout << "Nbr 1 : " << nbr1 <<  std::endl;
			break;
		case 11:
			nbr1/=nbr2;
			break;
		case 12:
			nbr1*=nbr2;
			break;
		case 13:
			nbr1-=nbr2;
			break;
		default:
			break;
		}
	//	std::cout << "Nbr 1 : " << nbr1 <<  std::endl;
		nbr2=0;
		nbr_operateurs--;
		op=symbole[i];
		i++;
	}
	//std::cout << "Resultat " << nbr1 << std::endl;
	return nbr1;
}

void detecCavite(Mat& envRect)
{
	float res[9];
	int a =0;
	int i;
	int val,symbole;
	std::ofstream fichier;
	float sizeMat = envRect.rows*envRect.cols;
	bool g=0,b=0,d=0,h=0,c=0;
	float gauche = 0, droite = 0, bas = 0, haut = 0, central =0, nord_ouest = 0, sud_ouest = 0, nord_est = 0, sud_est = 0;
	for (i = 0 ; i < envRect.rows ; i++) {
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
				if (g == 1 && d == 1 && b == 1 && h == 1)
				{
					central++;
				}
				else if (g == 1 && d == 1 && h == 1 && b == 0)
				{
					bas++;
				}
				else if (b == 1 && d == 1 && h == 1 && g == 0)
				{
					gauche++;
				}
				else if (g == 1 && b == 1 && h == 1 && d == 0)
				{
					droite++;
				}
				else if (g == 1 && d == 1 && b == 1 && h == 0)
				{
					haut++;
				}
				else if (g == 0 && d == 1 && b == 1 && h == 0)
				{
					nord_ouest++;
				}
				else if (g == 1 && d == 0 && b == 1 && h == 0)
				{
					nord_est++;
				}
				else if (g == 0 && d == 1 && b == 0 && h == 1)
				{
					sud_ouest++;
				}
				else if (g == 1 && d == 0 && b == 0 && h == 1)
				{
					sud_est++;
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
//	std::cout << " gauche " << gauche << " droite " << droite << " haut " << haut << " bas " << bas << " nord_ouest " << nord_ouest << " nord_est " << nord_est << " sud_ouest " << sud_ouest << " sud_est " << sud_est << " central " << central << "total " << a << "\n";
	fichier.open("log.txt", std::ios_base::app);
	for (int z = 0 ; z < 9 ; z++) {
		fichier << res[z] << "  ";
	}
	fichier << std::endl;
	fichier.close();
	compDistance(res);
	std::cout << valeur_symbole[0] << " " << valeur_symbole[1] << " " << valeur_symbole[2] << " " << valeur_symbole[3] << " " << valeur_symbole[4] << " " << valeur_symbole[5] << " " << valeur_symbole[6] << " " << valeur_symbole[7] << " " << valeur_symbole[8] << " "  << valeur_symbole[9] << " " << valeur_symbole[10] << " " << valeur_symbole[11] << " " << valeur_symbole[12] << " " << valeur_symbole[13] << std::endl;

	for(i=1;i<=nbr_fichier;i++)
	{
		lectureFic(i);
		compDistance(res);
		std::cout << valeur_symbole[0] << " " << valeur_symbole[1] << " " << valeur_symbole[2] << " " << valeur_symbole[3] << " " << valeur_symbole[4] << " " << valeur_symbole[5] << " " << valeur_symbole[6] << " " << valeur_symbole[7] << " " << valeur_symbole[8] << " "  << valeur_symbole[9] << " " << valeur_symbole[10] << " " << valeur_symbole[11] << " " << valeur_symbole[12] << " " << valeur_symbole[13] << std::endl;

	}
	std::cout << "Symbole : " << maxSymbole() << std::endl;
}

// Fonction permettant de repérer la moyenne des valeurs des pixels afin de filtrer la gaussienne sur l'histogramme de l'image (ancienne fonction trouver contours. Le début est réutilisé, la fin est en commentaires)
int trouverMoyenne(Mat& img)
{
	Mat subImage;
	int sommeLigne[img.rows];
//	int sommeColonne[img.cols];
//	int moyColonne = 0;
	int moyLigne = 0;
//	int zz = 0;
//	int x_min = 0, x_max = 0, y_min = 0, y_max = 0;
//	int x_min_tmp, x_max_tmp, y_min_tmp, y_max_tmp;
//	for (int j = 0 ; j < img.cols ; j++) {
//		sommeColonne[j] = 0;
//	}
	for (int j = 0 ; j < img.rows ; j++) {
		sommeLigne[j] = 0;
	}
	for (int i = 0 ; i < img.rows ; i++) {
		for (int j = 0 ; j < img.cols ; j++) {
			sommeLigne[i] += (int)img.at<uchar>(i,j);
//			sommeColonne[j] += (int)img.at<uchar>(i,j);
		}
		sommeLigne[i] /= img.cols;
	}
//	for (int j = 0 ; j < img.cols ; j++) {
//		sommeColonne[j] /= img.rows;
//		moyColonne += sommeColonne[j];
//	}
//	moyColonne /= img.cols;
//	std:: cout << "moyColonne : " << moyColonne << " nb Colonnes : " << img.cols << std::endl;

	for (int j = 0 ; j < img.rows ; j++) {
		moyLigne += sommeLigne[j];
	}

	moyLigne /= img.rows;
	std:: cout << "moyLigne : " << moyLigne << " nb Lignes : "<< img.rows << std::endl;
//	moyLigne -= moyLigne*5/100;
//	std::cout<< "Moyenne ligne - 5% " << moyLigne << std::endl;
	return moyLigne;
/*


	for (int j = 0 ; j < img.rows ; j++) {
		if (sommeLigne[j] < moyLigne) {
			x_min_tmp = j;
			while (j < img.rows && sommeLigne[j] < moyLigne)
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
			y_min_tmp = j;
			while (j < img.cols && sommeColonne[j] > moyColonne)
				j++;
			y_max_tmp = j--;
			if (y_max_tmp - y_min_tmp > y_max - y_min) {
				y_max = y_max_tmp;
				y_min = y_min_tmp;
			}
		}
	}
	std::cout << x_min << " - " << x_max << " - " << y_min << " - " <<  y_max << std::endl;
	Point hg(x_min,  y_max);
	Point bd(x_max, y_min);
	subImage = Mat(img,Rect(hg,bd));
	imwrite("sub_image.jpg", subImage);*/
}

// Distance euclidienne entre le point que l'on cherche à reconnaitre et les 14 points du fichier de référence actuel
float distEucl(float * cavite, int symbole_d)
{
	int i;
	float somme = 0;
	for(i=0;i<9;i++)
	{
		somme += (cavite[i]-cav_moyenne[symbole_d][i])*(cavite[i]-cav_moyenne[symbole_d][i]);
//		if(symbole_d == 0)
//			std::cout << "Cavite : " << cavite[i] << " Cavite moyenne : " << cav_moyenne[symbole_d][i] << " Symbole " << symbole_d;
	}
//	std::cout << "Somme : " << somme << " pour le symbole : " << symbole_d;
//	if(symbole_d == 0)
//		std::cout << std::endl;
	return somme;
}

//Récupération de la distance euclidienne la plus faible
void compDistance(float * cavite)
{
	int i = 0;
	int res1 = 0;
	int res2 = 0;
	int res3 = 0;
	float dist_min1 = 0;
	float dist_min2 = 0;
	float dist_min3 = 0;
	float dist = distEucl(cavite,0);
	dist_min1 = dist;
	dist_min2 = dist;
	dist_min3 = dist;
	for(i=1;i<14;i++)
	{
		dist = distEucl(cavite,i);
//		std::cout << std::endl;
		if(dist < dist_min1)
		{
			res3= res2;
			res2=res1;
			res1 = i;
			dist_min3 = dist_min2;
			dist_min2 = dist_min1;
			dist_min1 = dist;
//			std::cout << " Distance minimale " << dist_min << " donc symbole : " << i << std::endl;
		}
		else if(dist < dist_min2)
		{
			res3=res2;
			res2=i;
			dist_min3=dist_min2;
			dist_min2=dist;
		}
		else if(dist < dist_min3)
		{
			res3=i;
			dist_min3=dist;
		}
	}
//	std::cout << "Symbole : " << res << std::endl;
	// Selon si la distance obtenue est la plus faible, la deuxième plus faible ou la troisième plus faible, on augmente la valeur du symbole correspondant. En augmentant les valeurs à l'aide des références de plusieurs fichiers, on pourra trouver le point le plus probable.
	valeur_symbole[res1]+=9;
	valeur_symbole[res2]+=4;
	valeur_symbole[res3]+=1;
//	return res;
}

// Lecture d'un ficher contenant 14 points de référence (un par symbole) afin de chercher la proximité du point à reconnaitre avec plusieurs échantillons
void lectureFic(int num_fichier)
{
	std::stringstream ss;
	std::ifstream fichier;
	ss << num_fichier;
	std::string str = ss.str();
	std::string nom = "cavite" + str + ".txt";
	fichier.open(nom.c_str(), std::ios::in);
	if(fichier)
	{
		for(int i=0;i<14;i++)
		{
			for(int j=0;j<9;j++)
			{
				fichier >> cav_moyenne[i][j];
			}
		}
	}
	fichier.close();
}
// Après avoir comparé les différentes distances, on cherche quel est le symbole le plus probable en cherchant celui dont la valeur est le plus élevé
int maxSymbole()
{
	int i;
	int valeur_max = valeur_symbole[0];
	int symbole_max = 0;
	for(i=0;i<14;i++)
	{
		if (valeur_symbole[i] > valeur_max)
		{
			valeur_max = valeur_symbole[i];
			symbole_max = i;
//			std::cout << "Valeur max : " << valeur_max << "Symbole max : " << symbole_max << std::endl;
		}
	}
	return symbole_max;
}
