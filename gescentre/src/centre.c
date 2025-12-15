#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "centre.h"

#define FICHIER_CENTRES "centres.txt"
#define FICHIER_ENTRAINEURS "entraineurs.txt"
#define FICHIER_INSCRIPTIONS "entraineurs_inscriptions.txt"

// --------------------------------------------------------------
// FONCTION POUR AJOUTER UN CENTRE
// --------------------------------------------------------------
void ajouter_centre(Centre *nouveau_centre) {
    FILE *f = fopen(FICHIER_CENTRES, "a");
    if (f != NULL) {
        fprintf(f, "%d|%s|%s|%s|%d|%s|%s|%d\n",
                nouveau_centre->id_centre,
                nouveau_centre->nom,
                nouveau_centre->adresse,
                nouveau_centre->ville,
                nouveau_centre->capacite,
                nouveau_centre->telephone,
                nouveau_centre->email,
                nouveau_centre->actif);
        fclose(f);
    } else {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", FICHIER_CENTRES);
    }
}

// --------------------------------------------------------------
// FONCTION POUR RECHERCHER UN CENTRE PAR ID
// --------------------------------------------------------------
Centre* rechercher_centre_par_id(int id_centre) {
    static Centre resultat;
    FILE *f = fopen(FICHIER_CENTRES, "r");
    if (f != NULL) {
        char ligne[500];
        while (fgets(ligne, sizeof(ligne), f)) {
            // Supprimer le saut de ligne
            ligne[strcspn(ligne, "\n")] = 0;
            
            if (sscanf(ligne, "%d|%99[^|]|%199[^|]|%49[^|]|%d|%19[^|]|%99[^|]|%d",
                   &resultat.id_centre,
                   resultat.nom,
                   resultat.adresse,
                   resultat.ville,
                   &resultat.capacite,
                   resultat.telephone,
                   resultat.email,
                   &resultat.actif) == 8) {
                if (resultat.id_centre == id_centre) {
                    fclose(f);
                    return &resultat;
                }
            }
        }
        fclose(f);
    }
    return NULL;
}

// --------------------------------------------------------------
// FONCTION POUR RECHERCHER UN CENTRE PAR NOM
// --------------------------------------------------------------
Centre* rechercher_centre_par_nom(const char *nom) {
    static Centre resultat;
    FILE *f = fopen(FICHIER_CENTRES, "r");
    if (f != NULL) {
        char ligne[500];
        while (fgets(ligne, sizeof(ligne), f)) {
            // Supprimer le saut de ligne
            ligne[strcspn(ligne, "\n")] = 0;
            
            if (sscanf(ligne, "%d|%99[^|]|%199[^|]|%49[^|]|%d|%19[^|]|%99[^|]|%d",
                   &resultat.id_centre,
                   resultat.nom,
                   resultat.adresse,
                   resultat.ville,
                   &resultat.capacite,
                   resultat.telephone,
                   resultat.email,
                   &resultat.actif) == 8) {
                // Comparaison insensible à la casse
                if (strcasecmp(resultat.nom, nom) == 0) {
                    fclose(f);
                    return &resultat;
                }
            }
        }
        fclose(f);
    }
    return NULL;
}

