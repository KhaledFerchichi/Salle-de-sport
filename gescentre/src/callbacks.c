#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "centre.h"

// ============================================
// VARIABLES GLOBALES
// ============================================

int is_admin_mode = 0;
int recherche_par_nom = 1;
int current_trainer_id = 0;

// ============================================
// FONCTIONS UTILITAIRES
// ============================================

void show_message(GtkWindow *parent, GtkMessageType type, const char *title, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, type, GTK_BUTTONS_OK, "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void show_info_message(GtkWindow *parent, const char *message) {
    show_message(parent, GTK_MESSAGE_INFO, "Information", message);
}

void show_error_message(GtkWindow *parent, const char *message) {
    show_message(parent, GTK_MESSAGE_ERROR, "Erreur", message);
}

int show_confirm_message(GtkWindow *parent, const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(parent, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION, 
                                               GTK_BUTTONS_YES_NO, "%s", message);
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return (response == GTK_RESPONSE_YES);
}

// ============================================
// FONCTION POUR CHANGER DE FENÊTRE (simple)
// ============================================

void switch_window(GtkWidget *from_window, const char *to_window_name) {
    if (!from_window) return;
    
    // Cacher la fenêtre courante
    gtk_widget_hide(from_window);
    
    GtkWidget *new_window = NULL;
    
    // Créer la nouvelle fenêtre selon votre ordre
    if (strcmp(to_window_name, "Menu_principal") == 0)
        new_window = create_Menu_principal();
    else if (strcmp(to_window_name, "selection") == 0)
        new_window = create_selection();
    else if (strcmp(to_window_name, "authentification") == 0)
        new_window = create_authentification();
    else if (strcmp(to_window_name, "authentification_entraineur") == 0)
        new_window = create_authentification__entraineur();
    else if (strcmp(to_window_name, "Gestion_des_centres") == 0) {
        new_window = create_Gestion_des_centres();
        if (new_window) {
            remplir_treeview_centres(new_window);
        }
    }
    else if (strcmp(to_window_name, "ajout_centre") == 0) {
        new_window = create_ajout_centre();
        if (new_window) {
            GtkWidget *entry_ville = lookup_widget(new_window, "entry_ville_ajout_lc");
            if (entry_ville) {
                gtk_editable_set_editable(GTK_EDITABLE(entry_ville), TRUE);
            }
        }
    }
    else if (strcmp(to_window_name, "modifier_centre") == 0)
        new_window = create_modifier_centre();
    else if (strcmp(to_window_name, "supprimer") == 0)
        new_window = create_supprimer();
    else if (strcmp(to_window_name, "Rechercher_un_centre") == 0)
        new_window = create_Rechercher_un_centre();
    else if (strcmp(to_window_name, "inscription") == 0) {
        new_window = create_inscription();
        if (new_window) {
            initialiser_combobox_specialite(new_window);
        }
    }
    
    if (new_window) {
        gtk_widget_show_all(new_window);
    }
}

// ============================================
// FONCTIONS POUR TREEVIEW ET COMBOBOX
// ============================================

