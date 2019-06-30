#define _CRT_SECURE_NO_WARNINGS
#define  MAX 512
#pragma warning(disable : 4996) 

//TODO: REPAIR INSERT FUNCTION, LOGS;

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <Windows.h>
#include <stdarg.h>
#include "sqlite3.h"

int n; //nr de angajati
FILE* log_file;
char* sql = new char[350];
char *zErrMsg=0;
int rc, id=3;

typedef int (TIP_PF)(const void*, const void*);

sqlite3* opendb(sqlite3 *db) {
	rc = sqlite3_open("Database.db", &db);

	if (rc) {
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
		return 0;
	}
	else {
		printf("Database opened succesfully!\n");
		return db;
	}
}

static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");

	return 0;
}

void CreateTable(sqlite3* db) {
	strcpy(sql,"CREATE TABLE IF NOT EXISTS ANGAJATI("\
		"ID INT IDENTITY(1,1) PRIMARY KEY	NOT NULL,"\
		"NUME TEXT			NOT NULL,"\
		"PRENUME TEXT		NOT NULL,"\
		"CNP TEXT			NOT NULL,"\
		"FUNCTIE TEXT		NOT NULL,"\
		"SALAR INT			NOT NULL);");

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		printf("Tabel created succesfully!\n");
	}
}

void Select(sqlite3* db) {
	strcpy(sql, "SELECT * FROM ANGAJATI");

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		printf("SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else {
		printf("Operation done succesfully\n");
	}
}

