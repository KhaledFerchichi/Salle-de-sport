#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "centre.h"

int main() {
    printf("=== TEST COMPLET DU MODULE CENTRE ===\n\n");
    
    // NETTOYER le fichier avant de commencer
    remove("centres.txt");
    
    // 1. Test d'ajout de centres
    printf("1. TEST AJOUT DE CENTRES...\n");
    Centre c1 = {1, "Sport Plus", "123 Rue des Sports", "Paris", 150, "01 45 67 89 10", "contact@sportplus.fr", 1};
    Centre c2 = {2, "Fit City", "456 Avenue Fitness", "Lyon", 200, "04 56 78 90 12", "info@fitcity.fr", 1};
    
    ajouter_centre(&c1);
    ajouter_centre(&c2);
    printf("✅ 2 centres ajoutés\n\n");
    
    // 2. Test affichage
    printf("2. TEST AFFICHAGE...\n");
    afficher_tous_centres();
    
    // 3. Test modification
    printf("3. TEST MODIFICATION...\n");
    Centre modif = {2, "Fit City MODIFIE", "999 Nouvelle Adresse", "Lyon", 250, "04 00 00 00 00", "nouveau@fitcity.fr", 1};
    modifier_centre(2, &modif);
    printf("✅ Centre ID 2 modifié\n\n");
    
    // 4. Test de la fonction indépendante pour ajouter des entraîneurs
    printf("4. TEST AJOUT ENTRAÎNEURS INDÉPENDANT...\n");
    printf("-------------------------------------------\n");
    
    ajouter_entraineur_centre(1, 101, "Fitness", 4.5);
    ajouter_entraineur_centre(1, 102, "Yoga", 4.8);
    ajouter_entraineur_centre(2, 103, "Musculation", 4.2);
    
    printf("✅ 3 entraîneurs ajoutés aux centres\n");
    
    // 5. Vérification finale
    printf("\n5. VÉRIFICATION FINALE...\n");
    printf("📄 CONTENU FINAL DE centres.txt :\n");
    FILE *f_final = fopen("centres.txt", "r");
    if (f_final != NULL) {
        char ligne[200];
        while (fgets(ligne, sizeof(ligne), f_final)) {
            printf("%s", ligne);
        }
        fclose(f_final);
    }
    
    printf("\n=== TEST COMPLET TERMINÉ ===\n");
    return 0;
}  
