#ifndef CENTRE_H
#define CENTRE_H

#define MAX_NOM 100
#define MAX_ADRESSE 200
#define MAX_VILLE 50
#define MAX_TELEPHONE 20
#define MAX_EMAIL 100

typedef struct {
    int id_centre;
    char nom[MAX_NOM];
    char adresse[MAX_ADRESSE];
    char ville[MAX_VILLE];
    int capacite;
    char telephone[MAX_TELEPHONE];
    char email[MAX_EMAIL];
    int actif;
    int nb_entraineurs_inscrits;
} Centre;

/* Fonctions de gestion des centres */
void ajouter_centre(Centre *nouveau_centre);
void modifier_centre(int id, Centre *centre_modifie);
void supprimer_centre(int id);
Centre* rechercher_centre_par_id(int id);
Centre* rechercher_centre_par_nom(const char *nom);
void sinscrire(int id_entraineur, const char *nom, const char *prenom, 
               const char *nom_centre, const char *specialite, float note);

/* Fonctions utilitaires */
int centre_existe(int id);
int obtenir_prochain_id(void);
int obtenir_nombre_centres(void);
void afficher_tous_centres(void);
void lister_centres(void);
void ajouter_entraineur_centre(int id_centre, int id_entraineur, const char *specialite, float note);

#endif /* CENTRE_H */
