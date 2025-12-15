#ifndef RESERVATION_H_INCLUDED
#define RESERVATION_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RESERVATIONS 1000
#define FICHIER_RESERVATIONS "reservations.txt"

typedef struct
{
    int id;
    int id_membre;
    int id_entraineur;
    char cours[50];
    char jour[20];
    int heure_debut;
    int heure_fin;
    int prive;   
} Reservation;

extern Reservation reservations[MAX_RESERVATIONS];
extern int nb_reservations;
extern int dernier_id_res;

void charger_reservations(void);
void sauvegarder_reservations(void);

void ajouter_reservation(Reservation *r);
void modifier_reservation(int id, Reservation *r);
void supprimer_reservation(int id);

Reservation* rechercher_reservation(int id);
void supprimer_reservations_entraineur(int id_entraineur);
int coach_disponible(int id_entraineur, const char *jour, int hdeb, int hfin);
int membre_disponible(int id_membre, const char *jour, int hdeb, int hfin);

#endif