void remplir_treeview_centres(GtkWidget *window) {
    GtkWidget *treeview = lookup_widget(window, "treeview1");
    if (!treeview || !GTK_IS_TREE_VIEW(treeview)) {
        printf("TreeView non trouvé ou incorrect\n");
        return;
    }
    
    // Créer le modèle de données
    GtkListStore *store = gtk_list_store_new(8,
        G_TYPE_INT,      // ID
        G_TYPE_STRING,   // Nom
        G_TYPE_STRING,   // Ville
        G_TYPE_STRING,   // Adresse
        G_TYPE_STRING,   // Téléphone
        G_TYPE_STRING,   // Email
        G_TYPE_INT,      // Capacité
        G_TYPE_STRING    // Statut
    );
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Supprimer les colonnes existantes
    GList *columns = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
    GList *col_iter;
    for (col_iter = columns; col_iter != NULL; col_iter = col_iter->next) {
        gtk_tree_view_remove_column(GTK_TREE_VIEW(treeview), GTK_TREE_VIEW_COLUMN(col_iter->data));
    }
    g_list_free(columns);
    
    // Créer les colonnes
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", 1, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Ville", renderer, "text", 2, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Adresse", renderer, "text", 3, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Téléphone", renderer, "text", 4, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Email", renderer, "text", 5, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Capacité", renderer, "text", 6, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Statut", renderer, "text", 7, NULL);
    gtk_tree_view_column_set_resizable(column, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    // Remplir avec les données
    FILE *f = fopen("centres.txt", "r");
    if (!f) {
        printf("Fichier centres.txt non trouvé\n");
        return;
    }
    
    char line[500];
    GtkTreeIter iter;
    
    while (fgets(line, sizeof(line), f)) {
        Centre centre;
        if (sscanf(line, "%d|%[^|]|%[^|]|%[^|]|%d|%[^|]|%[^|]|%d",
                   &centre.id_centre,
                   centre.nom,
                   centre.adresse,
                   centre.ville,
                   &centre.capacite,
                   centre.telephone,
                   centre.email,
                   &centre.actif) == 8) {
            
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                0, centre.id_centre,
                1, centre.nom,
                2, centre.ville,
                3, centre.adresse,
                4, centre.telephone,
                5, centre.email,
                6, centre.capacite,
                7, centre.actif ? "Actif" : "Inactif",
                -1);
        }
    }
    fclose(f);
    
    printf("TreeView rempli avec succès\n");
}

void initialiser_combobox_specialite(GtkWidget *window) {
    GtkWidget *combobox = lookup_widget(window, "combobox_specialite_lc");
    if (!combobox || !GTK_IS_COMBO_BOX(combobox)) {
        printf("ComboBox non trouvé ou incorrect\n");
        return;
    }
    
    GtkListStore *store = gtk_list_store_new(1, G_TYPE_STRING);
    
    GtkTreeIter iter;
    
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "Fitness", -1);
    
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "cardio", -1);
    
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "Taekwondo", -1);
    
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "prise de poid", -1);
    
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "perte de poid", -1);
    
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "crossfit", -1);
    
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, 0, "APA", -1);
    
    gtk_combo_box_set_model(GTK_COMBO_BOX(combobox), GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combobox), renderer, TRUE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combobox), renderer, "text", 0, NULL);
    
    gtk_widget_set_sensitive(combobox, TRUE);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combobox), 0);
    
    printf("ComboBox initialisé avec succès\n");
}

// ============================================
// CALLBACKS PRINCIPAUX
// ============================================

void on_button_administrateur_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "authentification");
}

void on_button_entraineur_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "authentification_entraineur");
}

void on_check_admin_clicked(GtkButton *button, gpointer user_data) {
    GtkToggleButton *toggle = GTK_TOGGLE_BUTTON(button);
    is_admin_mode = gtk_toggle_button_get_active(toggle);
}

void on_button_connexion_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    
    GtkWidget *entry_login = lookup_widget(window, "entry_login_admin_lc");
    GtkWidget *entry_password = lookup_widget(window, "entry_password_admin_lc");
    
    if (!entry_login || !entry_password) {
        show_error_message(GTK_WINDOW(window), "Champs de connexion non trouvés");
        return;
    }
    
    const char *login = gtk_entry_get_text(GTK_ENTRY(entry_login));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entry_password));
    
    if (strcmp(login, "admin") == 0 && strcmp(password, "admin123") == 0) {
        show_info_message(GTK_WINDOW(window), "Connexion administrateur réussie !");
        switch_window(window, "Gestion_des_centres");
    } else {
        show_error_message(GTK_WINDOW(window), "Identifiants incorrects !");
    }
}

