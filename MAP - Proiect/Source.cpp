#define _CRT_SECURE_NO_WARNINGS
#define  MAX 512

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <Windows.h>
#include <stdarg.h>

int n; //nr de angajati
FILE* log_file;

typedef int (TIP_PF)(const void*, const void*);

int comparare(const void* a, const void* b) {
	return (*(int*)a) - (*(int*)b);
}

struct Angajat {
	char nume[30];
	char prenume[30];
	char CNP[14];
	char functie[30];
	int salar;
};

int compara_nume(const void* a, const void* b) {
	char* x = (((Angajat*)a)->nume);
	char* y = (((Angajat*)b)->nume);
	
	return strcmp(x, y);
}

void LogEvent(FILE* log_file, char msg[100]) {
	char dt[20];
	struct tm *tm;
	time_t current_time= time(0);
	tm = localtime(&current_time);

	strftime(dt, sizeof(dt), "%d-%m-%Y %H:%M:%S", tm); //converteste data in string
	fprintf(log_file, "%s %s\n", dt, msg);

}

Angajat load_db(FILE* file, Angajat* vAngajati) {
	
	int i;
	fscanf(file, "%d", &n);

	for (i = 0; i < n; i++) {
		fscanf(file, "%s", &vAngajati[i].nume);
		fscanf(file, "%s", &vAngajati[i].prenume);
		fscanf(file, "%s", &vAngajati[i].CNP);
		fscanf(file, "%s", &vAngajati[i].functie);
		fscanf(file, "%d", &vAngajati[i].salar);
	}
	printf("==Baza de date incarcata cu succes==\n");
	
	char msg[30] = "Baza de date a fost incarcata";
	LogEvent(log_file, msg);
	return *vAngajati;
}

Angajat inserare_angajat(Angajat* vAngajati, Angajat nou) {
	
	strcpy(vAngajati[n].nume, nou.nume);
	strcpy(vAngajati[n].prenume, nou.prenume);
	strcpy(vAngajati[n].CNP, nou.CNP);
	strcpy(vAngajati[n].functie, nou.functie);
	vAngajati[n].salar = nou.salar;

	n++;
	char msg[100] = "Angajat adaugat: ";
	strcat(msg, nou.nume);
	strcat(msg, " ");
	strcat(msg, nou.prenume);
	LogEvent(log_file, msg);
	return *vAngajati;
}

Angajat stergere_angajat(Angajat* vAngajati, char cheie[14]) {
	int i, pozitie=0;
	for (i = 0; i < n; i++) {
		if (strcmp(cheie, vAngajati[i].CNP) == 0) {
			pozitie = i;
			break;
		}
	}

	for (i = pozitie; i < n; i++)
		vAngajati[i] = vAngajati[i + 1];
	n--;

	char msg[50] = "Angajat sters:(CNP) ";
	strcat(msg, cheie);
	LogEvent(log_file, msg);
	
	return *vAngajati;
}

Angajat modificare_angajat(Angajat* vAngajati, char cheie[14]) {
	int gasit = 0;
	char msg[100];
	for (int i = 0; i < n; i++)
	{
		if (strcmp(vAngajati[i].CNP, cheie) == 0) {
			printf("Introduceti noul nume:");
			scanf("%s", &vAngajati[i].nume);
			printf("Introduceti noul prenume:");
			scanf("%s", &vAngajati[i].prenume);
			printf("Introduceti noul CNP:");
			scanf("%s", &vAngajati[i].CNP);
			printf("Introduceti noua functie:");
			scanf("%s", &vAngajati[i].functie);
			printf("Introduceti noul salar:");
			scanf("%d", &vAngajati[i].salar);
			gasit = 1;
		}
	}
	if (gasit == 0) {
		printf("Angajatul nu a fost gasit\n");
		strcpy(msg, "Eroare-Modificare angajat inexistent");
		LogEvent(log_file, msg);
		system("pause");
	}
	else {
		strcpy(msg, "Angajat modificat:(CNP) ");
		strcat(msg, cheie);
		LogEvent(log_file, msg);
	}
	
	return *vAngajati;
}

