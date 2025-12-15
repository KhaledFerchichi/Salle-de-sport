#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "gestion_entraineurs.h"
#include "inscription.h"

int sexe_aj = 1;      
int sexe_m = 1;       
int prive_aj = 0;     
int prive_m = 0; 

GtkWidget *admin_window = NULL;
GtkWidget *gesen_window = NULL;
GtkWidget *Login_admin = NULL;
GtkWidget *Login_entraineur = NULL;
GtkWidget *Ajouter_window = NULL;
GtkWidget *modifier_window = NULL;
GtkWidget *supp_window = NULL;
GtkWidget *recherche_window = NULL;
GtkWidget *liste_entraineurs_window = NULL;
GtkWidget *inscription_window = NULL;

int id_entraineur_connecte = -1;

void afficher_treeview_entraineurs(GtkWidget *treeview) {
    GtkListStore *store;
    GtkTreeIter iter;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    store = gtk_list_store_new(10,
                              G_TYPE_INT,     
                              G_TYPE_STRING,  
                              G_TYPE_STRING,  
                              G_TYPE_STRING,  
                              G_TYPE_STRING,   
                              G_TYPE_STRING,   
                              G_TYPE_STRING,   
                              G_TYPE_STRING,   
                              G_TYPE_STRING,   
                              G_TYPE_STRING);  
    
   
    GtkTreeViewColumn *col;
    while ((col = gtk_tree_view_get_column(GTK_TREE_VIEW(treeview), 0)) != NULL) {
        gtk_tree_view_remove_column(GTK_TREE_VIEW(treeview), col);
    }
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL);
    gtk_tree_view_column_set_min_width(column, 50);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", 1, NULL);
    gtk_tree_view_column_set_min_width(column, 80);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Prénom", renderer, "text", 2, NULL);
    gtk_tree_view_column_set_min_width(column, 80);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Sexe", renderer, "text", 3, NULL);
    gtk_tree_view_column_set_min_width(column, 60);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Téléphone", renderer, "text", 4, NULL);
    gtk_tree_view_column_set_min_width(column, 100);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Email", renderer, "text", 5, NULL);
    gtk_tree_view_column_set_min_width(column, 120);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Spécialité", renderer, "text", 6, NULL);
    gtk_tree_view_column_set_min_width(column, 100);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Centre", renderer, "text", 7, NULL);
    gtk_tree_view_column_set_min_width(column, 100);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Jour", renderer, "text", 8, NULL);
    gtk_tree_view_column_set_min_width(column, 80);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Horaires", renderer, "text", 9, NULL);
    gtk_tree_view_column_set_min_width(column, 80);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    int i;
    for (i = 0; i < nb_entraineurs; i++) {
        gchar horaires[50];
        snprintf(horaires, sizeof(horaires), "%dh-%dh", 
                entraineurs[i].heure_debut, entraineurs[i].heure_fin);
        
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                          0, entraineurs[i].id,
                          1, entraineurs[i].nom,
                          2, entraineurs[i].prenom,
                          3, entraineurs[i].sexe,
                          4, entraineurs[i].num_tel,
                          5, entraineurs[i].email,
                          6, entraineurs[i].specialite,
                          7, entraineurs[i].centre_sportif,
                          8, entraineurs[i].jour_dispo,
                          9, horaires,
                          -1);
    }
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    printf("DEBUG: %d entraineurs affichés dans le TreeView\n", nb_entraineurs);
}

void afficher_treeview_cours(GtkWidget *treeview) {
    GtkListStore *store;
    GtkTreeIter iter;
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    
    printf("DEBUG: Affichage cours pour entraineur ID=%d\n", id_entraineur_connecte);
    printf("DEBUG: Nombre total d'inscriptions=%d\n", nb_inscriptions);
    
    store = gtk_list_store_new(4,
                              G_TYPE_STRING,   
                              G_TYPE_STRING,   
                              G_TYPE_STRING,   
                              G_TYPE_STRING);  
    GtkTreeViewColumn *col;
    while ((col = gtk_tree_view_get_column(GTK_TREE_VIEW(treeview), 0)) != NULL) {
        gtk_tree_view_remove_column(GTK_TREE_VIEW(treeview), col);
    }
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Cours", renderer, "text", 0, NULL);
    gtk_tree_view_column_set_min_width(column, 80);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Équipements", renderer, "text", 1, NULL);
    gtk_tree_view_column_set_min_width(column, 100);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Jour", renderer, "text", 2, NULL);
    gtk_tree_view_column_set_min_width(column, 70);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
    
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Horaires", renderer, "text", 3, NULL);
    gtk_tree_view_column_set_min_width(column, 70);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
   int i;
    int count = 0;
    for (i = 0; i < nb_inscriptions; i++) {
        printf("DEBUG: Inscription %d - ID entraineur=%d, Cours=%s\n", 
               i, inscriptions[i].id_entraineur, inscriptions[i].cours);
        
        if (inscriptions[i].id_entraineur == id_entraineur_connecte) {
            gchar horaires[50];
            snprintf(horaires, sizeof(horaires), "%dh-%dh", 
                    inscriptions[i].heure_debut, inscriptions[i].heure_fin);
            
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                              0, inscriptions[i].cours,
                              1, inscriptions[i].equipements,
                              2, inscriptions[i].jour,
                              3, horaires,
                              -1);
            count++;
            printf("DEBUG: Cours ajouté au TreeView: %s\n", inscriptions[i].cours);
        }
    }
    
    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    printf("DEBUG: %d cours affichés pour l'entraineur ID=%d\n", count, id_entraineur_connecte);
}

void afficher_message_dialog(GtkWidget *widget, const gchar *message, GtkMessageType type) {
    GtkWidget *parent_window = gtk_widget_get_toplevel(widget);
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        type,
        GTK_BUTTONS_OK,
        "%s", message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}



void
on_btn_admin_en_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
   printf("DEBUG: Bouton Administrateur cliqué!\n");
  
  gesen_window = lookup_widget(objet_graphique, "gesen_window");
  if (gesen_window) {
    gtk_widget_hide(gesen_window);
  }

  Login_admin = create_Login_admin();
  gtk_widget_show(Login_admin);
}

void
on_btn_entraineur_en_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton Entraineur cliqué!\n");
  
  gesen_window = lookup_widget(objet_graphique, "gesen_window");
  if (gesen_window) {
    gtk_widget_hide(gesen_window);
  }
  
  Login_entraineur = create_Login_entraineur();
  gtk_widget_show(Login_entraineur);
}

void
on_btn_quitter_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton Quitter cliqué!\n");
  gtk_main_quit();
}


