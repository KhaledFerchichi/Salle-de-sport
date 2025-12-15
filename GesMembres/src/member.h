#ifndef MEMBER_H_INCLUDED
#define MEMBER_H_INCLUDED

#include <stdio.h>
#include <string.h>

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    int  ID;
    char nom[30];
    char prenom[30];
    char gender[10];
    char cours[100];
    int  taille_cm;
    int  poids_kg;
    Date date_naissance;
    char phone[20];
    char email[100];
    char pwd[30];
} Member;

int ajouter(const char *filename, Member m);
int modifier(const char *filename, int id, Member nouv);
int supprimer(const char *filename, int id);
Member chercher(const char *filename, int id);

#endif