// --------------------------------------------------------------
// FONCTION POUR MODIFIER UN CENTRE
// --------------------------------------------------------------
void modifier_centre(int id_centre, Centre *centre_modifie) {
    FILE *f = fopen(FICHIER_CENTRES, "r");
    if (f == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s en lecture\n", FICHIER_CENTRES);
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) {
        fclose(f);
        printf("Erreur: Impossible de créer le fichier temporaire\n");
        return;
    }
    
    char ligne[500];
    int trouve = 0;
    
    while (fgets(ligne, sizeof(ligne), f)) {
        ligne[strcspn(ligne, "\n")] = 0;
        
        Centre centre_actuel;
        if (sscanf(ligne, "%d|%99[^|]|%199[^|]|%49[^|]|%d|%19[^|]|%99[^|]|%d",
                   &centre_actuel.id_centre,
                   centre_actuel.nom,
                   centre_actuel.adresse,
                   centre_actuel.ville,
                   &centre_actuel.capacite,
                   centre_actuel.telephone,
                   centre_actuel.email,
                   &centre_actuel.actif) == 8) {
            
            if (centre_actuel.id_centre == id_centre) {
                // Écrire le centre modifié
                fprintf(temp, "%d|%s|%s|%s|%d|%s|%s|%d\n",
                        centre_modifie->id_centre,
                        centre_modifie->nom,
                        centre_modifie->adresse,
                        centre_modifie->ville,
                        centre_modifie->capacite,
                        centre_modifie->telephone,
                        centre_modifie->email,
                        centre_modifie->actif);
                trouve = 1;
            } else {
                // Copier l'ancien centre
                fprintf(temp, "%s\n", ligne);
            }
        } else {
            // Copier la ligne même si le format est incorrect
            fprintf(temp, "%s\n", ligne);
        }
    }
    
    fclose(f);
    fclose(temp);
    
    if (trouve) {
        remove(FICHIER_CENTRES);
        rename("temp.txt", FICHIER_CENTRES);
        printf("Centre modifié avec succès (ID: %d)\n", id_centre);
    } else {
        remove("temp.txt");
        printf("Erreur: Centre non trouvé (ID: %d)\n", id_centre);
    }
}

// --------------------------------------------------------------
// FONCTION POUR SUPPRIMER UN CENTRE
// --------------------------------------------------------------
void supprimer_centre(int id_centre) {
    FILE *f = fopen(FICHIER_CENTRES, "r");
    if (f == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", FICHIER_CENTRES);
        return;
    }
    
    FILE *temp = fopen("temp.txt", "w");
    if (temp == NULL) {
        fclose(f);
        printf("Erreur: Impossible de créer le fichier temporaire\n");
        return;
    }
    
    char ligne[500];
    int trouve = 0;
    
    while (fgets(ligne, sizeof(ligne), f)) {
        ligne[strcspn(ligne, "\n")] = 0;
        
        Centre centre_actuel;
        if (sscanf(ligne, "%d|%99[^|]|%199[^|]|%49[^|]|%d|%19[^|]|%99[^|]|%d",
                   &centre_actuel.id_centre,
                   centre_actuel.nom,
                   centre_actuel.adresse,
                   centre_actuel.ville,
                   &centre_actuel.capacite,
                   centre_actuel.telephone,
                   centre_actuel.email,
                   &centre_actuel.actif) == 8) {
            
            if (centre_actuel.id_centre != id_centre) {
                fprintf(temp, "%s\n", ligne);
            } else {
                trouve = 1;
            }
        } else {
            fprintf(temp, "%s\n", ligne);
        }
    }
    
    fclose(f);
    fclose(temp);
    
    if (trouve) {
        remove(FICHIER_CENTRES);
        rename("temp.txt", FICHIER_CENTRES);
        printf("Centre supprimé avec succès (ID: %d)\n", id_centre);
    } else {
        remove("temp.txt");
        printf("Erreur: Centre non trouvé (ID: %d)\n", id_centre);
    }
}

// --------------------------------------------------------------
// FONCTION POUR OBTENIR LE NOMBRE TOTAL DE CENTRES
// --------------------------------------------------------------
int obtenir_nombre_centres() {
    FILE *f = fopen(FICHIER_CENTRES, "r");
    int count = 0;
    if (f != NULL) {
        char ligne[500];
        while (fgets(ligne, sizeof(ligne), f)) {
            count++;
        }
        fclose(f);
    }
    return count;
}