void
on_btn_cnxad_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton connexion admin cliqué!\n");
  
  GtkWidget *entry_id = lookup_widget(objet_graphique, "entry_id_ad");
  GtkWidget *entry_mdp = lookup_widget(objet_graphique, "entry_mdp_ad");
  
  if (!entry_id || !entry_mdp) {
    printf("DEBUG: Erreur - widgets non trouvés!\n");
    return;
  }
  
  const gchar *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
  const gchar *mdp = gtk_entry_get_text(GTK_ENTRY(entry_mdp));
  
  printf("DEBUG: ID admin='%s', MDP='%s'\n", id_text, mdp);
  
  if (strlen(id_text) == 0) {
    afficher_message_dialog(objet_graphique, "Veuillez entrer un ID!", GTK_MESSAGE_WARNING);
    return;
  }
  
  int id = atoi(id_text);
  printf("DEBUG: Recherche admin avec ID=%d\n", id);
  
  Entraineur *admin = rechercher_entraineur(id);
  
  if (admin != NULL) {
    printf("DEBUG: Admin trouvé - ID: %d, Nom: %s, Prénom: %s\n", 
           admin->id, admin->nom, admin->prenom);
    printf("DEBUG: MDP saisi: '%s', MDP stocké: '%s'\n", mdp, admin->mot_de_passe);
    
    if (g_strcmp0(admin->mot_de_passe, mdp) == 0) {
      printf("DEBUG: Authentification admin réussie!\n");
      
      afficher_message_dialog(objet_graphique, "Connexion réussie!", GTK_MESSAGE_INFO);
      
      Login_admin = lookup_widget(objet_graphique, "Login_admin");
      if (Login_admin) {
        gtk_widget_hide(Login_admin);
      }
      
      admin_window = create_admin_window();
      gtk_widget_show(admin_window);
    } else {
      printf("DEBUG: Mot de passe incorrect!\n");
      afficher_message_dialog(objet_graphique, "Mot de passe incorrect!", GTK_MESSAGE_ERROR);
    }
  } else {
    printf("DEBUG: Aucun admin trouvé avec ID=%d\n", id);
    afficher_message_dialog(objet_graphique, "Aucun admin trouvé avec cet ID!", GTK_MESSAGE_ERROR);
  }
}

void
on_btn_annule_cnxad_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton annuler connexion admin cliqué!\n");
  
  Login_admin = lookup_widget(objet_graphique, "Login_admin");
  if (Login_admin) {
    gtk_widget_hide(Login_admin);
  }
  
  gesen_window = create_gesen_window();
  gtk_widget_show(gesen_window);
}

void on_btn_cnx_en_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
    printf("DEBUG: Bouton connexion entraineur cliqué!\n");
    
    GtkWidget *entry_id = lookup_widget(objet_graphique, "entry_ID_EN_cnx");
    GtkWidget *entry_mdp = lookup_widget(objet_graphique, "entry_mdp_EN_cnx");
    
    if (!entry_id || !entry_mdp) {
        printf("DEBUG: Erreur - widgets entraineur non trouvés!\n");
        return;
    }
    
    const gchar *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
    const gchar *mdp = gtk_entry_get_text(GTK_ENTRY(entry_mdp));
    
    printf("DEBUG: ID entraineur='%s', MDP='%s'\n", id_text, mdp);
    
    if (strlen(id_text) == 0) {
        afficher_message_dialog(objet_graphique, "Veuillez entrer un ID!", GTK_MESSAGE_WARNING);
        return;
    }
    
    int id = atoi(id_text);
    printf("DEBUG: Recherche entraineur avec ID=%d\n", id);
    
    Entraineur *entraineur = rechercher_entraineur(id);
    
    if (entraineur != NULL) {
        printf("DEBUG: Entraineur trouvé - ID: %d, Nom: %s, Prénom: %s\n", 
               entraineur->id, entraineur->nom, entraineur->prenom);
        
        if (g_strcmp0(entraineur->mot_de_passe, mdp) == 0) {
            printf("DEBUG: Connexion entraineur réussie!\n");
            id_entraineur_connecte = entraineur->id;
            
            Login_entraineur = lookup_widget(objet_graphique, "Login_entraineur");
            if (Login_entraineur) {
                gtk_widget_hide(Login_entraineur);
            }
            
            GtkWidget *inscription_window = create_inscription_window();
            
            GtkWidget *treeview_cours = lookup_widget(inscription_window, "treeview_cours");
            if (treeview_cours) {
                afficher_treeview_cours(treeview_cours);
                printf("DEBUG: TreeView cours initialisé\n");
            }
            
            gtk_widget_show(inscription_window);
            
            gchar message[100];
            snprintf(message, sizeof(message), "Connexion réussie! Bienvenue %s %s", 
                    entraineur->prenom, entraineur->nom);
            afficher_message_dialog(objet_graphique, message, GTK_MESSAGE_INFO);
        } else {
            printf("DEBUG: Mot de passe incorrect!\n");
            afficher_message_dialog(objet_graphique, "Mot de passe incorrect!", GTK_MESSAGE_ERROR);
        }
    } else {
        printf("DEBUG: Aucun entraineur trouvé avec ID=%d\n", id);
        afficher_message_dialog(objet_graphique, "Aucun entraineur trouvé avec cet ID!", GTK_MESSAGE_ERROR);
    }
}

void
on_btn_annule_cnx_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton annuler connexion entraineur cliqué!\n");
  
  Login_entraineur = lookup_widget(objet_graphique, "Login_entraineur");
  if (Login_entraineur) {
    gtk_widget_hide(Login_entraineur);
  }
  
  gesen_window = create_gesen_window();
  gtk_widget_show(gesen_window);
}


void
on_btn_ajouter_en_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton Ajouter entraineur cliqué!\n");
  
  admin_window = lookup_widget(objet_graphique, "admin_window");
  if (admin_window) {
    gtk_widget_hide(admin_window);
  }
  
  Ajouter_window = create_Ajouter_window();
  gtk_widget_show(Ajouter_window);
}

void
on_btn_modifier_en_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton Modifier entraineur cliqué!\n");
  
  admin_window = lookup_widget(objet_graphique, "admin_window");
  if (admin_window) {
    gtk_widget_hide(admin_window);
  }
  
  modifier_window = create_modifier_window();
  gtk_widget_show(modifier_window);
}

void
on_btn_supprimer_en_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton Supprimer entraineur cliqué!\n");
  
  admin_window = lookup_widget(objet_graphique, "admin_window");
  if (admin_window) {
    gtk_widget_hide(admin_window);
  }
  
  supp_window = create_supp_window();
  gtk_widget_show(supp_window);
}