void Update(sqlite3* db, char nume[30], char prenume[30], char cnp[14], char functie[30], int salar, char cheie[30]) {
	sprintf(sql, "UPDATE ANGAJATI SET NUME = '%s', prenume= '%s', CNP='%s', functie= '%s', salar=%d WHERE ID = %s;", nume, prenume, cnp, functie, salar, cheie);

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		printf("SQL error: %s", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
		printf("Operation done succesfully!\n");
}

struct Angajat {
	char nume[30];
	char prenume[30];
	char CNP[14];
	char functie[30];
	int salar;
};

void LogEvent(FILE* log_file, char msg[100]) {
	char dt[20];
	struct tm *tm;
	time_t current_time= time(0);
	tm = localtime(&current_time);

	strftime(dt, sizeof(dt), "%d-%m-%Y %H:%M:%S", tm); //converteste data in string
	fprintf(log_file, "%s %s\n", dt, msg);

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

void medie_salar(sqlite3* db) {
	sprintf(sql, "SELECT AVG(SALAR) FROM ANGAJATI;");

	rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		printf("SQL error: %s", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	else
		printf("Opeation done succesfully!\n");
}
															
int main() {
	LARGE_INTEGER start, stop, frequency;
	Angajat vAngajati[100], nou;
	char cheie[14];
	int prag;
	char msg[50];
	int meniu, submeniu;
	sqlite3* db=NULL;
	
	db=opendb(db);
	CreateTable(db);
		
	log_file = fopen("C:\\Users\\adic8\\Desktop\\logs.txt", "a");
	if (log_file == NULL) {
		printf("Eroare la deschiderea fisierului pentru log-uri!\n");
		system("pause");
		return -1;
	}
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
		printf("8. Iesire\n");
		printf("Introduceti optiunea:\n");
		scanf("%d", &meniu);

		switch (meniu) {

		case 1:
			//afisare angajati
			do {
				system("cls");
				printf("1. Toti angajatii\n");
				printf("2. Angajatii cu salar peste un anumit prag\n");
				printf("3. Inapoi\n");
				printf("Introduceti optiunea:");
				scanf("%d", &submeniu);

				switch (submeniu) {
				case 1:
					Select(db);
					system("pause");
					break;

				case 2:
					printf("Introduceti pragul:");
					scanf("%d", &prag);
					
					sprintf(sql, "SELECT * FROM ANGAJATI WHERE SALAR>%d", prag);

					rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

					if (rc != SQLITE_OK) {
						printf("SQL error: %s\n", zErrMsg);
						sqlite3_free(zErrMsg);
					}
					else {
						printf("Operation done succesfully\n");
					}

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

			sprintf(sql, "INSERT INTO ANGAJATI VALUES (%d, '%s' , '%s' , '%s' , '%s' ,%d);", ++id, nou.nume, nou.prenume, nou.CNP, nou.functie, nou.salar);

			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

			if (rc != SQLITE_OK) {
				printf("SQL error: %s", zErrMsg);
				sqlite3_free(zErrMsg);
			}
			else {
				printf("Operation done succesfully!\n");
			}

			system("pause");
			break;

		case 3:
			//stergere
			printf("Introduceti ID-ul angajatului pe care doriti sa-l stergeti:");
			scanf("%s", &cheie);
			
			sprintf(sql, "DELETE FROM ANGAJATI WHERE ID=%s;",cheie);

			rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

			if (rc != SQLITE_OK) {
				printf("SQL error: %s", zErrMsg);
				sqlite3_free(zErrMsg);
			}
			else
				printf("Operation done succesfully!\n");

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
					
					sprintf(sql, "SELECT * FROM ANGAJATI WHERE CNP = %s;", cheie);

					QueryPerformanceCounter(&start);
					rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
					QueryPerformanceCounter(&stop);
					QueryPerformanceFrequency(&frequency);

					if (rc != SQLITE_OK) {
						printf("SQL error: %s", zErrMsg);
						sqlite3_free(zErrMsg);
					}
					else {
						printf("\nOperation done succesfully!\n");
					}
					
					printf("\nQuery time: %f(ms)\n\n", 1000 * (stop.QuadPart - start.QuadPart) / ((float)frequency.QuadPart));

					system("pause");
					break;

				case 2:
					//cautare dupa nume
					printf("Introduceti numele cautat:");
					scanf("%s", &cheie);
					sprintf(sql, "SELECT * FROM ANGAJATI WHERE NUME = '%s';", cheie);

					QueryPerformanceCounter(&start);
					rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
					QueryPerformanceCounter(&stop);
					QueryPerformanceFrequency(&frequency);

					if (rc != SQLITE_OK) {
						printf("SQL error: %s", zErrMsg);
						sqlite3_free(zErrMsg);
					}
					else {
						printf("\nOperation done succesfully!\n");
					}

					printf("\nQuery time: %f(ms)\n\n", 1000 * (stop.QuadPart - start.QuadPart) / ((float)frequency.QuadPart));

					
					system("pause");
					break;

				case 3:
					//inapoi
					break;
				}
			} while (submeniu != 3);
			break;

		case 5:
			//modificare
			printf("Introduceti ID-ul angajatului pe care doriti sa-l modificati:");
			scanf("%s", &cheie);
			
			printf("Nume=");
			scanf("%s", &nou.nume);
			printf("Prenume=");
			scanf("%s", &nou.prenume);
			printf("CNP=");
			scanf("%s", &nou.CNP);
			printf("Functie=");
			scanf("%s", &nou.functie);
			printf("Salar=");
			scanf("%d", &nou.salar);

			Update(db, nou.nume, nou.prenume, nou.CNP, nou.functie, nou.salar, cheie);
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
					//qsort(vAngajati, n, sizeof(Angajat), compara_nume);
					//printf("Angajatii au fost ordonati dupa nume!\n");
					strcpy(sql, "SELECT * FROM ANGAJATI ORDER BY NUME;");

					rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

					if (rc != SQLITE_OK) {
						printf("SQL error: %s\n", zErrMsg);
						sqlite3_free(zErrMsg);
					}
					else {
						printf("Operation done succesfully\n");
					}

					strcpy(msg, "Angajati ordonati dupa nume - crescator");
					LogEvent(log_file, msg);
					system("pause");
					break;

				case 2:
					//descrescator
					
					strcpy(sql, "SELECT * FROM ANGAJATI ORDER BY NUME DESC;");
					rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

					if (rc != SQLITE_OK) {
						printf("SQL error: %s\n", zErrMsg);
						sqlite3_free(zErrMsg);
					}
					else {
						printf("Operation done succesfully\n");
					}

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
			//afisare media salariilor tuturor angajatiilor

			medie_salar(db);
			system("pause");
			break;

		case 8:
			//iesire
			break;
		}//sf switch
	} while (meniu != 8);
	
	sqlite3_close(db);
	return 0;
}