void* cautare_secventiala(const void* cheie, const void* v, size_t n, size_t dim, int comparare(const void*, const void*)) {
	char msg[50];
	for (int i = 0; i < n; i++) {
		Angajat* adrElement = (Angajat*)v + dim * i;
		if (comparare(cheie, adrElement->CNP) == 0) {
			strcpy(msg, "Angajat gasit dupa CNP: ");
			strcat(msg, adrElement->CNP);
			LogEvent(log_file, msg);
			return adrElement;
		}
	}
	strcpy(msg, "Angajat cautat inexistent");
	LogEvent(log_file, msg);
	return NULL;
}

Angajat* cautare_binara(Angajat* v, int inf, int sup, char* cheie) {
	int mijl;
	char msg[50];

	while (inf <= sup) {
		mijl = (inf + sup) / 2;
		if (strcmp(v[mijl].nume, cheie) == 0) {
			strcpy(msg, "Angajat gasit dupa nume: ");
			strcat(msg, v[mijl].nume);
			LogEvent(log_file, msg);
			return &v[mijl];
		}
		else if (strcmp(v[mijl].nume, cheie) < 0)
			inf = mijl + 1;
		else
			sup = mijl - 1;
	}

	strcpy(msg, "Angajat cautat inexistent");
	LogEvent(log_file, msg);
	return NULL;
}

//recursiviate aici
void ordonare_descrescator(Angajat* vAngajati, int n) {
	Angajat aux;

	for (int i = 0; i < n-1; i++) {
		if (strcmp(vAngajati[i].nume, vAngajati[i + 1].nume) < 0) {
			aux = vAngajati[i];
			vAngajati[i] = vAngajati[i + 1];
			vAngajati[i + 1] = aux;
		}
		ordonare_descrescator(vAngajati, n - 1);
	}
}

double medie_salar(Angajat vAngajati[100], int nr_argumente, ...) {
	va_list valist;
	int suma = 0;
	char msg[60];

	va_start(valist, nr_argumente); //initializeaza lista pentru nr argumente

	for (int i = 0; i < nr_argumente; i++)
		suma += va_arg(valist, int);

	va_end(valist);


	sprintf(msg, "Afisare media salariilor a %d angajati", nr_argumente);
	LogEvent(log_file, msg);

	return suma/nr_argumente;
}

void afisare_angajati_salar(Angajat* vAngajati, int prag) {
	char msg[50];

	for (int i = 0; i < n; i++) {
		if (vAngajati[i].salar <= prag)
			continue;
													    
		printf("\nNume:%s\n", vAngajati[i].nume);
		printf("Prenume:%s\n", vAngajati[i].prenume);
		printf("CNP:%s\n", vAngajati[i].CNP);
		printf("Functie:%s\n", vAngajati[i].functie);
		printf("Salar:%d lei\n", vAngajati[i].salar);
		printf("\n\n");
	}

	sprintf(msg, "Afisare angajati cu salar peste pragul de %d", prag);
	LogEvent(log_file, msg);
}

void backup_db(FILE* db_file, FILE* backup_file) {
	char msg[20];
	char String[MAX];

	while (!feof(db_file)) {
		fgets(String, MAX, db_file);
	}

	char* buffer = new char[strlen(String)];

	fwrite(buffer, sizeof(char), strlen(buffer), backup_file);

	strcpy(msg, "Backup efectuat");
	LogEvent(log_file, msg);
	fclose(db_file);
	fclose(backup_file);
}

void restore_db(FILE* backup_file, FILE* db_file) {
	char* buffer = (char*)malloc(MAX * sizeof(char));
	while (!feof(backup_file)) {
		fread(buffer, sizeof(char), MAX, backup_file);
		fprintf(db_file, "%s", buffer);
	}

	fclose(db_file);
	fclose(backup_file);
}
															
void print_db(Angajat* vAngajati) {
	int i;
	
	for (i = 0; i < n; i++) {
		printf("Nume:%s\n", vAngajati[i].nume);
		printf("Prenume:%s\n", vAngajati[i].prenume);
		printf("CNP:%s\n", vAngajati[i].CNP);
		printf("Functie:%s\n", vAngajati[i].functie);
		printf("Salar:%d lei\n", vAngajati[i].salar);
		printf("\n\n");
	}

	char msg[20] = "Baza date afisata";
	LogEvent(log_file, msg);
}