void
on_btn_rechercher_en_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton Rechercher entraineur cliqué!\n");
  
  admin_window = lookup_widget(objet_graphique, "admin_window");
  if (admin_window) {
    gtk_widget_hide(admin_window);
  }
  
  recherche_window = create_recherche_window();
  gtk_widget_show(recherche_window);
}

void on_btn_voirliste_en_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
    printf("DEBUG: Bouton Voir liste entraineurs cliqué!\n");
    
    admin_window = lookup_widget(objet_graphique, "admin_window");
    if (admin_window) {
        gtk_widget_hide(admin_window);
    }
    
    liste_entraineurs_window = create_liste_entraineurs_window();
    
    GtkWidget *treeview = lookup_widget(liste_entraineurs_window, "treeview_entraineurs");
    if (treeview) {
        afficher_treeview_entraineurs(treeview);
    } else {
        printf("DEBUG: ERREUR - treeview_entraineurs non trouvé!\n");
    }
    
    gtk_widget_show(liste_entraineurs_window);
}

void
on_btn_retour_admin_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton Retour admin cliqué!\n");
  
  admin_window = lookup_widget(objet_graphique, "admin_window");
  if (admin_window) {
    gtk_widget_hide(admin_window);
  }
  
  Login_admin = create_Login_admin();
  gtk_widget_show(Login_admin);
}
void on_btn_ajoute_en_clicked(GtkWidget *objet_graphique, gpointer user_data)
{
    printf("DEBUG: Bouton Ajouter cliqué!\n");
    
    GtkWidget *Ajouter_window = lookup_widget(objet_graphique, "Ajouter_window");
    if (!Ajouter_window) {
        printf("DEBUG: Erreur - Ajouter_window non trouvée!\n");
        return;
    }
    
    GtkWidget *entry_id, *entry_nom, *entry_prenom, *entry_num, *entry_email, *entry_mdp;
    GtkWidget *com_specialite, *com_centre, *com_jour;
    GtkWidget *spin_hd, *spin_hf;
    GtkWidget *radio_homme, *radio_femme;
    GtkWidget *check_oui, *check_non;
    
    entry_id = lookup_widget(Ajouter_window, "entry_id_en_aj");
    entry_nom = lookup_widget(Ajouter_window, "entry_nom_aj");
    entry_prenom = lookup_widget(Ajouter_window, "entry_prenom_aj");
    entry_num = lookup_widget(Ajouter_window, "entry_num_aj");
    entry_email = lookup_widget(Ajouter_window, "entry_email_aj");
    entry_mdp = lookup_widget(Ajouter_window, "entry_mdp_aj");
    com_specialite = lookup_widget(Ajouter_window, "com_specialite_aj");
    com_centre = lookup_widget(Ajouter_window, "com_center_aj");
    com_jour = lookup_widget(Ajouter_window, "com_jour_aj");
    spin_hd = lookup_widget(Ajouter_window, "spin_hd_aj");
    spin_hf = lookup_widget(Ajouter_window, "spin_hf_aj");
    radio_homme = lookup_widget(Ajouter_window, "radio_homme_aj");
    radio_femme = lookup_widget(Ajouter_window, "radio_femme_aj");
    check_oui = lookup_widget(Ajouter_window, "check_oui_aj");
    check_non = lookup_widget(Ajouter_window, "check_non__aj");
    
    const gchar *id_text = entry_id ? gtk_entry_get_text(GTK_ENTRY(entry_id)) : "";
    const gchar *nom = entry_nom ? gtk_entry_get_text(GTK_ENTRY(entry_nom)) : "";
    const gchar *prenom = entry_prenom ? gtk_entry_get_text(GTK_ENTRY(entry_prenom)) : "";
    const gchar *num = entry_num ? gtk_entry_get_text(GTK_ENTRY(entry_num)) : "";
    const gchar *email = entry_email ? gtk_entry_get_text(GTK_ENTRY(entry_email)) : "";
    const gchar *mdp = entry_mdp ? gtk_entry_get_text(GTK_ENTRY(entry_mdp)) : "";
    
    if (strlen(id_text) == 0 || strlen(nom) == 0 || strlen(prenom) == 0) {
        afficher_message_dialog(objet_graphique, "Veuillez remplir tous les champs!", GTK_MESSAGE_WARNING);
        return;
    }
    
    int id = atoi(id_text);
    
    if (rechercher_entraineur(id) != NULL) {
        afficher_message_dialog(objet_graphique, "Cet ID existe déjà !", GTK_MESSAGE_ERROR);
        return;
    }
  
    Entraineur nouvel_entraineur;
    nouvel_entraineur.id = id;
    strcpy(nouvel_entraineur.nom, nom);
    strcpy(nouvel_entraineur.prenom, prenom);
    strcpy(nouvel_entraineur.num_tel, num);
    strcpy(nouvel_entraineur.email, email);
    strcpy(nouvel_entraineur.mot_de_passe, strlen(mdp) > 0 ? mdp : "1234");
    
    if (sexe_aj == 1)
        strcpy(nouvel_entraineur.sexe, "Homme");
    else
        strcpy(nouvel_entraineur.sexe, "Femme");
    
    nouvel_entraineur.prive = prive_aj;
    
    if (com_specialite) {
        gchar *spec = gtk_combo_box_get_active_text(GTK_COMBO_BOX(com_specialite));
        strcpy(nouvel_entraineur.specialite, (spec && strlen(spec) > 0) ? spec : "Fitness");
        if (spec) g_free(spec);
    } else {
        strcpy(nouvel_entraineur.specialite, "Fitness");
    }
    
    if (com_centre) {
        gchar *centre = gtk_combo_box_get_active_text(GTK_COMBO_BOX(com_centre));
        strcpy(nouvel_entraineur.centre_sportif, (centre && strlen(centre) > 0) ? centre : "Centre Principal");
        if (centre) g_free(centre);
    } else {
        strcpy(nouvel_entraineur.centre_sportif, "Centre Principal");
    }
    
    if (com_jour) {
        gchar *jour = gtk_combo_box_get_active_text(GTK_COMBO_BOX(com_jour));
        strcpy(nouvel_entraineur.jour_dispo, (jour && strlen(jour) > 0) ? jour : "Lundi");
        if (jour) g_free(jour);
    } else {
        strcpy(nouvel_entraineur.jour_dispo, "Lundi");
    }
    
    if (spin_hd)
        nouvel_entraineur.heure_debut = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_hd));
    else
        nouvel_entraineur.heure_debut = 8;
    
    if (spin_hf)
        nouvel_entraineur.heure_fin = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_hf));
    else
        nouvel_entraineur.heure_fin = 18;
    
    ajouter_entraineur_avec_id(&nouvel_entraineur);
   
    gchar message[100];
    snprintf(message, sizeof(message), "Entraineur ajouté avec succès! ID: %d", nouvel_entraineur.id);
    afficher_message_dialog(objet_graphique, message, GTK_MESSAGE_INFO);
  
    if (entry_id) gtk_entry_set_text(GTK_ENTRY(entry_id), "");
    if (entry_nom) gtk_entry_set_text(GTK_ENTRY(entry_nom), "");
    if (entry_prenom) gtk_entry_set_text(GTK_ENTRY(entry_prenom), "");
    if (entry_num) gtk_entry_set_text(GTK_ENTRY(entry_num), "");
    if (entry_email) gtk_entry_set_text(GTK_ENTRY(entry_email), "");
    if (entry_mdp) gtk_entry_set_text(GTK_ENTRY(entry_mdp), "");

    if (spin_hd) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hd), 0.0);
        gtk_spin_button_update(GTK_SPIN_BUTTON(spin_hd));
    }
    if (spin_hf) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hf), 0.0);
        gtk_spin_button_update(GTK_SPIN_BUTTON(spin_hf));
    }
    
    if (com_specialite) gtk_combo_box_set_active(GTK_COMBO_BOX(com_specialite), -1);
    if (com_centre) gtk_combo_box_set_active(GTK_COMBO_BOX(com_centre), -1);
    if (com_jour) gtk_combo_box_set_active(GTK_COMBO_BOX(com_jour), -1);
    
    sexe_aj = 1;
    prive_aj = 0;
    
    if (radio_homme) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_homme), TRUE);
     
    if (check_non) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_non), TRUE);
    
    printf("DEBUG: Tous les champs ont été réinitialisés!\n");
}