void on_button_connexion_entraineur_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    
    GtkWidget *entry_id = lookup_widget(window, "entry_numero_entaineur_lc");
    GtkWidget *entry_password = lookup_widget(window, "entry_password_entraineur_lc");
    
    if (!entry_id || !entry_password) {
        show_error_message(GTK_WINDOW(window), "Champs de connexion non trouvés");
        return;
    }
    
    const char *id_str = gtk_entry_get_text(GTK_ENTRY(entry_id));
    const char *password = gtk_entry_get_text(GTK_ENTRY(entry_password));
    
    if (strlen(id_str) == 0 || strlen(password) == 0) {
        show_error_message(GTK_WINDOW(window), "ID et mot de passe requis");
        return;
    }
    
    current_trainer_id = atoi(id_str);
    if (current_trainer_id <= 0) current_trainer_id = 100;
    
    show_info_message(GTK_WINDOW(window), "Connexion entraîneur réussie !");
    switch_window(window, "inscription");
}

// ============================================
// MENU PRINCIPAL
// ============================================

void on_button_gestion_centre_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "selection");
}

void on_button_gestion_entraineur_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    show_info_message(GTK_WINDOW(window), "Gestion des entraîneurs - À implémenter");
}

void on_button_gestion_membre_enter(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    show_info_message(GTK_WINDOW(window), "Gestion des membres - À implémenter");
}

void on_button_gestion_cours_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    show_info_message(GTK_WINDOW(window), "Gestion des cours - À implémenter");
}

void on_button_gestion_equipement_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    show_info_message(GTK_WINDOW(window), "Gestion des équipements - À implémenter");
}

// ============================================
// GESTION DES CENTRES
// ============================================

void on_button_ajouter_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "ajout_centre");
}

void on_button_modifier_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "modifier_centre");
}

void on_button_supprimer_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "supprimer");
}

void on_button_rechercher_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Rechercher_un_centre");
}

void on_button_s_inscrire_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "inscription");
}

// TOUS LES BOUTONS RETOUR RAMÈNENT AU MENU PRINCIPAL
void on_button_retour_gestion_lc_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Menu_principal");
}

void on_button_retour_ajout_lc_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Menu_principal");
}

void on_button_retour_modif_lc_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Menu_principal");
}

void on_button_retour_supprimer_lc_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Menu_principal");
}

void on_button_retour_recherche_lc_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Menu_principal");
}

void on_button_retour_inscription_lc_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Menu_principal");
}

void on_button_annuler_ajout_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Menu_principal");
}

void on_button_annuler_modif_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Menu_principal");
}

void on_button_annuler_recherche_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Menu_principal");
}

void on_button_annuler_inscription_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Menu_principal");
}

void on_button_non_supprimer_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    switch_window(window, "Menu_principal");
}

// ============================================
// AJOUT D'UN CENTRE
// ============================================

