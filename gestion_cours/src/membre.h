#ifndef MEMBRE_H
#define MEMBRE_H

typedef struct {
  char id[50]; // CIN
  char nom[50];
  char prenom[50];
  char sexe[20];
  char email[100];
  char abonnement[50]; // "1 mois", "3 mois", "1 an"
  char sports[50];     // "Tennis", "Natation", etc.
  int jour;
  int mois;
  int annee;
  char password[50];
} Membre;

// Check login credentials
int verifier_login_membre(char *id, char *password);

// Add a member
void ajouter_membre(Membre m);

// Check if member exists
int membre_existe(char *id);

// Search member by ID (returns pointer to allocated Membre, user must free)
Membre *rechercher_membre(char *id);

// Supprimer membre
void supprimer_membre(char *id);

#endif