void
on_btn_annule_aj_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton Annuler ajout cliqué!\n");
  
  Ajouter_window = lookup_widget(objet_graphique, "Ajouter_window");
  if (Ajouter_window) {
    gtk_widget_hide(Ajouter_window);
  }
  
  admin_window = create_admin_window();
  gtk_widget_show(admin_window);
}

void
on_radio_femme_aj_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  printf("DEBUG: Radio Femme (ajout) toggled!\n");
  if (gtk_toggle_button_get_active(togglebutton)) {
    sexe_aj = 2; 
    printf("DEBUG: Sexe ajout = Femme\n");
  }
}

void
on_radio_homme_aj_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  printf("DEBUG: Radio Homme (ajout) toggled!\n");
  if (gtk_toggle_button_get_active(togglebutton)) {
    sexe_aj = 1;  
    printf("DEBUG: Sexe ajout = Homme\n");
  }
}

void
on_radio_femme_m_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  printf("DEBUG: Radio Femme (modification) toggled!\n");
  if (gtk_toggle_button_get_active(togglebutton)) {
    sexe_m = 2;  
    printf("DEBUG: Sexe modification = Femme\n");
  }
}

void
on_radio_homme_m_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  printf("DEBUG: Radio Homme (modification) toggled!\n");
  if (gtk_toggle_button_get_active(togglebutton)) {
    sexe_m = 1; 
    printf("DEBUG: Sexe modification = Homme\n");
  }
}

void
on_check_oui_aj_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  printf("DEBUG: Check Oui (ajout) toggled!\n");
  if (gtk_toggle_button_get_active(togglebutton)) {
    prive_aj = 1; 
    printf("DEBUG: Privé ajout = OUI\n");
  }
}

void
on_check_non__aj_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  printf("DEBUG: Check Non (ajout) toggled!\n");
  if (gtk_toggle_button_get_active(togglebutton)) {
    prive_aj = 0;  
    printf("DEBUG: Privé ajout = NON\n");
  }
}

void
on_check_oui_m_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  printf("DEBUG: Check Oui (modification) toggled!\n");
  if (gtk_toggle_button_get_active(togglebutton)) {
    prive_m = 1;
    printf("DEBUG: Privé modification = OUI\n");
  }
}

void
on_check_non_m_toggled (GtkToggleButton *togglebutton, gpointer user_data)
{
  printf("DEBUG: Check Non (modification) toggled!\n");
  if (gtk_toggle_button_get_active(togglebutton)) {
    prive_m = 0;  
    printf("DEBUG: Privé modification = NON\n");
  }
}

void
on_check_oui_m_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Check Oui (modification) clicked!\n");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(objet_graphique))) {
    prive_m = 1;  
    printf("DEBUG: Privé modification = OUI (clicked)\n");
  }
}

void
on_check_non_m_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Check Non (modification) clicked!\n");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(objet_graphique))) {
    prive_m = 0;  
    printf("DEBUG: Privé modification = NON (clicked)\n");
  }
}