void on_button_valider_ajout_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    
    GtkWidget *spin_id = lookup_widget(window, "spinbutton_id_ajout_lc");
    GtkWidget *entry_nom = lookup_widget(window, "entry_nom_ajout_lc");
    GtkWidget *entry_adresse = lookup_widget(window, "entry_adresse_ajout_lc");
    GtkWidget *entry_ville = lookup_widget(window, "entry_ville_ajout_lc");
    GtkWidget *entry_telephone = lookup_widget(window, "entry_telephone_ajout_lc");
    GtkWidget *entry_email = lookup_widget(window, "entry_email_ajout_lc");
    GtkWidget *spin_capacite = lookup_widget(window, "spinbutton_capacite_ajout_lc");
    GtkWidget *radio_actif = lookup_widget(window, "radiobutton_actif_ajout");
    
    if (!spin_id || !entry_nom || !entry_adresse || !entry_ville || 
        !entry_telephone || !entry_email || !spin_capacite || !radio_actif) {
        show_error_message(GTK_WINDOW(window), "Widgets non trouvés !");
        return;
    }
    
    int id = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_id));
    const char *nom = gtk_entry_get_text(GTK_ENTRY(entry_nom));
    const char *adresse = gtk_entry_get_text(GTK_ENTRY(entry_adresse));
    const char *ville = gtk_entry_get_text(GTK_ENTRY(entry_ville));
    const char *telephone = gtk_entry_get_text(GTK_ENTRY(entry_telephone));
    const char *email = gtk_entry_get_text(GTK_ENTRY(entry_email));
    int capacite = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_capacite));
    int actif = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_actif)) ? 1 : 0;
    
    if (id <= 0) {
        show_error_message(GTK_WINDOW(window), "ID invalide !");
        return;
    }
    
    if (strlen(nom) == 0) {
        show_error_message(GTK_WINDOW(window), "Nom requis !");
        return;
    }
    
    if (strlen(adresse) == 0) {
        show_error_message(GTK_WINDOW(window), "Adresse requise !");
        return;
    }
    
    if (strlen(ville) == 0) {
        show_error_message(GTK_WINDOW(window), "Ville requise !");
        return;
    }
    
    Centre *centre_existant = rechercher_centre_par_id(id);
    if (centre_existant != NULL) {
        show_error_message(GTK_WINDOW(window), "Un centre avec cet ID existe déjà !");
        return;
    }
    
    Centre nouveau_centre;
    nouveau_centre.id_centre = id;
    strncpy(nouveau_centre.nom, nom, MAX_NOM-1);
    nouveau_centre.nom[MAX_NOM-1] = '\0';
    strncpy(nouveau_centre.adresse, adresse, MAX_ADRESSE-1);
    nouveau_centre.adresse[MAX_ADRESSE-1] = '\0';
    strncpy(nouveau_centre.ville, ville, MAX_VILLE-1);
    nouveau_centre.ville[MAX_VILLE-1] = '\0';
    strncpy(nouveau_centre.telephone, telephone, MAX_TELEPHONE-1);
    nouveau_centre.telephone[MAX_TELEPHONE-1] = '\0';
    strncpy(nouveau_centre.email, email, MAX_EMAIL-1);
    nouveau_centre.email[MAX_EMAIL-1] = '\0';
    nouveau_centre.capacite = capacite;
    nouveau_centre.actif = actif;
    nouveau_centre.nb_entraineurs_inscrits = 0;
    
    ajouter_centre(&nouveau_centre);
    
    show_info_message(GTK_WINDOW(window), "Centre ajouté avec succès !");
    switch_window(window, "Menu_principal");
}

void on_spinbutton_capacite_ajout_changed(GtkEditable *editable, gpointer user_data) {}
void on_spinbutton_id_ajout_delete_text(GtkEditable *editable, gpointer user_data) {}
void on_radiobutton_inactif_ajout_clicked(GtkButton *button, gpointer user_data) {}
void on_radiobutton_actif_ajout_clicked(GtkButton *button, gpointer user_data) {}

// ============================================
// MODIFICATION D'UN CENTRE
// ============================================

void on_spinbutton_id_modif_change_value(GtkEditable *editable, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(editable)));
    
    GtkWidget *spin_id = lookup_widget(window, "spinbutton_id_modif_lc");
    if (!spin_id) return;
    
    int id = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_id));
    
    Centre *centre = rechercher_centre_par_id(id);
    if (centre != NULL) {
        GtkWidget *entry_nom = lookup_widget(window, "entry_nom_modif_lc");
        GtkWidget *entry_adresse = lookup_widget(window, "entry_adresse_modif_lc");
        GtkWidget *entry_ville = lookup_widget(window, "entry_ville_modif_lc");
        GtkWidget *entry_telephone = lookup_widget(window, "entry_telephone_modif_lc");
        GtkWidget *entry_email = lookup_widget(window, "entry_email_modif_lc");
        GtkWidget *spin_capacite = lookup_widget(window, "spinbutton_capacite_modif_lc");
        GtkWidget *radio_actif = lookup_widget(window, "radiobutton_actif_modif");
        GtkWidget *radio_inactif = lookup_widget(window, "radiobutton_inactif_modif");
        
        if (entry_nom) gtk_entry_set_text(GTK_ENTRY(entry_nom), centre->nom);
        if (entry_adresse) gtk_entry_set_text(GTK_ENTRY(entry_adresse), centre->adresse);
        if (entry_ville) gtk_entry_set_text(GTK_ENTRY(entry_ville), centre->ville);
        if (entry_telephone) gtk_entry_set_text(GTK_ENTRY(entry_telephone), centre->telephone);
        if (entry_email) gtk_entry_set_text(GTK_ENTRY(entry_email), centre->email);
        if (spin_capacite) gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_capacite), centre->capacite);
        
        if (radio_actif && radio_inactif) {
            if (centre->actif) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_actif), TRUE);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_inactif), FALSE);
            } else {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_actif), FALSE);
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_inactif), TRUE);
            }
        }
    }
}

