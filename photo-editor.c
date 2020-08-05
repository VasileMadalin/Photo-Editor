//Vasile Madalin Constantin 312CB
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "bmp_header.h"
#define nmax 200000
#define nmin -200000
#define len1 50
#define len2 50
typedef struct {
	int l;
	int c;  
} coord;
typedef struct {
	unsigned char  blue;
	unsigned char  green;
 	unsigned char  red;
} pixels;
typedef struct {
	long long blue;
	long long green; 
	long long red;
} pixels_sums;
//aceasta functie deschide un fisier binar si citeste datele de intrare
void read_input(char *input, char *filters, char *pooling, char *clustering) {
	int length;
	FILE *f;
	f = fopen("input.txt", "rb");
	if (f == NULL) {
		printf("Eroare la deschiderea fisierului input.txt");
	}
	fgets(input, len1, f);//citim denumirea imaginii
	length = strlen(input); input[length - 1] = '\0';
	fgets(filters, len2, f); //citim calea spre filtrul de la task3
	length = strlen(filters); filters[length-1] = '\0';
	fgets(pooling, len2, f);//citim calea spre filtrul de la task4
	length = strlen(pooling); pooling[length - 1] = '\0';
	fgets(clustering, len2, f);//citim calea spre filtrul de la task5
	length = strlen(clustering); clustering[length - 1] = '\0';
	fclose(f);
}
//aceasta functie afiseaza o imagine daca parametrul ok este 1
//si afiseaza doar cele doua headere daca ok este 0(se foloseste la task2
void print(FILE * q, bmp_fileheader a, bmp_infoheader b, pixels **mat,
	int ok) {
	int i, j, k;
	unsigned char t;
	signed int maxx;
	fwrite(&a.fileMarker1, sizeof(unsigned char), 1, q);
	fwrite(&a.fileMarker2, sizeof(unsigned char), 1, q);
	fwrite(&a.bfSize, sizeof(unsigned int), 1, q);
	fwrite(&a.unused1, sizeof(unsigned short), 1, q);
	fwrite(&a.unused2, sizeof(unsigned short), 1, q);
	fwrite(&a.imageDataOffset, sizeof(unsigned int), 1, q);
	fwrite(&b.biSize, sizeof(unsigned int), 1, q);
	if (ok == 1) {
		fwrite(&b.width, sizeof(signed int), 1, q);
		fwrite(&b.height, sizeof(signed int), 1, q);
	}
	else {//daca ok este 0 atunci dimensiunile imaginii se modifica
		if (b.height > b.width) maxx = b.height;//pentru task3
		else maxx = b.width;//atat nr de coloane cat si nr de linii vor avea
		fwrite(&maxx, sizeof(signed int), 1, q);//valorea maxima
		fwrite(&maxx, sizeof(signed int), 1, q);
	}
	fwrite(&b.planes, sizeof(unsigned short), 1, q);
	fwrite(&b.bitPix, sizeof(unsigned short), 1, q);
	fwrite(&b.biCompression, sizeof(unsigned int), 1, q);
	fwrite(&b.biSizeImage, sizeof(unsigned int), 1, q);
	fwrite(&b.biXPelsPerMeter, sizeof(int), 1, q);
	fwrite(&b.biYPelsPerMeter, sizeof(int), 1, q);
	fwrite(&b.biClrUsed, sizeof(unsigned int), 1, q);
	fwrite(&b.biClrImportant, sizeof(unsigned int),1 , q);
	if (ok == 1) {//daca ok este 1 afisam matricea de pixeli
		for (i = 0; i < b.height; i++) {
			for (j = 0; j < b.width; j++) {
				fwrite(&mat[i][j].blue, sizeof(unsigned char), 1, q);
				fwrite(&mat[i][j].green, sizeof(unsigned char), 1, q);
				fwrite(&mat[i][j].red, sizeof(unsigned char), 1, q);
			}
			t = 0;
			for (k = 0; k <(b.width)%4 ; k++)
				fwrite(&t, sizeof(unsigned char), 1, q);
		}
	}
}
void black_white(FILE * q, bmp_fileheader a, bmp_infoheader b,
	pixels ** mat) {
	int i, j;//aceasta functie inlocuieste fiecare pixek (R,G,B) cu (X,X,X)
	pixels **matrice = malloc(b.height * sizeof(pixels *));//X=[(R+G+B)/3]
	if (matrice == NULL)  printf("Nu s-a reusit alocarea\n");
	for (i = 0; i < b.height; i++) {
		matrice[i] = malloc(b.width * sizeof(pixels));
		if (matrice[i] == NULL) printf("Nu s-a reusit alocarea\n");
	}
	for (i = 0; i < b.height; i++) {//fiecare canal este inlocuit
		for (j = 0; j < b.width; j++) {// cu valoarea X
			matrice[i][j].blue = (mat[i][j].blue +
				mat[i][j].green + mat[i][j].red) / 3;
			matrice[i][j].green = (mat[i][j].blue +
   	   			mat[i][j].green + mat[i][j].red) / 3;
			matrice[i][j].red = (mat[i][j].blue +
				mat[i][j].green + mat[i][j].red) / 3;
		}
	}//afisam imaginea folosind functia print
	print(q, a, b, matrice, 1);
	for (i = 0; i < b.height; i++) free(matrice[i]);
	free(matrice);
}
void nocrop(FILE * q, bmp_fileheader a, bmp_infoheader b, pixels ** matrix) {
	int i, j, k;//aceasta functie transforma o imagine
	unsigned char t;//intr-o imagine patratica
	signed int front, back;
	print(q, a, b, matrix, 0);//afisam cele doua headere cu functia print
	//abordam cele doua cazuri(nr de coloane este mai mare decat cel de
	if (b.width > b.height) {//linii si invers)
		if (( b.width - b.height ) % 2 != 0) {
			front = ( b.width - b.height ) / 2;
			back = ( ( b.width - b.height ) / 2 ) + 1;
		}//back si front reprezinta numarul de linii
		else {//care trebuie completate cu pixeli albi
			front = (b.width - b.height) / 2;
			back = (b.width - b.height) / 2;
		}
		for (i = 0; i < back; i++) {
			for (j = 0; j < b.width; j++) {
				t = 255;//completam imaginea cu pixeli albi
				fwrite(&t, sizeof(unsigned char), 1, q);
				fwrite(&t, sizeof(unsigned char), 1, q);
				fwrite(&t, sizeof(unsigned char), 1, q);
			}
			t = 0;//punem valorea 0 pe toti octetii de padding
			for (k = 0;k < (b.width) % 4; k++) 
			fwrite(&t, sizeof(unsigned char), 1, q);
		}
		for (i = 0; i < b.height; i++) {
			for (j = 0;j < b.width; j++) {//adaugam pixelii imaginii initiale
				fwrite(&matrix[i][j].blue, sizeof(unsigned char), 1, q);
				fwrite(&matrix[i][j].green, sizeof(unsigned char), 1, q);
				fwrite(&matrix[i][j].red, sizeof(unsigned char), 1, q);
   	   	}
		t = 0;//punem valorea 0 pe toti octetii de padding
     	for (k = 0; k < (b.width) % 4; k++)
     		fwrite(&t, sizeof(unsigned char), 1, q);
		}
		for (i = 0; i < front; i++) {
		 	for (j = 0; j < b.width; j++) {
		 		t = 255;//completam imaginea cu pixeli albi
				fwrite(&t, sizeof(unsigned char), 1, q);
				fwrite(&t, sizeof(unsigned char), 1, q);
				fwrite(&t, sizeof(unsigned char), 1, q);
			}
		    t = 0;//punem valorea 0 pe toti octetii de padding
		    for (k = 0; k < (b.width) % 4; k++)
			fwrite(&t,sizeof(unsigned char),1,q);
		}
	}//al doilea caz
	if (b.width <= b.height) {//completam cu pixeli albi primele
		for (i = 0; i < b.height; i++) {//(b.height-b.width)/2 coloane
			for (j = 0;j < (b.height-b.width) / 2;j++) {
				t = 255;
				fwrite(&t, sizeof(unsigned char), 1, q);
				fwrite(&t, sizeof(unsigned char), 1, q);
				fwrite(&t, sizeof(unsigned char), 1, q);
			}
			for (j = 0; j < b.width; j++) {
				fwrite(&matrix[i][j].blue, sizeof(unsigned char), 1, q);
				fwrite(&matrix[i][j].green, sizeof(unsigned char), 1, q);
				fwrite(&matrix[i][j].red, sizeof(unsigned char), 1, q);
			}//completam cu pixeli albi ultimele 
			for (j = 0; j < (b.height - b.width) / 2; j++)  {
			t = 255;//(b.height-b.width)/2 coloane
				fwrite(&t, sizeof(unsigned char), 1, q);
				fwrite(&t, sizeof(unsigned char), 1, q);
				fwrite(&t, sizeof(unsigned char), 1, q);
			}
			t = 0;//punem valorea 0 pe toti octetii de padding
			for (k = 0; k < (b.height) % 4; k++)
				fwrite(&t, sizeof(unsigned char), 1, q);
		}
	}
}
void filter(FILE * q,FILE *q1 ,bmp_fileheader a,bmp_infoheader b,
	pixels ** matrix) {
	int i, j, size, s1, s2, s3, p, k, ni, nj;
	pixels **aux, **res;
	fscanf(q, "%d", &size);//citim dimensiunea filtru-ului
	int **mat_filter = malloc(len1 * sizeof(int *));
	if (mat_filter == NULL)  printf("Nu s-a reusit alocarea\n");
	for (i = 0; i < len1; i++) {
		mat_filter[i] = malloc(len1 * sizeof(int));
		if (mat_filter[i] == NULL) printf("Nu s-a reusit alocarea\n");
	}
	for (i = 0; i < size; i++)
		for (j = 0; j  < size; j++)//citim matricea-filtru
			fscanf(q, "%d", &mat_filter[i][j]);
	aux = malloc(b.height * sizeof(pixels *));
	if (aux == NULL)  printf("Nu s-a reusit alocarea\n");
	for (i = 0; i < b.height; i++) {
		aux[i] = malloc(b.width * sizeof(pixels));
		if (aux[i] == NULL) printf("Nu s-a reusit alocarea\n");
	}
	for (i = 0; i < b.height; i++)//copiem in matricea aux
		for (j = 0; j < b.width; j++) {//matricea pixelilor dar inversata
			aux[b.height - i - 1][j].blue = matrix[i][j].blue;
			aux[b.height - i - 1][j].green = matrix[i][j].green;
			aux[b.height - i - 1][j].red = matrix[i][j].red;
		}
	res = malloc(b.height*sizeof(pixels *));
	if (res == NULL)  printf("Nu s-a reusit alocarea\n");
	for (i = 0; i < b.height; i++) {
		res[i] = malloc(b.width * sizeof(pixels));
		if (res[i] == NULL) printf("Nu s-a reusit alocarea\n");
	}
	for (i = 0; i < b.height; i++)//cream o copie a matricei aux
		for (j = 0; j < b.width; j++) {
			res[i][j].blue = aux[i][j].blue;
			res[i][j].green = aux[i][j].green;
			res[i][j].red = aux[i][j].red;
		}
	for (i = 0; i < b.height; i++)//aplicam filtrul pentru fiecare pixel
		for (j = 0; j < b.width; j++) {
			s1 = 0; s2 = 0; s3 = 0;
			for (p = 0; p < size; p++) {//pentru fiecare pixel parcurgem
				for (k = 0; k < size; k++) {//matricea-filtru
					ni = i - (size / 2) + p;
					nj = j - (size / 2) + k;//aplicam formula
					if ((ni >= 0&&ni < b.height) && 
						(nj >= 0&&nj < b.width)) {
						s1 = s1 + mat_filter[p][k] * res[ni][nj].blue;
						s2 = s2 + mat_filter[p][k] * res[ni][nj].green;
						s3 = s3 + mat_filter[p][k] * res[ni][nj].red;
					}
				}
			}
			if (s1 < 0) s1 = 0;
			if (s1 > 255) s1 = 255;
			if (s2 < 0) s2 = 0;
			if (s2 > 255) s2 = 255;
			if (s3 < 0) s3 = 0;
			if (s3 > 255) s3 = 255;
			aux[i][j].blue = s1; aux[i][j].green = s2; aux[i][j].red = s3;
		}
	for (i = 0; i < b.height; i++)
		for (j = 0; j < b.width; j++) {//copiem in res mat. aux dar inversata
			res[b.height - i - 1][j].blue = aux[i][j].blue;
			res[b.height - i - 1][j].green = aux[i][j].green;
			res[b.height - i - 1][j].red = aux[i][j].red;
		}
	print(q1, a, b, res, 1);//afisam imaginea folosind functia print
	for (i = 0; i < b.height; i++) free(aux[i]);
	free(aux);
	for (i = 0; i < b.height; i++) free(res[i]);
	free(res);
	for (i = 0; i < len1; i++) free(mat_filter[i]);
	free(mat_filter);
}
void pool(FILE * q, FILE *q1, bmp_fileheader a, bmp_infoheader b,
	pixels ** matrix) {
	int i, j, size, ok, m1, m2, m3, p, k, ni, nj;
	char min_max;
	fscanf(q, "%c", &min_max);//citim tipul filtrului
	fscanf(q, "%d", &size);//citim dimensiunea filtrului
	pixels **aux, **res;
	aux = malloc(b.height * sizeof(pixels *));
	if (aux == NULL)  printf("Nu s-a reusit alocarea\n");
	for (i = 0; i < b.height; i++) {
		aux[i] = malloc(b.width * sizeof(pixels));
		if (aux[i] == NULL) printf("Nu s-a reusit alocarea\n");
	}
	for (i = 0; i < b.height; i++)
		for (j = 0; j < b.width; j++) {
			aux[b.height - i - 1][j].blue = matrix[i][j].blue;
			aux[b.height - i - 1][j].green = matrix[i][j].green;
			aux[b.height - i - 1][j].red = matrix[i][j].red;
		}
	res = malloc(b.height * sizeof(pixels *));
	if (res == NULL)  printf("Nu s-a reusit alocarea\n");
	for (i = 0; i < b.height; i++) {
		res[i] = malloc(b.width * sizeof(pixels));
		if (res[i] == NULL) printf("Nu s-a reusit alocarea\n");
	}
	for (i = 0; i < b.height; i++)
		for (j = 0;j < b.width;j++) {
			res[i][j].blue = aux[i][j].blue;
			res[i][j].green = aux[i][j].green;
			res[i][j].red = aux[i][j].red;
		}
	for (i = 0; i < b.height; i++)//analizam cele doua cazuri in care
		for (j = 0; j < b.width; j++) {//aplicam filtru max sau filtru min
			if (min_max == 'm') {
				m1 = nmax; m2 = nmax; m3 = nmax; ok = 1;
				for (p = 0; p < size; p++) {
					for (k = 0; k < size; k++) {
						ni = i - ( size / 2 ) + p;
						nj = j - ( size / 2 ) + k;//pentru minim calculam
						if ((ni >= 0 && ni < b.height) &&//min dintre vecini
							(nj >= 0 && nj < b.width)) {
							if (res[ni][nj].blue < m1) m1 = res[ni][nj].blue;
							if (res[ni][nj].green < m2) 
								m2 = res[ni][nj].green;
							if (res[ni][nj].red < m3) m3 = res[ni][nj].red;
          				}
						else ok = 0;
					}
				}
			}
			if (min_max == 'M') {
				m1 = nmin; m2 = nmin; m3 = nmin;
				for (p = 0; p < size; p++) {
					for (k = 0; k < size; k++) {
						ni = i - ( size / 2 ) + p;
						nj = j - ( size / 2 ) + k;//pentru maxim calculam
						if ((ni >= 0 && ni < b.height) &&//max dintre vecini
							(nj >= 0 && nj < b.width)) {
							if (res[ni][nj].blue > m1) m1 = res[ni][nj].blue;
							if (res[ni][nj].green > m2) 
								m2 = res[ni][nj].green;
							if (res[ni][nj].red > m3) m3 = res[ni][nj].red;
						}
					}
				}
			}
			if (m1 < 0) m1 = 0;//daca minimul este mai mic 
			if (m2 < 0) m2 = 0;//decat 0 atunci acesta devine 0
			if (m3 < 0) m3 = 0; 
			if (ok == 0) {
				m1 = 0; m2 = 0; m3 = 0;
			}//copiem in matricea pixelilor minimul sau maximul
			aux[i][j].blue = m1; aux[i][j].green = m2; aux[i][j].red = m3;
	}
	for (i = 0; i < b.height; i++)
		for (j = 0;j < b.width; j++) {//copiem in res mat. aux dar inversata
			res[b.height-i-1][j].blue = aux[i][j].blue;
			res[b.height-i-1][j].green = aux[i][j].green;
			res[b.height-i-1][j].red = aux[i][j].red;
		}
	print(q1, a, b, res,1);
	for (i = 0; i < b.height; i++) free(aux[i]);
	free(aux);
	for (i = 0; i < b.height; i++) free(res[i]);
	free(res);
}					 
void clustering(FILE * q, FILE *q1, bmp_fileheader a, 
	bmp_infoheader b, pixels ** matrix) {
	int size, i, j, area_number = 0, p, k, r, xc, yc, ni, nj;
	int **aux, *dim;
	fscanf(q, "%d", &size);
	unsigned char abs1, abs2, abs3; coord *v; pixels_sums *colors;
	int *dl = malloc(4*sizeof(int)), *dc = malloc(4*sizeof(int));
	dl[0] = 1; dl[1] = -1; dl[2] = 0; dl[3] = 0;
	dc[0] = 0; dc[1] = 0; dc[2] = -1; dc[3] = 1;
	v = malloc(nmax * sizeof(coord)); dim = malloc(nmax * sizeof(int));
	colors = malloc(nmax * sizeof(pixels_sums));
	if (v == NULL || dim == NULL || colors==NULL || dl==NULL || dc==NULL) 
		printf("Nu s-a reusit alocarea\n");
	aux = malloc( b.height * sizeof(int *));
	if (aux == NULL)  printf("Nu s-a reusit alocarea\n");
	for (i = 0; i < b.height; i++) {
		aux[i] = malloc(b.width * sizeof(int));
		if (aux[i] == NULL) printf("Nu s-a reusit alocarea\n");
	}//dam valoarea -1 fiecarui element din mat aux,nu face parte dintr-o zona
	for (i = 0;i < b.height; i++) for (j = 0;j <b.width;j++) aux[i][j] = -1;
	pixels **inv = malloc(b.height * sizeof(pixels *));
	if (inv == NULL)  printf("Nu s-a reusit alocarea\n");
	for (i = 0; i < b.height; i++) {
		inv[i] = malloc(b.width * sizeof(pixels));
		if (inv[i] == NULL) printf("Nu s-a reusit alocarea\n");
	}
	pixels **res = malloc(b.height * sizeof(pixels *));
	if (res == NULL)  printf("Nu s-a reusit alocarea\n");
	for (i = 0; i < b.height; i++) {
		res[i] = malloc(b.width * sizeof(pixels));
		if (res[i] == NULL) printf("Nu s-a reusit alocarea\n");
	}
	for (i = 0; i < b.height; i++)
		for ( j = 0; j < b.width; j++) {//copiem in inv,matrix dar inversata
			inv[b.height - i - 1][j].blue = matrix[i][j].blue;
			inv[b.height - i - 1][j].green = matrix[i][j].green;
			inv[b.height - i - 1][j].red = matrix[i][j].red;
		}
	for (i = 0; i < b.height; i++)
		for (j = 0; j < b.width; j++) {////verficam daca pixelul
			if (aux[i][j] == -1) {//de pe (i,j) nu face parte dintr-o zona
				k = 1; p = 1; v[k].l = i; v[k].c = j;
				aux[i][j] = area_number;//adaugam pixelul in noua zona
				while (p <= k) {//verficam pentru fiecare vecin al unui pixel
					xc = v[p].l; yc = v[p].c; p++;//adaugat intr-o noua zona
					for ( r = 0; r < 4; r++) {//daca respecta cerintele;se
	   					ni = xc + dl[r]; nj = yc + dc[r];//verifica si vecinii
						if (ni >=0 && ni < b.height && 
							nj >= 0 && nj < b.width) {
							if (aux[ni][nj] == -1) {//fiecarui vecin
								abs1 = abs(inv[i][j].blue - inv[ni][nj].blue);
								abs2 = abs(inv[i][j].green -
									inv[ni][nj].green);
								abs3 = abs(inv[i][j].red - inv[ni][nj].red);
								if (abs1 + abs2 + abs3 <= size) {
									k++; v[k].l = ni; v[k].c = nj;
									aux[ni][nj] = area_number;
								}
							}
						}
					}
				}//dupa ce am terminat ce construit zona curenta trecem
			area_number++;//la urmatoarea zona
			}
		}
	for ( i = 0; i < area_number; i++) {//initializam cu 0 suma(fiecarui
		colors[i].blue = 0; colors[i].green = 0; colors[i].red = 0;//canal)
		dim[i] = 0;// si dimensiunea fiecarei zone
	}
	for (i = 0; i < b.height; i++) {
		for (j = 0; j < b.width; j++ ) {//calculam suma pentru fiecare canal
			colors[aux[i][j]].blue = colors[aux[i][j]].blue+inv[i][j].blue;
			colors[aux[i][j]].green = colors[aux[i][j]].green+inv[i][j].green;
			colors[aux[i][j]].red = colors[aux[i][j]].red+inv[i][j].red;
			dim[aux[i][j]] = dim[aux[i][j]] + 1;//si dimensiunea fiecarei zone
		}
	}	
	for (i = 0; i < b.height; i++) {
		for (j = 0;j < b.width; j++) {//atribuim fiecarui pixel media ceruta
			inv[i][j].blue = colors[aux[i][j]].blue / dim[aux[i][j]];
			inv[i][j].green = colors[aux[i][j]].green / dim[aux[i][j]];
			inv[i][j].red = colors[aux[i][j]].red / dim[aux[i][j]];
		}
	}
	for (i = 0; i < b.height; i++) {
		for (j=0;j<b.width;j++) {//inversam matricea inv
			res[b.height-i-1][j].blue = inv[i][j].blue;
			res[b.height-i-1][j].green = inv[i][j].green;
			res[b.height-i-1][j].red = inv[i][j].red;
		}
	}//afisam matricea res, folosind functia print
	print(q1,a,b,res,1);
	for (i = 0; i < b.height; i++) free(inv[i]);
	free(inv);
	for (i = 0; i < b.height; i++) free(res[i]);
	free(res);
	for (i = 0; i < b.height; i++) free(aux[i]);
	free(aux); free(v); free(dim); free(colors); free(dl); free(dc);
}
int main() {
	int i, j ;
	char *input = malloc(len1 * sizeof(char));
	char *filters = malloc(len2 * sizeof(char));
	char *pooling = malloc(len2 * sizeof(char));
	char *cluster = malloc(len2 * sizeof(char));
	char *output_task1 = malloc(len1 * sizeof(char));
	char *output_task2 = malloc(len1 * sizeof(char));
	char *output_task3 = malloc(len1 * sizeof(char));
	char *output_task4 = malloc(len1 * sizeof(char));
	char *output_task5 = malloc(len1 * sizeof(char));
	if (input == NULL || filters == NULL || pooling == NULL 
		|| cluster == NULL ) 
		printf("Nu s-a reusit alocarea\n");
	if (output_task1 == NULL || output_task2 == NULL || output_task3 == NULL 
		|| output_task4 == NULL || output_task5 == NULL)
		printf("NU s-a reusit alocarea\n");
	pixels **matrix; bmp_fileheader a; bmp_infoheader b; 
	FILE *image, *image1, *image2, *info_filter, *image3, *info_pooling;
	FILE  *image4, *info_cluster, *image5;
	//citim datele de intrare,folosind functia read_input
	read_input(input, filters, pooling, cluster);
	//deschidem fiserul text cu denumirea input
	image = fopen(input, "rt");
	if (image == NULL) {
		printf("Eroare la deschiderea fisierului %s", input);
	}
	//citim cele doua headere asociate imaginii
	fread(&a.fileMarker1, sizeof(unsigned char), 1, image);
	fread(&a.fileMarker2, sizeof(unsigned char), 1, image);
	fread(&a.bfSize, sizeof(unsigned int), 1, image);
	fread(&a.unused1, sizeof(unsigned short), 1, image);
	fread(&a.unused2, sizeof(unsigned short), 1, image);
	fread(&a.imageDataOffset, sizeof(unsigned int), 1, image);
	fread(&b.biSize, sizeof(unsigned int), 1, image);
	fread(&b.width, sizeof(signed int), 1, image);
	fread(&b.height, sizeof(signed int), 1, image);
	fread(&b.planes, sizeof(unsigned short), 1, image);
	fread(&b.bitPix, sizeof(unsigned short), 1, image);
	fread(&b.biCompression, sizeof(unsigned int), 1, image);
	fread(&b.biSizeImage, sizeof(unsigned int), 1, image);
	fread(&b.biXPelsPerMeter, sizeof(int), 1, image);
	fread(&b.biYPelsPerMeter, sizeof(int), 1, image);
	fread(&b.biClrUsed, sizeof(unsigned int), 1, image);
	fread(&b.biClrImportant, sizeof(unsigned int), 1, image);
	//declaram si citim matricea de pixeli asociata imaginii
	matrix = malloc(b.height * sizeof(pixels *));
	if (matrix == NULL)  printf("Nu s-a reusit alocarea\n");
	for (i = 0; i < b.height; i++) {
		matrix[i] = malloc(b.width * sizeof(pixels));
		if (matrix[i] == NULL) printf("Nu s-a reusit alocarea\n");
	}
	for (i = 0; i< b.height; i++) {
		for (j = 0;j <b.width; j++) {  
			fread(&matrix[i][j].blue, sizeof(unsigned char), 1, image);
			fread(&matrix[i][j].green, sizeof(unsigned char), 1, image);
			fread(&matrix[i][j].red, sizeof(unsigned char), 1, image);
		}
		fseek(image, (b.width)%4, 1);//trecem peste padding
	}
	fclose(image);
	//adaugam in sirul image1 denumirea imaginii cerute la task1,de la
	//denumirea imput-ului eliminam extensia "bmp" si adaugam noua
	//denumire specifica task-ului 1 urmata de extensia "bmp"
	strcpy(output_task1, input); output_task1[5] = '\0';
	strcat(output_task1, "_black_white.bmp"); 
	image1 = fopen(output_task1, "wb");
	if (image1 == NULL) {
		printf("Eroare la deschiderea fisierului %s", output_task1);
	}
	black_white(image1, a , b, matrix);//apelam functia care rezolva task1
	fclose(image1);
	strcpy(output_task2, input); output_task2[5] = '\0';
	strcat(output_task2, "_nocrop.bmp"); image2 = fopen(output_task2, "wb");
	if (image2 == NULL) {
		printf("Eroare la deschiderea fisierului %s", output_task2);
	}
	nocrop(image2, a, b, matrix);//apelam functia care rezolva task2
	fclose(image2);
	info_filter = fopen(filters, "rt"); strcpy(output_task3, input);
	if (info_filter == NULL) {
		printf("Eroare la deschiderea fisierului %s", filters);
	}
	output_task3[5] = '\0'; strcat(output_task3, "_filter.bmp");
	image3 = fopen(output_task3, "wb");
	if (image3 == NULL) {
		printf("Eroare la deschiderea fisierului %s", output_task3);
	}
	filter(info_filter, image3, a, b, matrix);//apelam functia care
	fclose(image3); fclose(info_filter);//rezolva task3
	info_pooling = fopen(pooling, "rt"); strcpy(output_task4, input);
	if (info_pooling == NULL) {
		printf("Eroare la deschiderea fisierului %s", pooling);
	}
	output_task4[5] = '\0'; strcat(output_task4, "_pooling.bmp");
	image4 = fopen(output_task4, "wb");
	if (image4 == NULL) {
		printf("Eroare la deschiderea fisierului %s", output_task4);
	}
	pool(info_pooling, image4, a, b, matrix);//apelam functia care
	fclose(image4); fclose(info_pooling);//rezolva task4
	info_cluster = fopen(cluster, "rt"); strcpy(output_task5, input);
	if (info_cluster == NULL) {
		printf("Eroare la deschiderea fisierului %s", cluster);
	}
	output_task5[5] = '\0'; strcat(output_task5, "_clustered.bmp");
	image5 = fopen(output_task5, "wb");
	if (image5 == NULL) {
		printf("Eroare la deschiderea fisierului %s", output_task5);
	}
	clustering(info_cluster, image5, a, b, matrix);//apelam functia care
	fclose(image5); fclose(info_cluster);//rezolva task5
	for (i = 0; i < b.height; i++) free(matrix[i]);//eliberam memoria alocata
	free(matrix); free(input); free(filters); free(pooling); free(cluster);
	free(output_task1); free(output_task2); free(output_task3);
	free(output_task4); free(output_task5);
}