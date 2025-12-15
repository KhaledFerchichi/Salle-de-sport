#ifndef COURS_H
#define COURS_H

#include <gtk/gtk.h>

typedef struct {
  int id;
  char type[50];
  char nom[50];
  int jour;
  int mois;
  int annee;
  char heure_debut[20];
  char heure_fin[20];
  int places;
  char niveau[50];
  char coach[50];
  char categorie[50];
} Cours;

enum {
  ID_COLUMN,
  TYPE_COLUMN,
  NOM_COLUMN,
  JOUR_COLUMN,
  MOIS_COLUMN,
  ANNEE_COLUMN,
  HEURE_DEBUT_COLUMN,
  HEURE_FIN_COLUMN,
  PLACES_COLUMN,
  NIVEAU_COLUMN,
  COACH_COLUMN,
  CATEGORIE_COLUMN,
  N_COLUMNS
};

// Prototypes
void ajouter_cours(Cours c);
int modifier_cours(int id, Cours c);
int supprimer_cours(int id);
Cours *rechercher_cours(int id);
int id_existe(int id);
void afficher_cours(GtkTreeView *liste);
void afficher_cours_id(GtkTreeView *liste, int id_filter);

#endif