void on_button_modifier_centre_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    
    GtkWidget *spin_id = lookup_widget(window, "spinbutton_id_modif_lc");
    GtkWidget *entry_nom = lookup_widget(window, "entry_nom_modif_lc");
    GtkWidget *entry_adresse = lookup_widget(window, "entry_adresse_modif_lc");
    GtkWidget *entry_ville = lookup_widget(window, "entry_ville_modif_lc");
    GtkWidget *entry_telephone = lookup_widget(window, "entry_telephone_modif_lc");
    GtkWidget *entry_email = lookup_widget(window, "entry_email_modif_lc");
    GtkWidget *spin_capacite = lookup_widget(window, "spinbutton_capacite_modif_lc");
    GtkWidget *radio_actif = lookup_widget(window, "radiobutton_actif_modif");
    
    if (!spin_id || !entry_nom || !entry_adresse || !entry_ville || 
        !entry_telephone || !entry_email || !spin_capacite || !radio_actif) {
        show_error_message(GTK_WINDOW(window), "Widgets non trouvés !");
        return;
    }
    
    int id = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_id));
    const char *nom = gtk_entry_get_text(GTK_ENTRY(entry_nom));
    const char *adresse = gtk_entry_get_text(GTK_ENTRY(entry_adresse));
    const char *ville = gtk_entry_get_text(GTK_ENTRY(entry_ville));
    const char *telephone = gtk_entry_get_text(GTK_ENTRY(entry_telephone));
    const char *email = gtk_entry_get_text(GTK_ENTRY(entry_email));
    int capacite = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_capacite));
    int actif = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_actif)) ? 1 : 0;
    
    if (id <= 0) {
        show_error_message(GTK_WINDOW(window), "ID invalide !");
        return;
    }
    
    if (strlen(nom) == 0 || strlen(adresse) == 0 || strlen(ville) == 0) {
        show_error_message(GTK_WINDOW(window), "Champs obligatoires manquants !");
        return;
    }
    
    Centre *centre_existant = rechercher_centre_par_id(id);
    if (centre_existant == NULL) {
        show_error_message(GTK_WINDOW(window), "Centre non trouvé !");
        return;
    }
    
    Centre centre_modifie;
    centre_modifie.id_centre = id;
    strncpy(centre_modifie.nom, nom, MAX_NOM-1);
    centre_modifie.nom[MAX_NOM-1] = '\0';
    strncpy(centre_modifie.adresse, adresse, MAX_ADRESSE-1);
    centre_modifie.adresse[MAX_ADRESSE-1] = '\0';
    strncpy(centre_modifie.ville, ville, MAX_VILLE-1);
    centre_modifie.ville[MAX_VILLE-1] = '\0';
    strncpy(centre_modifie.telephone, telephone, MAX_TELEPHONE-1);
    centre_modifie.telephone[MAX_TELEPHONE-1] = '\0';
    strncpy(centre_modifie.email, email, MAX_EMAIL-1);
    centre_modifie.email[MAX_EMAIL-1] = '\0';
    centre_modifie.capacite = capacite;
    centre_modifie.actif = actif;
    
    modifier_centre(id, &centre_modifie);
    
    show_info_message(GTK_WINDOW(window), "Centre modifié avec succès !");
    switch_window(window, "Menu_principal");
}

