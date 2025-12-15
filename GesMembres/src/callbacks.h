#ifndef CALLBACKS_H
#define CALLBACKS_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

void on_radiobutton_kh_new_hom29_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_radiobutton_kh_new_fem30_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_button_kh_adm_int33_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_mem_int34_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_annul_login35_clicked(GtkButton *button, gpointer user_data);
void on_checkbutton_kh_human_login1_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_button_kh_connecter_login36_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_logout_int_adm37_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_aj_mem38_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_Mod_Mem39_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_del_mem40_clicked(GtkButton *button, gpointer user_data);
void on_radiobutton_kh_homme25_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_radiobutton_kh_femme_mem26_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_radiobutton_hom_kh_new29_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_button_kh_val_aj_mem35_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_41_clicked(GtkButton *button, gpointer user_data);
void on_button_valider_modif_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_42_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_annul_aj_mem36_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_43_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_3_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_search_mem41_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_49_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_1_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_2_clicked(GtkButton *button, gpointer user_data);
void on_radiobutton_kh_all_adm23_group_changed(GtkRadioButton *radiobutton, gpointer user_data);
void on_radiobutton_kh_spo_adm24_group_changed(GtkRadioButton *radiobutton, gpointer user_data);
void on_radiobutton_kh_par_ID_27_group_changed(GtkRadioButton *radiobutton, gpointer user_data);
void on_combobox_kh_cour_adm3_popup(GtkComboBox *combobox, gpointer user_data);
void on_combobox_kh_cour_mem4_popup(GtkComboBox *combobox, gpointer user_data);
void on_button_kh_39_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_40_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_4_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_44_clicked(GtkButton *button, gpointer user_data);
void on_calendar_kh_2_day_selected(GtkCalendar *calendar, gpointer user_data);
void on_button_kh_5_clicked(GtkButton *button, gpointer user_data);
void on_checkbutton_kh_1_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_checkbutton_kh_1_released(GtkButton *button, gpointer user_data);
void on_checkbutton_kh_2_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_checkbutton_kh_2_released(GtkButton *button, gpointer user_data);
void on_checkbutton_kh_3_toggled(GtkToggleButton *togglebutton, gpointer user_data);
void on_checkbutton_kh_3_released(GtkButton *button, gpointer user_data);
void on_button_kh_46_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_47_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_48_clicked(GtkButton *button, gpointer user_data);
void on_entry_kh_pass_login63_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_MDP_login64_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_nom_membre63_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_prenm_mem64_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_id_aj_mem65_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_pwd_aj_mem66_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_mdp_aj_mem67_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_num_mem68_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_mail_mem69_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_haut_mem70_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_poid_mem71_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_nv_nom73_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_nv_pre74_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_spinbutton_kh_new_day35_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_spinbutton_kh_new_month36_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_spinbutton_kh_new_year37_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_new_id75_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_new_pwd76_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_new_num78_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_new_mail_kh79_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_new_haut80_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_new_pd81_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_id_kh_sr83_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_84_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_entry_kh_93_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data);
void on_combobox_kh_6_popup(GtkComboBox *combobox, gpointer user_data);
void on_radiobutton_kh_34_group_changed(GtkRadioButton *radiobutton, gpointer user_data);
void on_radiobutton_kh_33_group_changed(GtkRadioButton *radiobutton, gpointer user_data);
gboolean on_treeview_kh_liste_mem1_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean on_treeview_kh_1_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
gboolean on_treeview_kh_2_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data);


void fill_treeview_reservations(GtkWidget *treeview);
void on_button_kh_refresh_clicked(GtkButton *button, gpointer user_data);
void on_button_kh_refrech_clicked(GtkButton *button, gpointer user_data);
void
on_Int_Membre_show                     (GtkWidget       *widget,
                                        gpointer         user_data);


G_END_DECLS

#endif 