void on_btn_recherche_m_clicked(GtkWidget *objet_graphique, gpointer user_data)
{
    printf("DEBUG: Bouton Recherche modification cliqué!\n");
    
    GtkWidget *modifier_window = lookup_widget(objet_graphique, "modifier_window");
    if (!modifier_window) {
        printf("DEBUG: Erreur - modifier_window non trouvée!\n");
        return;
    }
    
    GtkWidget *entry_recherche = lookup_widget(modifier_window, "entry_recherche_m");
    
    const gchar *id_text = gtk_entry_get_text(GTK_ENTRY(entry_recherche));
    
    if (strlen(id_text) == 0) {
        afficher_message_dialog(objet_graphique, "Veuillez entrer un ID!", GTK_MESSAGE_WARNING);
        return;
    }
    
    int id = atoi(id_text);
    Entraineur *entraineur = rechercher_entraineur(id);
    
    if (entraineur != NULL) {
        
        GtkWidget *lb_nom_m = lookup_widget(modifier_window, "lb_nom_m");
        GtkWidget *lb_prenom_m = lookup_widget(modifier_window, "lb_prenom_m");
        GtkWidget *lb_num_m = lookup_widget(modifier_window, "lb_num_m");
        GtkWidget *lb_sexe_m = lookup_widget(modifier_window, "lb_sexe_m");
        GtkWidget *lb_email_m = lookup_widget(modifier_window, "lb_email_m");
        GtkWidget *lb_specialite_m = lookup_widget(modifier_window, "lb_specialite_m");
        GtkWidget *lb_centres_m = lookup_widget(modifier_window, "lb_centres_m");
        GtkWidget *lb_jour_m = lookup_widget(modifier_window, "lb_jour_m");
        GtkWidget *lb_hd_m = lookup_widget(modifier_window, "lb_hd_m");
        GtkWidget *lb_hf_m = lookup_widget(modifier_window, "lb_hf_m");
        GtkWidget *label_prive_m = lookup_widget(modifier_window, "label_prive_m");
        
        
        GtkWidget *entry_nom, *entry_prenom, *entry_num, *entry_email;
        GtkWidget *spin_hd, *spin_hf;
        GtkWidget *radio_homme, *radio_femme;
        GtkWidget *check_oui, *check_non;
        GtkWidget *com_specialite, *com_centre, *com_jour;
        
        entry_nom = lookup_widget(modifier_window, "entry_nom_m");
        entry_prenom = lookup_widget(modifier_window, "entry_prenom_m");
        entry_num = lookup_widget(modifier_window, "entry_num_m");
        entry_email = lookup_widget(modifier_window, "entry_email_m");
        spin_hd = lookup_widget(modifier_window, "spin_hd_m");
        spin_hf = lookup_widget(modifier_window, "spin_hf_m");
        radio_homme = lookup_widget(modifier_window, "radio_homme_m");
        radio_femme = lookup_widget(modifier_window, "radio_femme_m");
        check_oui = lookup_widget(modifier_window, "check_oui_m");
        check_non = lookup_widget(modifier_window, "check_non_m");
        com_specialite = lookup_widget(modifier_window, "com_specialite_m");
        com_centre = lookup_widget(modifier_window, "com_centre_m");
        com_jour = lookup_widget(modifier_window, "com_jour_m");
        
        
        if (lb_nom_m) gtk_label_set_text(GTK_LABEL(lb_nom_m), entraineur->nom);
        if (lb_prenom_m) gtk_label_set_text(GTK_LABEL(lb_prenom_m), entraineur->prenom);
        if (lb_num_m) gtk_label_set_text(GTK_LABEL(lb_num_m), entraineur->num_tel);
        if (lb_sexe_m) gtk_label_set_text(GTK_LABEL(lb_sexe_m), entraineur->sexe);
        if (lb_email_m) gtk_label_set_text(GTK_LABEL(lb_email_m), entraineur->email);
        if (lb_specialite_m) gtk_label_set_text(GTK_LABEL(lb_specialite_m), entraineur->specialite);
        if (lb_centres_m) gtk_label_set_text(GTK_LABEL(lb_centres_m), entraineur->centre_sportif);
        if (lb_jour_m) gtk_label_set_text(GTK_LABEL(lb_jour_m), entraineur->jour_dispo);
        
      
        gchar heure_debut_str[10];
        gchar heure_fin_str[10];
        gchar prive_str[10];
        
        snprintf(heure_debut_str, sizeof(heure_debut_str), "%d", entraineur->heure_debut);
        snprintf(heure_fin_str, sizeof(heure_fin_str), "%d", entraineur->heure_fin);
        snprintf(prive_str, sizeof(prive_str), "%s", entraineur->prive == 1 ? "Oui" : "Non");
        
        if (lb_hd_m) gtk_label_set_text(GTK_LABEL(lb_hd_m), heure_debut_str);
        if (lb_hf_m) gtk_label_set_text(GTK_LABEL(lb_hf_m), heure_fin_str);
        if (label_prive_m) gtk_label_set_text(GTK_LABEL(label_prive_m), prive_str);
        
        if (entry_nom) gtk_entry_set_text(GTK_ENTRY(entry_nom), "");
        if (entry_prenom) gtk_entry_set_text(GTK_ENTRY(entry_prenom), "");
        if (entry_num) gtk_entry_set_text(GTK_ENTRY(entry_num), "");
        if (entry_email) gtk_entry_set_text(GTK_ENTRY(entry_email), "");
        
       
        if (spin_hd) gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hd), 0);
        if (spin_hf) gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hf), 0);
        
        
        if (com_specialite) gtk_combo_box_set_active(GTK_COMBO_BOX(com_specialite), -1);
        if (com_centre) gtk_combo_box_set_active(GTK_COMBO_BOX(com_centre), -1);
        if (com_jour) gtk_combo_box_set_active(GTK_COMBO_BOX(com_jour), -1);
        
        printf("DEBUG: Résultats affichés dans les labels\n");
        afficher_message_dialog(objet_graphique, "Entraineur trouvé! Les infos s'affichent ci-dessus.", GTK_MESSAGE_INFO);
    } else {
        afficher_message_dialog(objet_graphique, "Aucun entraineur trouvé!", GTK_MESSAGE_ERROR);
    }
}


