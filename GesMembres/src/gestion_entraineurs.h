#ifndef GESTION_ENTRAINEURS_H
#define GESTION_ENTRAINEURS_H

#define MAX_ENTRAINEURS 100
#define FICHIER_ENTRAINEURS "entraineurs.txt"

typedef struct {
    int id;
    char nom[100];
    char prenom[100];
    char sexe[10];
    char num_tel[20];
    char specialite[100];
    char email[100];
    char mot_de_passe[50];
    char centre_sportif[100];
    char jour_dispo[20];
    int heure_debut;
    int heure_fin;
    int prive;
} Entraineur;

Entraineur entraineurs[MAX_ENTRAINEURS];
int nb_entraineurs;
int dernier_id;

void ajouter_entraineur(Entraineur *e);
void modifier_entraineur(int id, Entraineur *e);
Entraineur* rechercher_entraineur(int id);
void supprimer_entraineur(int id);
void charger_entraineurs(void);
void sauvegarder_entraineurs(void);

#endif

