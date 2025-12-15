#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>

/* Variables globales */
extern int is_admin_mode;
extern int recherche_par_nom;
extern int current_trainer_id;

/* Fonctions utilitaires */
void show_message(GtkWindow *parent, GtkMessageType type, const char *title, const char *message);
void show_info_message(GtkWindow *parent, const char *message);
void show_error_message(GtkWindow *parent, const char *message);
int show_confirm_message(GtkWindow *parent, const char *message);
void switch_window(GtkWidget *from_window, const char *to_window_name);

/* Fonctions pour TreeView et ComboBox */
void remplir_treeview_centres(GtkWidget *window);
void initialiser_combobox_specialite(GtkWidget *window);

/* Callbacks principaux */
void on_button_administrateur_clicked(GtkButton *button, gpointer user_data);
void on_button_entraineur_clicked(GtkButton *button, gpointer user_data);
void on_check_admin_clicked(GtkButton *button, gpointer user_data);
void on_button_connexion_clicked(GtkButton *button, gpointer user_data);
void on_button_connexion_entraineur_clicked(GtkButton *button, gpointer user_data);

/* Menu principal */
void on_button_gestion_centre_clicked(GtkButton *button, gpointer user_data);
void on_button_gestion_entraineur_clicked(GtkButton *button, gpointer user_data);
void on_button_gestion_membre_enter(GtkButton *button, gpointer user_data);
void on_button_gestion_cours_clicked(GtkButton *button, gpointer user_data);
void on_button_gestion_equipement_clicked(GtkButton *button, gpointer user_data);

/* Gestion des centres */
void on_button_ajouter_clicked(GtkButton *button, gpointer user_data);
void on_button_modifier_clicked(GtkButton *button, gpointer user_data);
void on_button_supprimer_clicked(GtkButton *button, gpointer user_data);
void on_button_rechercher_clicked(GtkButton *button, gpointer user_data);
void on_button_s_inscrire_clicked(GtkButton *button, gpointer user_data);
void on_button_retour_gestion_lc_clicked(GtkButton *button, gpointer user_data);

/* Ajout d'un centre */
void on_button_valider_ajout_clicked(GtkButton *button, gpointer user_data);
void on_button_annuler_ajout_clicked(GtkButton *button, gpointer user_data);
void on_button_retour_ajout_lc_clicked(GtkButton *button, gpointer user_data);
void on_spinbutton_capacite_ajout_changed(GtkEditable *editable, gpointer user_data);
void on_spinbutton_id_ajout_delete_text(GtkEditable *editable, gpointer user_data);
void on_radiobutton_inactif_ajout_clicked(GtkButton *button, gpointer user_data);
void on_radiobutton_actif_ajout_clicked(GtkButton *button, gpointer user_data);

/* Modification d'un centre */
void on_spinbutton_id_modif_change_value(GtkEditable *editable, gpointer user_data);
void on_button_modifier_centre_clicked(GtkButton *button, gpointer user_data);
void on_button_annuler_modif_clicked(GtkButton *button, gpointer user_data);
void on_button_retour_modif_lc_clicked(GtkButton *button, gpointer user_data);
void on_radiobutton_actif_modif_clicked(GtkButton *button, gpointer user_data);
void on_radiobutton_inactif_modif_clicked(GtkButton *button, gpointer user_data);

/* Suppression d'un centre */
void on_button_oui_supprimer_clicked(GtkButton *button, gpointer user_data);
void on_button_non_supprimer_clicked(GtkButton *button, gpointer user_data);
void on_button_retour_supprimer_lc_clicked(GtkButton *button, gpointer user_data);
void on_spinbutton_id_supprimer_lc_value_changed(GtkSpinButton *spinbutton, gpointer user_data);
void on_spinbutton_id_supprimer_lc_changed(GtkEditable *editable, gpointer user_data);

/* Recherche d'un centre */
void on_button_lancer_recherche_clicked(GtkButton *button, gpointer user_data);
void on_button_annuler_recherche_clicked(GtkButton *button, gpointer user_data);
void on_button_retour_recherche_lc_clicked(GtkButton *button, gpointer user_data);
void on_radiobutton_recherche_nom_clicked(GtkButton *button, gpointer user_data);
void on_radiobutton_recherche_ville_clicked(GtkButton *button, gpointer user_data);

/* Inscription entraîneur */
void on_button_valider_inscription_clicked(GtkButton *button, gpointer user_data);
void on_button_annuler_inscription_clicked(GtkButton *button, gpointer user_data);
void on_button_retour_inscription_lc_clicked(GtkButton *button, gpointer user_data);
void on_combobox_specialite_lc_move_active(GtkComboBox *combobox, GtkScrollType scroll_type, gpointer user_data);
void on_combobox_specialite_lc_changed(GtkComboBox *combobox, gpointer user_data);
gboolean on_combobox_specialite_lc_popdown(GtkComboBox *combobox, gpointer user_data);
void on_combobox_specialite_lc_popup(GtkComboBox *combobox, gpointer user_data);

/* Gestion de fenêtre */
void on_window_destroy(GtkWidget *widget, gpointer user_data);

#endif /* CALLBACKS_H */
