#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gestion_entraineurs.h"
#include "inscription.h"


Entraineur entraineurs[MAX_ENTRAINEURS];
int nb_entraineurs = 0;
int dernier_id = 0;

void ajouter_entraineur(Entraineur *e) {
    if (nb_entraineurs >= MAX_ENTRAINEURS) {
        printf("DEBUG: Maximum d'entraineurs atteint!\n");
        return;
    }
    
    e->id = ++dernier_id;
    entraineurs[nb_entraineurs] = *e;
    nb_entraineurs++;
    sauvegarder_entraineurs();
    
    printf("DEBUG: Entraineur ajouté - ID: %d, Nom: %s %s\n", e->id, e->prenom, e->nom);
}

void ajouter_entraineur_avec_id(Entraineur *e) {
    if (nb_entraineurs >= MAX_ENTRAINEURS) {
        printf("DEBUG: Maximum d'entraineurs atteint!\n");
        return;
    }
    
    if (e->id > dernier_id) {
        dernier_id = e->id;
    }
    entraineurs[nb_entraineurs] = *e;
    nb_entraineurs++;
    sauvegarder_entraineurs();
    
    printf("DEBUG: Entraineur ajouté avec ID: %d, Nom: %s %s, Sexe: %s, Privé: %d\n", 
           e->id, e->prenom, e->nom, e->sexe, e->prive);
}

void modifier_entraineur(int id, Entraineur *e) {
    Entraineur *found = rechercher_entraineur(id);
    if (found != NULL) {
       
        int original_id = found->id;
        *found = *e;
        found->id = original_id;
        sauvegarder_entraineurs();
        printf("DEBUG: Entraineur ID %d modifié\n", id);
    }
}

Entraineur* rechercher_entraineur(int id) {
    int i;
    for (i = 0; i < nb_entraineurs; i++) {
        if (entraineurs[i].id == id) {
            return &entraineurs[i];
        }
    }
    return NULL;
}

void supprimer_entraineur(int id) {
    int i, j;
    for (i = 0; i < nb_entraineurs; i++) {
        if (entraineurs[i].id == id) {
            supprimer_inscriptions_entraineur(id);
            for (j = i; j < nb_entraineurs - 1; j++) {
                entraineurs[j] = entraineurs[j + 1];
            }
            nb_entraineurs--;
            sauvegarder_entraineurs();
            printf("DEBUG: Entraineur ID %d supprimé\n", id);
            return;
        }
    }
}

void sauvegarder_entraineurs(void) {
    FILE *f = fopen(FICHIER_ENTRAINEURS, "w");
    if (f == NULL) {
        printf("DEBUG: Erreur d'ouverture en écriture du fichier %s\n", FICHIER_ENTRAINEURS);
        return;
    }
    
    int i;
    for (i = 0; i < nb_entraineurs; i++) {
        Entraineur *e = &entraineurs[i];
        fprintf(f, "%d|%s|%s|%s|%s|%s|%s|%s|%s|%s|%d|%d|%d\n",
                e->id, e->nom, e->prenom, e->sexe, e->num_tel,
                e->specialite, e->email, e->mot_de_passe,
                e->centre_sportif, e->jour_dispo,
                e->heure_debut, e->heure_fin, e->prive);
    }
    fclose(f);
    printf("DEBUG: %d entraineurs sauvegardés\n", nb_entraineurs);
}

void charger_entraineurs(void) {
    FILE *f = fopen(FICHIER_ENTRAINEURS, "r");
    if (f == NULL) {
        printf("DEBUG: Fichier %s non trouvé, création d'un nouveau fichier\n", FICHIER_ENTRAINEURS);
        return;
    }
    
    nb_entraineurs = 0;
    dernier_id = 0;
    char line[500];
    int ligne_num = 0;
    
    while (fgets(line, sizeof(line), f)) {
        ligne_num++;
        
        line[strcspn(line, "\n")] = 0;
        
        if (strlen(line) == 0) continue;
        Entraineur e;
        memset(&e, 0, sizeof(Entraineur));
        
        char *token;
        char *rest = line;
        int champ = 0;
        
        while ((token = strtok_r(rest, "|", &rest)) && champ < 13) {
            switch(champ) {
                case 0: e.id = atoi(token); break;
                case 1: strncpy(e.nom, token, sizeof(e.nom)-1); break;
                case 2: strncpy(e.prenom, token, sizeof(e.prenom)-1); break;
                case 3: strncpy(e.sexe, token, sizeof(e.sexe)-1); break;
                case 4: strncpy(e.num_tel, token, sizeof(e.num_tel)-1); break;
                case 5: strncpy(e.specialite, token, sizeof(e.specialite)-1); break;
                case 6: strncpy(e.email, token, sizeof(e.email)-1); break;
                case 7: strncpy(e.mot_de_passe, token, sizeof(e.mot_de_passe)-1); break;
                case 8: strncpy(e.centre_sportif, token, sizeof(e.centre_sportif)-1); break;
                case 9: strncpy(e.jour_dispo, token, sizeof(e.jour_dispo)-1); break;
                case 10: e.heure_debut = atoi(token); break;
                case 11: e.heure_fin = atoi(token); break;
                case 12: e.prive = atoi(token); break;
            }
            champ++;
        }
        
        if (champ >= 8) { 
            entraineurs[nb_entraineurs] = e;
            if (e.id > dernier_id) dernier_id = e.id;
            nb_entraineurs++;
            printf("DEBUG: Entraineur chargé - ID: %d, Nom: %s %s\n", e.id, e.prenom, e.nom);
        } else {
            printf("DEBUG: Ligne %d ignorée (champs insuffisants: %d)\n", ligne_num, champ);
        }
    }
    
    fclose(f);
    printf("DEBUG: %d entraineurs chargés, dernier_id=%d\n", nb_entraineurs, dernier_id);
}
