#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interface.h"
#include "support.h"
#include "callbacks.h"

// Variables globales
static GtkWidget *current_window = NULL;

// ============================================
// FONCTIONS DE GESTION DES FENÊTRES
// ============================================

// Créer et afficher le Menu Principal (première fenêtre)
static void show_menu_principal(void) {
    if (current_window) {
        gtk_widget_destroy(current_window);
    }
    
    current_window = create_Menu_principal();
    if (current_window) {
        gtk_widget_show_all(current_window);
        
        // Connecter le signal de fermeture
        g_signal_connect(G_OBJECT(current_window), "destroy",
                        G_CALLBACK(on_window_destroy), NULL);
        
        printf("Menu Principal affiché\n");
    }
}

// Fonction pour changer de fenêtre selon votre ordre
void navigate_to_window(const char *window_name) {
    if (!current_window) {
        return;
    }
    
    // Cacher la fenêtre courante
    gtk_widget_hide(current_window);
    
    // Créer la nouvelle fenêtre selon l'ordre spécifié
    GtkWidget *new_window = NULL;
    
    if (strcmp(window_name, "Menu_principal") == 0) {
        new_window = create_Menu_principal();
    }
    else if (strcmp(window_name, "selection") == 0) {
        new_window = create_selection();
    }
    else if (strcmp(window_name, "authentification") == 0) {
        new_window = create_authentification();
    }
    else if (strcmp(window_name, "authentification_entraineur") == 0) {
        new_window = create_authentification__entraineur();
    }
    else if (strcmp(window_name, "Gestion_des_centres") == 0) {
        new_window = create_Gestion_des_centres();
    }
    else if (strcmp(window_name, "ajout_centre") == 0) {
        new_window = create_ajout_centre();
    }
    else if (strcmp(window_name, "modifier_centre") == 0) {
        new_window = create_modifier_centre();
    }
    else if (strcmp(window_name, "supprimer") == 0) {
        new_window = create_supprimer();
    }
    else if (strcmp(window_name, "rechercher_centre") == 0) {
        new_window = create_Rechercher_un_centre();
    }
    else if (strcmp(window_name, "inscription") == 0) {
        new_window = create_inscription();
    }
    
    if (new_window) {
        // Détruire l'ancienne fenêtre
        GtkWidget *old_window = current_window;
        current_window = new_window;
        
        // Afficher la nouvelle fenêtre
        gtk_widget_show_all(current_window);
        
        // Connecter le signal de fermeture
        g_signal_connect(G_OBJECT(current_window), "destroy",
                        G_CALLBACK(on_window_destroy), NULL);
        
        // Détruire l'ancienne fenêtre après un court délai
        if (old_window) {
            g_timeout_add(100, (GSourceFunc)gtk_widget_destroy, old_window);
        }
        
        printf("Navigation vers: %s\n", window_name);
    }
}

// Fonction pour revenir en arrière selon votre ordre
void navigate_back(void) {
    if (!current_window) return;
    
    // Déterminer d'où on vient selon l'ordre de navigation
    const char *current_title = gtk_window_get_title(GTK_WINDOW(current_window));
    
    if (strstr(current_title, "Inscription") || 
        strstr(current_title, "Rechercher") ||
        strstr(current_title, "Supprimer") ||
        strstr(current_title, "Modifier") ||
        strstr(current_title, "Ajouter")) {
        // Retour à Gestion des centres
        navigate_to_window("Gestion_des_centres");
    }
    else if (strstr(current_title, "Gestion des centres")) {
        // Retour à Authentification ou Menu Principal selon le mode
        if (is_admin_mode) {
            navigate_to_window("Menu_principal");
        } else {
            navigate_to_window("selection");
        }
    }
    else if (strstr(current_title, "Authentification")) {
        // Retour à Sélection
        navigate_to_window("selection");
    }
    else if (strstr(current_title, "Sélection")) {
        // Retour au Menu Principal
        navigate_to_window("Menu_principal");
    }
    else if (strstr(current_title, "Menu principal")) {
        // Fermer l'application
        quit_application();
    }
    else {
        // Par défaut, retour au Menu Principal
        navigate_to_window("Menu_principal");
    }
}

// ============================================
// FONCTION PRINCIPALE
// ============================================

int main(int argc, char *argv[]) {
    // Initialiser GTK
    gtk_init(&argc, &argv);
    
    // Ajouter le répertoire du programme au chemin de recherche
    add_pixmap_directory(PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");
    
    printf("=== Application Gestion des Centres Sportifs ===\n");
    printf("Ordre de navigation:\n");
    printf("1. Menu Principal\n");
    printf("2. Sélection (Admin/Entraîneur)\n");
    printf("3. Authentification → Gestion Centres → (Ajouter/Modifier/Supprimer/Rechercher)\n");
    printf("4. Authentification Entraîneur → Inscription\n");
    
    // Afficher le Menu Principal en premier (selon votre ordre)
    show_menu_principal();
    
    // Démarrer la boucle principale GTK
    gtk_main();
    
    printf("Arrêt de l'application.\n");
    return 0;
}

// ============================================
// FONCTIONS EXPORTÉES POUR LES CALLBACKS
// ============================================

// Fonction pour quitter l'application proprement
void quit_application(void) {
    if (current_window) {
        gtk_widget_destroy(current_window);
    }
    gtk_main_quit();
}

// Fonction pour obtenir la fenêtre courante
GtkWidget* get_current_window(void) {
    return current_window;
}