void write_db(FILE* file, Angajat* vAngajati) {

	fprintf(file, "%d\n", n);
	for (int i = 0; i < n; i++) {
		fprintf(file, "%s", vAngajati[i].nume);
		fprintf(file, " %s", vAngajati[i].prenume);
		fprintf(file, " %s", vAngajati[i].CNP);
		fprintf(file, " %s", vAngajati[i].functie);
		fprintf(file, " %d", vAngajati[i].salar);
		if (i < n - 1)
			fprintf(file, "\n");
	}
}

int main() {
	LARGE_INTEGER start, stop, frequency;
	Angajat vAngajati[100], nou;
	char cheie[14];
	int check = 0;
	int prag;
	char msg[50];
	Angajat* adrElement;
	int meniu, submeniu;
	FILE* db_init;
	FILE* backup;
	
	db_init = fopen("BazaDateInit.txt", "rt");

	if (db_init == NULL) {
		check = 1;
		goto ERR;
	}

	if (check == 1) {
		ERR: {
			printf("Eroare la deschiderea bazei de date!\n");
			strcpy(msg, "Eroare la deschiderea bazei de date");
			LogEvent(log_file, msg);
			system("pause");
			return -1;
		}
	}
	

	log_file = fopen("C:\\Users\\adic8\\Desktop\\logs.txt", "a");
	if (log_file == NULL) {
		printf("Eroare la deschiderea fisierului pentru log-uri!\n");
		system("pause");
		return -1;
	}


	//incarca baza de date
	load_db(db_init, vAngajati);
	system("pause");
		

	
	do {
		system("cls");
		printf("1. Afisare angajati\n");
		printf("2. Inserare angajat\n");
		printf("3. Stergere angajat\n");
		printf("4. Cautare angajat\n");
		printf("5. Modificare angajat\n");
		printf("6. Ordonare dupa nume\n");
		printf("7. Afisare medie salar a primilor N angajati\n");
		printf("8. Salvare modificari\n");
		printf("9. Iesire\n");
		printf("Introduceti optiunea:\n");
		scanf("%d", &meniu);

		switch (meniu) {

		case 1:
			//afisare angajati
			do {
				system("cls");
				printf("1. Toti angajatii\n");
				printf("2. Angajatii cu salar peste ...\n");
				printf("3. Inapoi\n");
				printf("Introduceti optiunea:");
				scanf("%d", &submeniu);

				switch (submeniu) {
				case 1:
					//afisare toti angajatiii
					print_db(vAngajati);
					
					system("pause");
					break;

				case 2:
					//afisare angajati cu salar peste...
					printf("Introduceti pragul:");
					scanf("%d", &prag);
					afisare_angajati_salar(vAngajati, prag);
					
					system("pause");
					break;
				}
			} while (submeniu != 3);
			
			break;

		case 2:
			//inserare angajat
			printf("Introduceti informatiile noului angajat:\n");
			printf("Nume:");
			scanf("%s", &nou.nume);

			printf("Prenume:");
			scanf("%s", &nou.prenume);

			printf("CNP:");
			scanf("%s", &nou.CNP);

			printf("Functie:");
			scanf("%s", &nou.functie);

			printf("Salar:");
			scanf("%d", &nou.salar);

			inserare_angajat(vAngajati, nou);

			printf("Angajatul a fost adaugat!\n");
			system("pause");
			break;

		case 3:
			//stergere
			printf("Introduceti CNP-ul angajatului pe care doriti sa-l stergeti:");
			scanf("%s", &cheie);

			stergere_angajat(vAngajati, cheie);
			printf("Angajatul a fost sters!\n");
			system("pause");
			break;

		case 4:
			//cautare
			do {
				system("cls");
				printf("1. Dupa CNP\n");
				printf("2. Dupa nume\n");
				printf("3. Inapoi\n");
				printf("Introduceti optiunea:");
				scanf("%d", &submeniu);

				switch (submeniu) {
				case 1:
					//cautare dupa CNP
					printf("Introduceti CNP-ul cautat:");
					scanf("%s", &cheie);
					
					QueryPerformanceCounter(&start);
					adrElement = (Angajat*)cautare_secventiala(&cheie, vAngajati, n, sizeof(char), comparare);
					QueryPerformanceCounter(&stop);
					QueryPerformanceFrequency(&frequency);


					if (adrElement != NULL) {
						printf("\nNume:%s\n", adrElement->nume);
						printf("Prenume:%s\n", adrElement->prenume);
						printf("CNP:%s\n", adrElement->CNP);
						printf("Functie:%s\n", adrElement->functie);
						printf("Salar:%d\n", adrElement->salar);
						printf("\nAngajat gasit in: %f(ms)\n\n", 1000 * (stop.QuadPart - start.QuadPart) / ((float)frequency.QuadPart));
						system("pause");
					}
					else {
						printf("Eroare - CNP inexistent\n");
						system("pause");
					}

					break;

				case 2:
					//cautare dupa nume
					printf("Introduceti numele cautat:");
					scanf("%s", &cheie);

					QueryPerformanceCounter(&start);
					adrElement = cautare_binara(vAngajati, 0, n, cheie);
					QueryPerformanceCounter(&stop);
					QueryPerformanceFrequency(&frequency);


					if (adrElement != NULL) {
						printf("\nNume:%s\n", adrElement->nume);
						printf("Prenume:%s\n", adrElement->prenume);
						printf("CNP:%s\n", adrElement->CNP);
						printf("Functie:%s\n", adrElement->functie);
						printf("Salar:%d\n", adrElement->salar);
						printf("\nAngajat gasit in: %f(ms)\n\n", 1000 * (stop.QuadPart - start.QuadPart) / ((float)frequency.QuadPart));
						system("pause");
					}
					else {
						printf("Eroare - Nume inexistent\n");
						system("pause");
					}

					break;

				case 3:
					//inapoi
					break;
				}
			} while (submeniu != 3);
			break;

		case 5:
			//modificare
			printf("Introduceti CNP-ul angajatului pe care doriti sa-l modificati:");
			scanf("%s", &cheie);
			modificare_angajat(vAngajati, cheie);
			printf("Angajatul a fost modificat!\n");
			system("pause");
			break;

		case 6:
			//ordonare
			do {
				system("cls");
				printf("1. Crescator\n");
				printf("2. Descrescator\n");
				printf("3. Inapoi\n");
				printf("Introduceti optiunea:");
				scanf("%d", &submeniu);

				switch (submeniu) {
				case 1:
					//crescator
					qsort(vAngajati, n, sizeof(Angajat), compara_nume);
					printf("Angajatii au fost ordonati dupa nume!\n");
					
					strcpy(msg, "Angajati ordonati dupa nume - crescator");
					LogEvent(log_file, msg);
					system("pause");
					break;

				case 2:
					//descrescator
					ordonare_descrescator(vAngajati, n);
					printf("Angajtii au fost ordonati descrescator!\n");

					strcpy(msg, "Angajati ordonati dupa nume - descrescator");
					LogEvent(log_file, msg);
					system("pause");
					break;

				case 3:
					//inapoi
					break;
				}

			} while (submeniu != 3);

			break;

		case 7:
			//afisare medie salar a primilor n angajati
			int nr_angajati;
			printf("Introduceti numarul de angajati:");
			scanf("%d", &nr_angajati);
			printf("Media salariilor: %.2lf\n", medie_salar(vAngajati, nr_angajati, vAngajati[0].salar, vAngajati[1].salar, vAngajati[2].salar));
			system("pause");
			break;

		case 8:
			//salvare
			fclose(db_init);  //inchid fisierul deschis ca citire
			db_init = fopen("BazaDateInit.txt", "wt"); //deschid fisierul ca scriere
			write_db(db_init, vAngajati);
			printf("Modificarile au fost salvate cu succes!\n");
			system("pause");
			fclose(db_init); //inchid fisierul deschis ca scriere
			
			strcpy(msg, "Baza de date salvata");
			LogEvent(log_file, msg);
			break;

		case 9:
			//iesire
			//fclose(db_init);
			//db_init = fopen("BazadateInit.txt", "rb");
			//backup = fopen("C:\\Users\\adic8\\Desktop\\backup_db.txt", "rb");
			//db_init = fopen("BazaDateInit.txt", "wt");
			//backup_db(db_init, backup);
			
			//restore_db(backup, db_init);
			break;
		}//sf switch
	} while (meniu != 9);
	
	return 0;
}