void on_btn_modifie_clicked(GtkWidget *objet_graphique, gpointer user_data)
{
    GtkWidget *modifier_window = lookup_widget(objet_graphique, "modifier_window");
    if (!modifier_window) {
        printf("DEBUG: Erreur - modifier_window non trouvée!\n");
        return;
    }
    
    GtkWidget *entry_recherche, *entry_nom, *entry_prenom, *entry_num, *entry_email;
    GtkWidget *com_specialite, *com_centre, *com_jour;
    GtkWidget *spin_hd, *spin_hf;
    GtkWidget *radio_homme, *radio_femme;
    GtkWidget *check_oui, *check_non;
    
    entry_recherche = lookup_widget(modifier_window, "entry_recherche_m");
    entry_nom = lookup_widget(modifier_window, "entry_nom_m");
    entry_prenom = lookup_widget(modifier_window, "entry_prenom_m");
    entry_num = lookup_widget(modifier_window, "entry_num_m");
    entry_email = lookup_widget(modifier_window, "entry_email_m");
    com_specialite = lookup_widget(modifier_window, "com_specialite_m");
    com_centre = lookup_widget(modifier_window, "com_centre_m");
    com_jour = lookup_widget(modifier_window, "com_jour_m");
    spin_hd = lookup_widget(modifier_window, "spin_hd_m");
    spin_hf = lookup_widget(modifier_window, "spin_hf_m");
    radio_homme = lookup_widget(modifier_window, "radio_homme_m");
    radio_femme = lookup_widget(modifier_window, "radio_femme_m");
    check_oui = lookup_widget(modifier_window, "check_oui_m");
    check_non = lookup_widget(modifier_window, "check_non_m");
    
    const gchar *id_text = gtk_entry_get_text(GTK_ENTRY(entry_recherche));
    
    if (strlen(id_text) == 0) {
        afficher_message_dialog(objet_graphique, "Recherchez d'abord un entraineur!", GTK_MESSAGE_WARNING);
        return;
    }
    
    int id = atoi(id_text);
    Entraineur entraineur_modifie;
    Entraineur *ancien = rechercher_entraineur(id);
    
    entraineur_modifie.id = id;
    strcpy(entraineur_modifie.nom, gtk_entry_get_text(GTK_ENTRY(entry_nom)));
    strcpy(entraineur_modifie.prenom, gtk_entry_get_text(GTK_ENTRY(entry_prenom)));
    strcpy(entraineur_modifie.num_tel, gtk_entry_get_text(GTK_ENTRY(entry_num)));
    strcpy(entraineur_modifie.email, gtk_entry_get_text(GTK_ENTRY(entry_email)));
    
    if (sexe_m == 1)
        strcpy(entraineur_modifie.sexe, "Homme");
    else
        strcpy(entraineur_modifie.sexe, "Femme");
    
    entraineur_modifie.prive = prive_m;
    
    if (ancien)
        strcpy(entraineur_modifie.mot_de_passe, ancien->mot_de_passe);
    
    if (com_specialite) {
        gchar *spec = gtk_combo_box_get_active_text(GTK_COMBO_BOX(com_specialite));
        strcpy(entraineur_modifie.specialite, spec && strlen(spec) > 0 ? spec : "Fitness");
        if (spec) g_free(spec);
    }
    
    if (com_centre) {
        gchar *centre = gtk_combo_box_get_active_text(GTK_COMBO_BOX(com_centre));
        strcpy(entraineur_modifie.centre_sportif, centre && strlen(centre) > 0 ? centre : "Centre Principal");
        if (centre) g_free(centre);
    }
    
    if (com_jour) {
        gchar *jour = gtk_combo_box_get_active_text(GTK_COMBO_BOX(com_jour));
        strcpy(entraineur_modifie.jour_dispo, jour && strlen(jour) > 0 ? jour : "Lundi");
        if (jour) g_free(jour);
    }
    
    if (spin_hd)
        entraineur_modifie.heure_debut = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_hd));
    if (spin_hf)
        entraineur_modifie.heure_fin = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_hf));
    
    modifier_entraineur(id, &entraineur_modifie);
    afficher_message_dialog(objet_graphique, "Modification réussie!", GTK_MESSAGE_INFO);
    
    if (entry_recherche) gtk_entry_set_text(GTK_ENTRY(entry_recherche), "");
    if (entry_nom) gtk_entry_set_text(GTK_ENTRY(entry_nom), "");
    if (entry_prenom) gtk_entry_set_text(GTK_ENTRY(entry_prenom), "");
    if (entry_num) gtk_entry_set_text(GTK_ENTRY(entry_num), "");
    if (entry_email) gtk_entry_set_text(GTK_ENTRY(entry_email), "");
    
    if (spin_hd) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hd), 0.0);
        gtk_spin_button_update(GTK_SPIN_BUTTON(spin_hd));
    }
    if (spin_hf) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hf), 0.0);
        gtk_spin_button_update(GTK_SPIN_BUTTON(spin_hf));
    }
    
    if (com_specialite) gtk_combo_box_set_active(GTK_COMBO_BOX(com_specialite), -1);
    if (com_centre) gtk_combo_box_set_active(GTK_COMBO_BOX(com_centre), -1);
    if (com_jour) gtk_combo_box_set_active(GTK_COMBO_BOX(com_jour), -1);
    
    sexe_m = 1;
    prive_m = 0;
    
    if (radio_homme) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_homme), TRUE);
    
    if (check_non) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_non), TRUE);
    
    GtkWidget *lb_nom_m = lookup_widget(modifier_window, "lb_nom_m");
    GtkWidget *lb_prenom_m = lookup_widget(modifier_window, "lb_prenom_m");
    GtkWidget *lb_num_m = lookup_widget(modifier_window, "lb_num_m");
    GtkWidget *lb_sexe_m = lookup_widget(modifier_window, "lb_sexe_m");
    GtkWidget *lb_email_m = lookup_widget(modifier_window, "lb_email_m");
    GtkWidget *lb_specialite_m = lookup_widget(modifier_window, "lb_specialite_m");
    GtkWidget *lb_centres_m = lookup_widget(modifier_window, "lb_centres_m");
    GtkWidget *lb_jour_m = lookup_widget(modifier_window, "lb_jour_m");
    GtkWidget *lb_hd_m = lookup_widget(modifier_window, "lb_hd_m");
    GtkWidget *lb_hf_m = lookup_widget(modifier_window, "lb_hf_m");
    GtkWidget *label_prive_m = lookup_widget(modifier_window, "label_prive_m");
    
    if (lb_nom_m) gtk_label_set_text(GTK_LABEL(lb_nom_m), "");
    if (lb_prenom_m) gtk_label_set_text(GTK_LABEL(lb_prenom_m), "");
    if (lb_num_m) gtk_label_set_text(GTK_LABEL(lb_num_m), "");
    if (lb_sexe_m) gtk_label_set_text(GTK_LABEL(lb_sexe_m), "");
    if (lb_email_m) gtk_label_set_text(GTK_LABEL(lb_email_m), "");
    if (lb_specialite_m) gtk_label_set_text(GTK_LABEL(lb_specialite_m), "");
    if (lb_centres_m) gtk_label_set_text(GTK_LABEL(lb_centres_m), "");
    if (lb_jour_m) gtk_label_set_text(GTK_LABEL(lb_jour_m), "");
    if (lb_hd_m) gtk_label_set_text(GTK_LABEL(lb_hd_m), "");
    if (lb_hf_m) gtk_label_set_text(GTK_LABEL(lb_hf_m), "");
    if (label_prive_m) gtk_label_set_text(GTK_LABEL(label_prive_m), "");
    
    printf("DEBUG: Tous les champs de modification ont été réinitialisés!\n");
}

void
on_btn_annule_m_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton Annuler modification cliqué!\n");
  
  modifier_window = lookup_widget(objet_graphique, "modifier_window");
  if (modifier_window) {
    gtk_widget_hide(modifier_window);
  }
  
  admin_window = create_admin_window();
  gtk_widget_show(admin_window);
}


