#ifndef INSCRIPTION_H
#define INSCRIPTION_H

#define MAX_INSCRIPTIONS 500
#define FICHIER_INSCRIPTIONS "inscriptions.txt"

typedef struct {
    int id_entraineur;
    char cours[50];
    char equipements[100];
    char jour[20];
    int heure_debut;
    int heure_fin;
} InscriptionCours;

InscriptionCours inscriptions[MAX_INSCRIPTIONS];
int nb_inscriptions;

void ajouter_inscription(InscriptionCours *ins);
void charger_inscriptions(void);
void sauvegarder_inscriptions(void);
void supprimer_inscriptions_entraineur(int id_entraineur);

#endif
