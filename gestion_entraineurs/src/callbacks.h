#include <gtk/gtk.h>


void
on_btn_rechercher_en_clicked           (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_ajouter_en_clicked              (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_modifier_en_clicked             (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_supprimer_en_clicked            (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_retour_admin_clicked            (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_voirliste_en_clicked            (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_admin_en_clicked                (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_quitter_clicked                 (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_entraineur_en_clicked           (GtkWidget *objet_graphique, gpointer user_data);


void
on_btn_recherche_m_clicked             (GtkWidget *objet_graphique, gpointer user_data);



void
on_btn_modifie_clicked                 (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_annule_m_clicked                (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_annule_cnxad_clicked            (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_cnxad_clicked                   (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_recherchere_clicked             (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_retour_rech_clicked             (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_annule_inscrit_clicked          (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_inscrit_clicked                 (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_supp_clicked                    (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_retour_supp_clicked             (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_annule_cnx_clicked              (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_cnx_en_clicked                  (GtkWidget *objet_graphique, gpointer user_data);


void
on_btn_ajoute_en_clicked               (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_annule_aj_clicked               (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_actualiser_entraineurs_clicked  (GtkWidget *objet_graphique, gpointer user_data);

void
on_btn_retour_l_en_clicked             (GtkWidget *objet_graphique, gpointer user_data);



void
on_radio_homme_m_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_radio_femme_m_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_check_oui_m_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_check_non_m_toggled                 (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_radio_femme_aj_toggled              (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_radio_homme_aj_toggled              (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_check_oui_aj_toggled                (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_check_non__aj_toggled               (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_treeview_entraineurs_row_activated  (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);

void
on_treeview_cours_row_activated        (GtkTreeView     *treeview,
                                        GtkTreePath     *path,
                                        GtkTreeViewColumn *column,
                                        gpointer         user_data);