// --------------------------------------------------------------
// FONCTION POUR "AFFICHER" TOUS LES CENTRES
// --------------------------------------------------------------
void afficher_tous_centres() {
    FILE *f = fopen(FICHIER_CENTRES, "r");
    if (f != NULL) {
        char ligne[500];
        int count = 1;
        printf("\n=== LISTE DES CENTRES ===\n");
        while (fgets(ligne, sizeof(ligne), f)) {
            ligne[strcspn(ligne, "\n")] = 0;
            
            Centre centre;
            if (sscanf(ligne, "%d|%99[^|]|%199[^|]|%49[^|]|%d|%19[^|]|%99[^|]|%d",
                       &centre.id_centre,
                       centre.nom,
                       centre.adresse,
                       centre.ville,
                       &centre.capacite,
                       centre.telephone,
                       centre.email,
                       &centre.actif) == 8) {
                
                printf("\nCentre #%d\n", count++);
                printf("  ID: %d\n", centre.id_centre);
                printf("  Nom: %s\n", centre.nom);
                printf("  Ville: %s\n", centre.ville);
                printf("  Adresse: %s\n", centre.adresse);
                printf("  Capacité: %d\n", centre.capacite);
                printf("  Téléphone: %s\n", centre.telephone);
                printf("  Email: %s\n", centre.email);
                printf("  Statut: %s\n", centre.actif ? "Actif" : "Inactif");
            }
        }
        fclose(f);
        printf("\n=======================\n");
    } else {
        printf("Aucun centre enregistré.\n");
    }
}

// --------------------------------------------------------------
// FONCTION POUR AJOUTER UN ENTRAINEUR À UN CENTRE
// --------------------------------------------------------------
void ajouter_entraineur_centre(int id_centre, int id_entraineur, const char *specialite, float note) {
    // À implémenter si nécessaire
    printf("Fonction ajouter_entraineur_centre appelée (centre: %d, entraîneur: %d)\n", 
           id_centre, id_entraineur);
}

// --------------------------------------------------------------
// FONCTION D'INSCRIPTION D'UN ENTRAINEUR
// --------------------------------------------------------------
void sinscrire(int id_entraineur, const char *nom, const char *prenom,
               const char *centre, const char *specialite, float note) 
{
    FILE *f = fopen(FICHIER_ENTRAINEURS, "a");
    if (f == NULL) {
        printf("Erreur: Impossible d'ouvrir le fichier %s\n", FICHIER_ENTRAINEURS);
        return;
    }

    fprintf(f, "%d;%s;%s;%s;%s;%.1f\n",
            id_entraineur, nom, prenom, centre, specialite, note);

    fclose(f);
    printf("Inscription réussie pour l'entraîneur %s %s\n", prenom, nom);
}

// --------------------------------------------------------------
// FONCTION UTILITAIRE: VÉRIFIER SI UN CENTRE EXISTE
// --------------------------------------------------------------
int centre_existe(int id) {
    return (rechercher_centre_par_id(id) != NULL);
}

// --------------------------------------------------------------
// FONCTION UTILITAIRE: OBTENIR LE PROCHAIN ID DISPONIBLE
// --------------------------------------------------------------
int obtenir_prochain_id(void) {
    int max_id = 0;
    FILE *f = fopen(FICHIER_CENTRES, "r");
    
    if (f != NULL) {
        char ligne[500];
        Centre centre;
        
        while (fgets(ligne, sizeof(ligne), f)) {
            ligne[strcspn(ligne, "\n")] = 0;
            
            if (sscanf(ligne, "%d|%99[^|]|%199[^|]|%49[^|]|%d|%19[^|]|%99[^|]|%d",
                       &centre.id_centre,
                       centre.nom,
                       centre.adresse,
                       centre.ville,
                       &centre.capacite,
                       centre.telephone,
                       centre.email,
                       &centre.actif) == 8) {
                if (centre.id_centre > max_id) {
                    max_id = centre.id_centre;
                }
            }
        }
        fclose(f);
    }
    
    return max_id + 1;
}

// --------------------------------------------------------------
// FONCTION UTILITAIRE: LISTER LES CENTRES (pour l'interface)
// --------------------------------------------------------------
void lister_centres(void) {
    afficher_tous_centres(); // Alias pour conserver la compatibilité
}