void
on_btn_supp_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton Supprimer cliqué!\n");
  
  GtkWidget *entry_id = lookup_widget(objet_graphique, "entry_ID_En_supp");
  if (!entry_id) {
    printf("DEBUG: Erreur - champ ID non trouvé!\n");
    return;
  }
  
  const gchar *id_text = gtk_entry_get_text(GTK_ENTRY(entry_id));
  
  if (strlen(id_text) == 0) {
    afficher_message_dialog(objet_graphique, "Veuillez entrer un ID!", GTK_MESSAGE_WARNING);
    return;
  }
  
  int id = atoi(id_text);
  Entraineur *entraineur = rechercher_entraineur(id);
  
  if (entraineur != NULL) {
    printf("DEBUG: Entraineur à supprimer: %s %s\n", entraineur->prenom, entraineur->nom);
    
    GtkWidget *parent_window = gtk_widget_get_toplevel(objet_graphique);
    gchar message[200];
    snprintf(message, sizeof(message), "Êtes-vous sûr de vouloir supprimer l'entraineur %s %s?", entraineur->prenom, entraineur->nom);
    
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(parent_window),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "%s", message);
    
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    if (result == GTK_RESPONSE_YES) {
      supprimer_entraineur(id);
      printf("DEBUG: Entraineur supprimé!\n");
      
      afficher_message_dialog(objet_graphique, "Entraineur supprimé avec succès!", GTK_MESSAGE_INFO);
      gtk_entry_set_text(GTK_ENTRY(entry_id), "");
    }
  } else {
    printf("DEBUG: Aucun entraineur trouvé avec ID=%d\n", id);
    
    afficher_message_dialog(objet_graphique, "Aucun entraineur trouvé avec cet ID!", GTK_MESSAGE_ERROR);
  }
}

void
on_btn_retour_supp_clicked (GtkWidget *objet_graphique, gpointer user_data)
{
  printf("DEBUG: Bouton Retour suppression cliqué!\n");
  
  supp_window = lookup_widget(objet_graphique, "supp_window");
  if (supp_window) {
    gtk_widget_hide(supp_window);
  }
  
  admin_window = create_admin_window();
  gtk_widget_show(admin_window);
}




void on_btn_recherchere_clicked (GtkWidget *objet_graphique, gpointer user_data) 
{
  printf("DEBUG: Bouton Recherche cliqué!\n");
  
  
  GtkWidget *recherche_window = lookup_widget(objet_graphique, "recherche_window");
  if (!recherche_window) {
    printf("DEBUG: Erreur - recherche_window non trouvée!\n");
    return;
  }
  

  GtkWidget *entryID_En_r = lookup_widget(recherche_window, "entryID_En_r");
  if (!entryID_En_r) {
    printf("DEBUG: Erreur - champ de recherche non trouvé!\n");
    return;
  }
  
  const gchar *id_text = gtk_entry_get_text(GTK_ENTRY(entryID_En_r));
  
  if (strlen(id_text) == 0) {
    afficher_message_dialog(objet_graphique, "Veuillez entrer un ID!", GTK_MESSAGE_WARNING);
    return;
  }
  
  int id = atoi(id_text);
  printf("DEBUG: Recherche de l'entraineur avec ID=%d\n", id);
  
  Entraineur *entraineur = rechercher_entraineur(id);
  
  if (entraineur != NULL) {
    printf("DEBUG: Entraineur trouvé!\n");
    
   
    GtkWidget *lab_nom_r = lookup_widget(recherche_window, "lab_nom_r");
    GtkWidget *lab_prenom_r = lookup_widget(recherche_window, "lab_prenom_r");
    GtkWidget *lab_sexe_r = lookup_widget(recherche_window, "lab_sexe_r");
    GtkWidget *lab_num_r = lookup_widget(recherche_window, "lab_num_r");
    GtkWidget *lab_specialite_r = lookup_widget(recherche_window, "lab_specialite_r");
    GtkWidget *lab_email_r = lookup_widget(recherche_window, "lab_email_r");
    GtkWidget *label_centre_r = lookup_widget(recherche_window, "label_centre_r");
    GtkWidget *label_jour_r = lookup_widget(recherche_window, "label_jour_r");
    GtkWidget *label_hd_r = lookup_widget(recherche_window, "label_hd_r");
    GtkWidget *label_hf_r = lookup_widget(recherche_window, "label_hf_r");
    GtkWidget *label_p_r = lookup_widget(recherche_window, "label_p_r");
    
 
    if (lab_nom_r) gtk_label_set_text(GTK_LABEL(lab_nom_r), entraineur->nom);
    if (lab_prenom_r) gtk_label_set_text(GTK_LABEL(lab_prenom_r), entraineur->prenom);
    if (lab_sexe_r) gtk_label_set_text(GTK_LABEL(lab_sexe_r), entraineur->sexe);
    if (lab_num_r) gtk_label_set_text(GTK_LABEL(lab_num_r), entraineur->num_tel);
    if (lab_specialite_r) gtk_label_set_text(GTK_LABEL(lab_specialite_r), entraineur->specialite);
    if (lab_email_r) gtk_label_set_text(GTK_LABEL(lab_email_r), entraineur->email);
    if (label_centre_r) gtk_label_set_text(GTK_LABEL(label_centre_r), entraineur->centre_sportif);
    if (label_jour_r) gtk_label_set_text(GTK_LABEL(label_jour_r), entraineur->jour_dispo);
    
    
    gchar heure_debut_str[10];
    gchar heure_fin_str[10];
    gchar prive_str[10];
    
    snprintf(heure_debut_str, sizeof(heure_debut_str), "%d", entraineur->heure_debut);
    snprintf(heure_fin_str, sizeof(heure_fin_str), "%d", entraineur->heure_fin);
    snprintf(prive_str, sizeof(prive_str), "%s", entraineur->prive == 1 ? "Oui" : "Non");
    
    if (label_hd_r) gtk_label_set_text(GTK_LABEL(label_hd_r), heure_debut_str);
    if (label_hf_r) gtk_label_set_text(GTK_LABEL(label_hf_r), heure_fin_str);
    if (label_p_r) gtk_label_set_text(GTK_LABEL(label_p_r), prive_str);
    
    printf("DEBUG: Résultats affichés dans les labels\n");
    afficher_message_dialog(objet_graphique, "Entraineur trouvé!", GTK_MESSAGE_INFO);
    
  } else {
    printf("DEBUG: Aucun entraineur trouvé avec ID=%d\n", id);
    afficher_message_dialog(objet_graphique, "Aucun entraineur trouvé avec cet ID!", GTK_MESSAGE_ERROR);
  }
}

void on_btn_retour_rech_clicked (GtkWidget *objet_graphique, gpointer user_data) 
{
  printf("DEBUG: Bouton Retour recherche cliqué!\n");
  
  recherche_window = lookup_widget(objet_graphique, "recherche_window");
  if (recherche_window) {
    gtk_widget_hide(recherche_window);
  }
  
  admin_window = create_admin_window();
  gtk_widget_show(admin_window);
}

void on_btn_annule_inscrit_clicked (GtkWidget *objet_graphique, gpointer user_data) 
{
  printf("DEBUG: Bouton Annuler inscription cliqué!\n");
  inscription_window = lookup_widget(objet_graphique, "inscription_window");
  if (inscription_window) {
    gtk_widget_hide(inscription_window);
  }
  
  Login_entraineur = create_Login_entraineur();
  gtk_widget_show(Login_entraineur);
}