void on_radiobutton_actif_modif_clicked(GtkButton *button, gpointer user_data) {}
void on_radiobutton_inactif_modif_clicked(GtkButton *button, gpointer user_data) {}

// ============================================
// SUPPRESSION D'UN CENTRE
// ============================================

void on_button_oui_supprimer_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    
    GtkWidget *spin_id = lookup_widget(window, "spinbutton_id_supprimer_lc");
    if (!spin_id) {
        show_error_message(GTK_WINDOW(window), "Widget ID non trouvé !");
        return;
    }
    
    int id = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_id));
    
    Centre *centre = rechercher_centre_par_id(id);
    if (centre == NULL) {
        show_error_message(GTK_WINDOW(window), "Centre non trouvé !");
        return;
    }
    
    char message[200];
    snprintf(message, sizeof(message), 
             "Voulez-vous vraiment supprimer le centre :\n'%s' (ID: %d) ?", 
             centre->nom, id);
    
    if (show_confirm_message(GTK_WINDOW(window), message)) {
        supprimer_centre(id);
        show_info_message(GTK_WINDOW(window), "Centre supprimé avec succès !");
        switch_window(window, "Menu_principal");
    }
}

void on_spinbutton_id_supprimer_lc_value_changed(GtkSpinButton *spinbutton, gpointer user_data) {}
void on_spinbutton_id_supprimer_lc_changed(GtkEditable *editable, gpointer user_data) {}

// ============================================
// RECHERCHE D'UN CENTRE
// ============================================

void on_button_lancer_recherche_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    
    GtkWidget *entry_recherche = lookup_widget(window, "entry_recherche_lc");
    GtkWidget *radio_nom = lookup_widget(window, "radiobutton_recherche_nom");
    GtkWidget *radio_id = lookup_widget(window, "radiobutton_recherche_ville");
    
    if (!entry_recherche || !radio_nom || !radio_id) {
        show_error_message(GTK_WINDOW(window), "Widgets non trouvés !");
        return;
    }
    
    const char *critere = gtk_entry_get_text(GTK_ENTRY(entry_recherche));
    
    if (strlen(critere) == 0) {
        show_error_message(GTK_WINDOW(window), "Veuillez entrer un critère de recherche !");
        return;
    }
    
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_nom))) {
        recherche_par_nom = 1;
    } else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_id))) {
        recherche_par_nom = 0;
    }
    
    Centre *resultat = NULL;
    char message[300];
    
    if (recherche_par_nom) {
        resultat = rechercher_centre_par_nom(critere);
        if (resultat != NULL) {
            snprintf(message, sizeof(message),
                    "Centre trouvé :\n\n"
                    "ID: %d\n"
                    "Nom: %s\n"
                    "Ville: %s\n"
                    "Adresse: %s\n"
                    "Capacité: %d\n"
                    "Statut: %s",
                    resultat->id_centre,
                    resultat->nom,
                    resultat->ville,
                    resultat->adresse,
                    resultat->capacite,
                    resultat->actif ? "Actif" : "Inactif");
            show_info_message(GTK_WINDOW(window), message);
        } else {
            show_error_message(GTK_WINDOW(window), "Aucun centre trouvé avec ce nom !");
        }
    } else {
        int id = atoi(critere);
        resultat = rechercher_centre_par_id(id);
        if (resultat != NULL) {
            snprintf(message, sizeof(message),
                    "Centre trouvé :\n\n"
                    "ID: %d\n"
                    "Nom: %s\n"
                    "Ville: %s\n"
                    "Adresse: %s\n"
                    "Capacité: %d\n"
                    "Statut: %s",
                    resultat->id_centre,
                    resultat->nom,
                    resultat->ville,
                    resultat->adresse,
                    resultat->capacite,
                    resultat->actif ? "Actif" : "Inactif");
            show_info_message(GTK_WINDOW(window), message);
        } else {
            show_error_message(GTK_WINDOW(window), "Aucun centre trouvé avec cet ID !");
        }
    }
}

