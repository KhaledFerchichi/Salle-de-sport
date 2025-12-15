#include <stdio.h>
#include <string.h>
#include "inscription.h"

InscriptionCours inscriptions[MAX_INSCRIPTIONS];
int nb_inscriptions = 0;

void ajouter_inscription(InscriptionCours *ins) {
    if (nb_inscriptions < MAX_INSCRIPTIONS) {
        inscriptions[nb_inscriptions] = *ins;
        nb_inscriptions++;
        sauvegarder_inscriptions();
    }
}

void charger_inscriptions(void) {
    FILE *f = fopen(FICHIER_INSCRIPTIONS, "r");
    if (f == NULL) return;
    nb_inscriptions = 0;
    char line[300];
    while (fgets(line, 300, f)) {
        InscriptionCours ins;
        if (sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%d|%d",
                   &ins.id_entraineur, ins.cours, ins.equipements, ins.jour,
                   &ins.heure_debut, &ins.heure_fin) == 6) {
            inscriptions[nb_inscriptions] = ins;
            nb_inscriptions++;
        }
    }
    fclose(f);
}

void sauvegarder_inscriptions(void) {
    FILE *f = fopen(FICHIER_INSCRIPTIONS, "w");
    if (f == NULL) return;
    int i;
    for (i = 0; i < nb_inscriptions; i++) {
        fprintf(f, "%d|%s|%s|%s|%d|%d\n",
                inscriptions[i].id_entraineur,
                inscriptions[i].cours,
                inscriptions[i].equipements,
                inscriptions[i].jour,
                inscriptions[i].heure_debut,
                inscriptions[i].heure_fin);
    }
    fclose(f);
}

void supprimer_inscriptions_entraineur(int id_entraineur) {
    int i, j = 0;
    for (i = 0; i < nb_inscriptions; i++) {
        if (inscriptions[i].id_entraineur != id_entraineur) {
            if (i != j) {
                inscriptions[j] = inscriptions[i];
            }
            j++;
        }
    }
    nb_inscriptions = j;
    sauvegarder_inscriptions();
}