void on_btn_inscrit_clicked (GtkWidget *objet_graphique, gpointer user_data) 
{
    printf("DEBUG: Bouton Inscription cliqué!\n");
    GtkWidget *inscription_window = lookup_widget(objet_graphique, "inscription_window");
    if (!inscription_window) {
        printf("DEBUG: Erreur - inscription_window non trouvée!\n");
        return;
    }
    
    GtkWidget *com_cours_ins = lookup_widget(inscription_window, "com_cours_ins");
    GtkWidget *com_choix_eq_ins = lookup_widget(inscription_window, "com_choix_eq_ins");
    GtkWidget *com_jour_ins = lookup_widget(inscription_window, "com_jour_ins");
    GtkWidget *spin_hd_ins = lookup_widget(inscription_window, "spin_hd_ins");
    GtkWidget *spin_ha_ins = lookup_widget(inscription_window, "spin_ha_ins");
    
    if (id_entraineur_connecte == -1) {
        afficher_message_dialog(objet_graphique, "Erreur: Aucun entraineur connecté!", GTK_MESSAGE_ERROR);
        return;
    }
    
    int id_entraineur = id_entraineur_connecte;
    
    gchar *cours = com_cours_ins ? gtk_combo_box_get_active_text(GTK_COMBO_BOX(com_cours_ins)) : NULL;
    gchar *equipement = com_choix_eq_ins ? gtk_combo_box_get_active_text(GTK_COMBO_BOX(com_choix_eq_ins)) : NULL;
    gchar *jour = com_jour_ins ? gtk_combo_box_get_active_text(GTK_COMBO_BOX(com_jour_ins)) : NULL;
    int heure_debut = spin_hd_ins ? gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_hd_ins)) : 8;
    int heure_fin = spin_ha_ins ? gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_ha_ins)) : 18;
    
    InscriptionCours inscription;
    inscription.id_entraineur = id_entraineur;
    strcpy(inscription.cours, cours && strlen(cours) > 0 ? cours : "Fitness");
    strcpy(inscription.equipements, equipement && strlen(equipement) > 0 ? equipement : "Aucun");
    strcpy(inscription.jour, jour && strlen(jour) > 0 ? jour : "Lundi");
    inscription.heure_debut = heure_debut;
    inscription.heure_fin = heure_fin;
    
    if (cours) g_free(cours);
    if (equipement) g_free(equipement);
    if (jour) g_free(jour);
    
    ajouter_inscription(&inscription);
    
    GtkWidget *treeview_cours = lookup_widget(inscription_window, "treeview_cours");
    if (treeview_cours) {
        afficher_treeview_cours(treeview_cours);
        printf("DEBUG: TreeView cours actualisé après inscription\n");
    }
    
    if (com_cours_ins) gtk_combo_box_set_active(GTK_COMBO_BOX(com_cours_ins), -1);
    if (com_choix_eq_ins) gtk_combo_box_set_active(GTK_COMBO_BOX(com_choix_eq_ins), -1);
    if (com_jour_ins) gtk_combo_box_set_active(GTK_COMBO_BOX(com_jour_ins), -1);
    
    if (spin_hd_ins) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_hd_ins), 0.0);
        gtk_spin_button_update(GTK_SPIN_BUTTON(spin_hd_ins));
    }
    if (spin_ha_ins) {
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_ha_ins), 0.0);
        gtk_spin_button_update(GTK_SPIN_BUTTON(spin_ha_ins));
    }
    
    afficher_message_dialog(objet_graphique, "Inscription enregistrée avec succès!", GTK_MESSAGE_INFO);
    
    printf("DEBUG: Tous les champs d'inscription ont été réinitialisés!\n");
}
void on_btn_actualiser_entraineurs_clicked (GtkWidget *objet_graphique, gpointer user_data) 
{
    printf("DEBUG: Bouton Actualiser liste cliqué!\n");
    
    GtkWidget *liste_entraineurs_window = lookup_widget(objet_graphique, "liste_entraineurs_window");
    if (!liste_entraineurs_window) {
        printf("DEBUG: Erreur - liste_entraineurs_window non trouvée!\n");
        return;
    }
    
    GtkWidget *treeview = lookup_widget(liste_entraineurs_window, "treeview_entraineurs");
    if (!treeview) {
        printf("DEBUG: Erreur - treeview_entraineurs non trouvé!\n");
        return;
    }
    
    afficher_treeview_entraineurs(treeview);
    
    afficher_message_dialog(objet_graphique, "Liste actualisée avec succès!", GTK_MESSAGE_INFO);
}
void on_btn_retour_l_en_clicked (GtkWidget *objet_graphique, gpointer user_data) 
{
  printf("DEBUG: Bouton Retour liste cliqué!\n");
  
  liste_entraineurs_window = lookup_widget(objet_graphique, "liste_entraineurs_window");
  if (liste_entraineurs_window) {
    gtk_widget_hide(liste_entraineurs_window);
  }
  
  admin_window = create_admin_window();
  gtk_widget_show(admin_window);
}

void on_treeview_entraineurs_row_activated(GtkTreeView *treeview,
                                           GtkTreePath *path,
                                           GtkTreeViewColumn *column,
                                           gpointer user_data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    gint id;
    gchar *nom, *prenom;
    
    model = gtk_tree_view_get_model(treeview);
    
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        gtk_tree_model_get(model, &iter,
                          0, &id,
                          1, &nom,
                          2, &prenom,
                          -1);
        
        gchar message[200];
        snprintf(message, sizeof(message), 
                "Entraineur sélectionné:\nID: %d\nNom: %s\nPrénom: %s", 
                id, nom, prenom);
        
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(treeview))),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "%s", message);
        
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        g_free(nom);
        g_free(prenom);
    }
}

void on_treeview_cours_row_activated(GtkTreeView *treeview,
                                     GtkTreePath *path,
                                     GtkTreeViewColumn *column,
                                     gpointer user_data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    gchar *cours, *equipements, *jour, *horaires;
    
    model = gtk_tree_view_get_model(treeview);
    
    if (gtk_tree_model_get_iter(model, &iter, path)) {
        gtk_tree_model_get(model, &iter,
                          0, &cours,
                          1, &equipements,
                          2, &jour,
                          3, &horaires,
                          -1);
        
        gchar message[300];
        snprintf(message, sizeof(message), 
                "Cours sélectionné:\n\nCours: %s\nÉquipements: %s\nJour: %s\nHoraires: %s", 
                cours, equipements, jour, horaires);
        
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(treeview))),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "%s", message);
        
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        
        g_free(cours);
        g_free(equipements);
        g_free(jour);
        g_free(horaires);
    }
}