void on_radiobutton_recherche_nom_clicked(GtkButton *button, gpointer user_data) {
    recherche_par_nom = 1;
}

void on_radiobutton_recherche_ville_clicked(GtkButton *button, gpointer user_data) {
    recherche_par_nom = 0;
}

// ============================================
// INSCRIPTION ENTRAINEUR
// ============================================

void on_button_valider_inscription_clicked(GtkButton *button, gpointer user_data) {
    GtkWidget *window = GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    
    GtkWidget *entry_centre = lookup_widget(window, "entry_centre_lc");
    GtkWidget *combobox = lookup_widget(window, "combobox_specialite_lc");
    GtkWidget *scale_note = lookup_widget(window, "scale_note_lc");
    GtkWidget *calendar = lookup_widget(window, "calendar_inscription_lc");
    
    if (!entry_centre || !combobox || !scale_note || !calendar) {
        show_error_message(GTK_WINDOW(window), "Widgets non trouvés !");
        return;
    }
    
    const char *nom_centre = gtk_entry_get_text(GTK_ENTRY(entry_centre));
    float note = gtk_range_get_value(GTK_RANGE(scale_note));
    
    char *specialite_text = NULL;
    gint specialite_index = gtk_combo_box_get_active(GTK_COMBO_BOX(combobox));
    
    if (specialite_index >= 0) {
        GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(combobox));
        if (model != NULL) {
            GtkTreeIter iter;
            if (gtk_tree_model_iter_nth_child(model, &iter, NULL, specialite_index)) {
                gtk_tree_model_get(model, &iter, 0, &specialite_text, -1);
            }
        }
    }
    
    if (specialite_text == NULL) {
        specialite_text = g_strdup("Fitness");
    }
    
    guint year, month, day;
    gtk_calendar_get_date(GTK_CALENDAR(calendar), &year, &month, &day);
    
    if (strlen(nom_centre) == 0) {
        show_error_message(GTK_WINDOW(window), "Veuillez entrer le nom du centre !");
        g_free(specialite_text);
        return;
    }
    
    if (specialite_index < 0) {
        show_error_message(GTK_WINDOW(window), "Veuillez sélectionner une spécialité !");
        g_free(specialite_text);
        return;
    }
    
    Centre *centre = rechercher_centre_par_nom(nom_centre);
    if (centre == NULL) {
        show_error_message(GTK_WINDOW(window), "Centre non trouvé !");
        g_free(specialite_text);
        return;
    }
    
    sinscrire(current_trainer_id > 0 ? current_trainer_id : 100, 
              "Dupont", "Jean", nom_centre, specialite_text, note);
    
    char message[200];
    snprintf(message, sizeof(message),
            "Inscription réussie !\n\n"
            "Centre: %s\n"
            "Date: %02d/%02d/%d\n"
            "Spécialité: %s\n"
            "Note: %.1f/5",
            nom_centre, day, month + 1, year, specialite_text, note);
    
    show_info_message(GTK_WINDOW(window), message);
    
    g_free(specialite_text);
    switch_window(window, "Menu_principal");
}

void on_combobox_specialite_lc_move_active(GtkComboBox *combobox, GtkScrollType scroll_type, gpointer user_data) {}
void on_combobox_specialite_lc_changed(GtkComboBox *combobox, gpointer user_data) {}
gboolean on_combobox_specialite_lc_popdown(GtkComboBox *combobox, gpointer user_data) { return FALSE; }
void on_combobox_specialite_lc_popup(GtkComboBox *combobox, gpointer user_data) {}

// ============================================
// GESTION DE FENÊTRE
// ============================================

void on_window_destroy(GtkWidget *widget, gpointer user_data) {
    gtk_main_quit();
}
