#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "callbacks.h"
#include "cours.h"
#include "interface.h"
#include "membre.h"
#include "support.h"

// --- Navigation helpers ---
static int current_modify_id = -1;
static int current_delete_id = -1;

// --- MAIN MENU ---

void on_button6_clicked(GtkButton *button, gpointer user_data) {
  // Admin login
  GtkWidget *main_win = lookup_widget(GTK_WIDGET(button), "Menu Principal");
  GtkWidget *admin_login = create_Menu_Admin();
  gtk_widget_show(admin_login);
  if (main_win)
    gtk_widget_hide(main_win);
}

void on_button7_clicked(GtkButton *button, gpointer user_data) {
  // Membre Login
  GtkWidget *main_win = lookup_widget(GTK_WIDGET(button), "Menu Principal");
  GtkWidget *login_win = create_Login_membre();
  gtk_widget_show(login_win);
  if (main_win)
    gtk_widget_hide(main_win);
}

void on_button9_clicked(GtkButton *button, gpointer user_data) {
  // Next
  GtkWidget *chk = lookup_widget(GTK_WIDGET(button), "checkbutton1");
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chk))) {
    on_button6_clicked(button, user_data);
  } else {
    GtkWidget *dialog =
        gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING,
                               GTK_BUTTONS_OK, "Cochez 'Je suis humain'");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
  }
}

// --- LOGIN ---

void on_button14_clicked(GtkButton *button, gpointer user_data) {
  // Back to Main
  GtkWidget *login_win = lookup_widget(GTK_WIDGET(button), "Menu Admin");
  GtkWidget *main_win = create_Menu_Principal();
  gtk_widget_show(main_win);
  if (login_win)
    gtk_widget_destroy(login_win);
}

void on_button8_clicked(GtkButton *button, gpointer user_data) {
  // Connect
  GtkWidget *entry_user = lookup_widget(GTK_WIDGET(button), "entry8");
  GtkWidget *entry_pass = lookup_widget(GTK_WIDGET(button), "entry7");
  const gchar *user = gtk_entry_get_text(GTK_ENTRY(entry_user));
  const gchar *pass = gtk_entry_get_text(GTK_ENTRY(entry_pass));

  if (strcmp(user, "admin") == 0 && strcmp(pass, "admin") == 0) {
    GtkWidget *login_win = lookup_widget(GTK_WIDGET(button), "Menu Admin");
    GtkWidget *admin_int = create_Interface_Admin();
    gtk_widget_show(admin_int);
    if (login_win)
      gtk_widget_destroy(login_win);
  } else {
    GtkWidget *dialog =
        gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
                               GTK_BUTTONS_OK, "Erreur Login");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
  }
}

// --- INTERFACE ADMIN ---

void on_button25_clicked(GtkButton *button, gpointer user_data) {
  // Logout
  GtkWidget *admin_int = lookup_widget(GTK_WIDGET(button), "Interface Admin");
  GtkWidget *main_win = create_Menu_Principal();
  gtk_widget_show(main_win);
  if (admin_int)
    gtk_widget_destroy(admin_int);
}

void on_button10_clicked(GtkButton *button, gpointer user_data) {
  // Open Ajouter with DYNAMIC ID FIELD and TIME
  GtkWidget *win_add = create_AD_Ajouter();

  // Check access to fields. If Glade has issues, we can try to force them
  // editable here
  GtkWidget *e_nom = lookup_widget(win_add, "entry_nom_cours");
  if (e_nom)
    gtk_editable_set_editable(GTK_EDITABLE(e_nom), TRUE);

  GtkWidget *vbox = lookup_widget(win_add, "vbox_ajouter");
  if (vbox) {
    // 1. ID FIELD
    GtkWidget *hbox_id = gtk_hbox_new(FALSE, 5);
    GtkWidget *lbl_id = gtk_label_new("<b>ID du Cours (Unique):</b>");
    gtk_label_set_use_markup(GTK_LABEL(lbl_id), TRUE);
    GtkWidget *entry_id = gtk_entry_new();
    gtk_widget_set_name(entry_id, "entry_id_manual");

    gtk_box_pack_start(GTK_BOX(hbox_id), lbl_id, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_id), entry_id, TRUE, TRUE, 0);

    // Pack at top
    gtk_box_pack_start(GTK_BOX(vbox), hbox_id, FALSE, FALSE, 0);
    gtk_box_reorder_child(GTK_BOX(vbox), hbox_id, 0);

    // KEY FIX: Attach to window so we can find it later!
    g_object_set_data(G_OBJECT(win_add), "entry_id_manual", entry_id);

    // 2. TIME FIELDS (Heure Debut / Heure Fin) with SpinButtons (30min step
    // logic) We use H (0-23) and M (0,30)
    GtkWidget *hbox_time = gtk_hbox_new(FALSE, 5);

    GtkWidget *lbl_h1 = gtk_label_new("Début (H:M):");
    GtkAdjustment *adj_h1_h =
        (GtkAdjustment *)gtk_adjustment_new(8.0, 0.0, 23.0, 1.0, 1.0, 0.0);
    GtkWidget *spin_h1_h = gtk_spin_button_new(adj_h1_h, 1.0, 0);
    GtkAdjustment *adj_h1_m =
        (GtkAdjustment *)gtk_adjustment_new(0.0, 0.0, 30.0, 30.0, 30.0, 0.0);
    GtkWidget *spin_h1_m = gtk_spin_button_new(adj_h1_m, 1.0, 0);

    GtkWidget *lbl_h2 = gtk_label_new("Fin (H:M):");
    GtkAdjustment *adj_h2_h =
        (GtkAdjustment *)gtk_adjustment_new(10.0, 0.0, 23.0, 1.0, 1.0, 0.0);
    GtkWidget *spin_h2_h = gtk_spin_button_new(adj_h2_h, 1.0, 0);
    GtkAdjustment *adj_h2_m =
        (GtkAdjustment *)gtk_adjustment_new(0.0, 0.0, 30.0, 30.0, 30.0, 0.0);
    GtkWidget *spin_h2_m = gtk_spin_button_new(adj_h2_m, 1.0, 0);

    gtk_box_pack_start(GTK_BOX(hbox_time), lbl_h1, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_time), spin_h1_h, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_time), spin_h1_m, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_time), lbl_h2, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_time), spin_h2_h, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_time), spin_h2_m, FALSE, FALSE, 0);

    // Append to VBox (bottom)
    gtk_box_pack_start(GTK_BOX(vbox), hbox_time, FALSE, FALSE, 0);
    gtk_box_reorder_child(GTK_BOX(vbox), hbox_time, 1);

    // Attach to window
    g_object_set_data(G_OBJECT(win_add), "spin_h1_h", spin_h1_h);
    g_object_set_data(G_OBJECT(win_add), "spin_h1_m", spin_h1_m);
    g_object_set_data(G_OBJECT(win_add), "spin_h2_h", spin_h2_h);
    g_object_set_data(G_OBJECT(win_add), "spin_h2_m", spin_h2_m);

    gtk_widget_show_all(vbox);
  }

  gtk_widget_show(win_add);
}

// --- MODIFIER HELPERS ---

void on_treeview_mod_row_activated(GtkTreeView *treeview, GtkTreePath *path,
                                   GtkTreeViewColumn *col, gpointer user_data) {
  GtkTreeIter iter;
  GtkTreeModel *model = gtk_tree_view_get_model(treeview);

  if (gtk_tree_model_get_iter(model, &iter, path)) {
    gint id;
    gtk_tree_model_get(model, &iter, ID_COLUMN, &id, -1);

    // Populate Fields
    current_modify_id = id;
    GtkWidget *win_mod = gtk_widget_get_toplevel(GTK_WIDGET(treeview));
    Cours *c = rechercher_cours(id);
    if (c) {
      GtkWidget *e_type = lookup_widget(win_mod, "entry_mod_type");
      if (e_type)
        gtk_entry_set_text(GTK_ENTRY(e_type), c->type);

      GtkWidget *e_nom = lookup_widget(win_mod, "entry_mod_nom");
      if (e_nom)
        gtk_entry_set_text(GTK_ENTRY(e_nom), c->nom);

      GtkWidget *s_jour = lookup_widget(win_mod, "spinbutton_mod_jour");
      if (s_jour)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(s_jour), c->jour);

      GtkWidget *s_mois = lookup_widget(win_mod, "spinbutton_mod_mois");
      if (s_mois)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(s_mois), c->mois);

      GtkWidget *s_annee = lookup_widget(win_mod, "spinbutton_mod_annee");
      if (s_annee)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(s_annee), c->annee);

      // Parse Time (HH:MM)
      int h1_h = 0, h1_m = 0, h2_h = 0, h2_m = 0;
      sscanf(c->heure_debut, "%d:%d", &h1_h, &h1_m);
      sscanf(c->heure_fin, "%d:%d", &h2_h, &h2_m);

      GtkWidget *s_h1_h = lookup_widget(win_mod, "spin_mod_h1_h");
      if (s_h1_h)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(s_h1_h), h1_h);
      GtkWidget *s_h1_m = lookup_widget(win_mod, "spin_mod_h1_m");
      if (s_h1_m)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(s_h1_m), h1_m);

      GtkWidget *s_h2_h = lookup_widget(win_mod, "spin_mod_h2_h");
      if (s_h2_h)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(s_h2_h), h2_h);
      GtkWidget *s_h2_m = lookup_widget(win_mod, "spin_mod_h2_m");
      if (s_h2_m)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(s_h2_m), h2_m);

      GtkWidget *s_places = lookup_widget(win_mod, "spinbutton_mod_places");
      if (s_places)
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(s_places), c->places);

      GtkWidget *e_coach = lookup_widget(win_mod, "entry_mod_coach");
      if (e_coach)
        gtk_entry_set_text(GTK_ENTRY(e_coach), c->coach);

      GtkWidget *e_niv = lookup_widget(win_mod, "entry_mod_niveau");
      if (e_niv)
        gtk_entry_set_text(GTK_ENTRY(e_niv), c->niveau);

      GtkWidget *e_cat = lookup_widget(win_mod, "entry_mod_cat");
      if (e_cat)
        gtk_entry_set_text(GTK_ENTRY(e_cat), c->categorie);

      free(c);
    }
  }
}

void on_button_search_mod_clicked(GtkButton *button, gpointer user_data) {
  GtkWidget *win_mod = lookup_widget(GTK_WIDGET(button), "AD_Modifier");
  GtkWidget *entry_id = lookup_widget(win_mod, "entry_search_id_mod");
  GtkWidget *treeview = lookup_widget(win_mod, "treeview_mod");

  int id = -1;
  if (entry_id) {
    const gchar *txt = gtk_entry_get_text(GTK_ENTRY(entry_id));
    if (strlen(txt) > 0)
      id = atoi(txt);
  }
  afficher_cours_id(GTK_TREE_VIEW(treeview), id);
}

void on_button11_clicked(GtkButton *button, gpointer user_data) {
  // Open Modifier DIRECTLY with List
  GtkWidget *win_mod = create_AD_Modifier();

  // Setup Search Button
  GtkWidget *btn_search = lookup_widget(win_mod, "button_search_mod");
  if (btn_search) {
    g_signal_connect(btn_search, "clicked",
                     G_CALLBACK(on_button_search_mod_clicked), NULL);
  }

  // Setup List
  GtkWidget *treeview = lookup_widget(win_mod, "treeview_mod");
  if (treeview) {
    afficher_cours(GTK_TREE_VIEW(treeview));
    g_signal_connect(treeview, "row-activated",
                     G_CALLBACK(on_treeview_mod_row_activated), NULL);
  }

  gtk_widget_show(win_mod);
}

void on_button12_clicked(GtkButton *button, gpointer user_data) {
  // Open Supprimer (With ID search prompt)
  GtkWidget *dialog = gtk_dialog_new_with_buttons(
      "Supprimer : Entrer ID", NULL, GTK_DIALOG_MODAL, GTK_STOCK_OK,
      GTK_RESPONSE_ACCEPT, GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT, NULL);
  GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *entry = gtk_entry_new();
  gtk_container_add(GTK_CONTAINER(content),
                    gtk_label_new("ID du cours à supprimer:"));
  gtk_container_add(GTK_CONTAINER(content), entry);
  gtk_widget_show_all(dialog);

  if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
    const gchar *txt_id = gtk_entry_get_text(GTK_ENTRY(entry));
    int id = atoi(txt_id);

    if (!id_existe(id)) {
      GtkWidget *msg =
          gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_OK, "ID introuvable !");
      gtk_dialog_run(GTK_DIALOG(msg));
      gtk_widget_destroy(msg);
      gtk_widget_destroy(dialog);
      return;
    }

    Cours *c = rechercher_cours(id);
    if (c) {
      current_delete_id = id;
      GtkWidget *win_conf = create_SUPP_CONFIRMATION();

      GtkWidget *lbl = lookup_widget(win_conf, "label107");
      if (lbl) {
        char buff[256];
        sprintf(buff, "Voulez-vous vraiment supprimer le cours :\n%s (%s) ?",
                c->nom, c->type);
        gtk_label_set_text(GTK_LABEL(lbl), buff);
      }

      gtk_widget_show(win_conf);
      free(c);
    }
  }
  gtk_widget_destroy(dialog);
}

void on_button13_clicked(GtkButton *button, gpointer user_data) {
  // Open Rechercher (New Version)
  GtkWidget *win_rech = create_AD_Rechercher();
  gtk_widget_show(win_rech);
}

// --- AJOUTER ---

void on_button31_clicked(GtkButton *button, gpointer user_data) {
  // Valider Ajout
  Cours c;
  GtkWidget *win_add = lookup_widget(GTK_WIDGET(button), "AD_Ajouter");

  // 1. GET MANUAL ID from Attached Data (Fixes validation error)
  GtkWidget *entry_id =
      (GtkWidget *)g_object_get_data(G_OBJECT(win_add), "entry_id_manual");
  const gchar *txt_id = entry_id ? gtk_entry_get_text(GTK_ENTRY(entry_id)) : "";
  int id = atoi(txt_id);

  // VALIDATIONS
  if (strlen(txt_id) == 0 || id <= 0) {
    GtkWidget *msg = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
        "Veuillez entrer un ID valide (Entier > 0).");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
    return;
  }

  if (id_existe(id)) {
    GtkWidget *msg = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
        "Cet ID existe déjà ! Veuillez en choisir un autre.");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
    return;
  }

  c.id = id;

  // 2. GET OTHER FIELDS
  GtkWidget *combo_type = lookup_widget(win_add, "comboboxentry19");
  GtkWidget *entry_coach =
      lookup_widget(win_add, "entry_ajout_coach"); // NEW Entry widget
  GtkWidget *entry_nom = lookup_widget(win_add, "entry_nom_cours");
  GtkWidget *spin_jour = lookup_widget(win_add, "spinbutton_ajout_jour");
  GtkWidget *spin_mois = lookup_widget(win_add, "spinbutton_ajout_mois"); // NEW
  GtkWidget *spin_annee = lookup_widget(win_add, "spinbutton_ajout_annee");
  GtkWidget *spin_places = lookup_widget(win_add, "spinbutton15");

  // Time Inputs (Dynamic)
  GtkWidget *s_h1_h =
      (GtkWidget *)g_object_get_data(G_OBJECT(win_add), "spin_h1_h");
  GtkWidget *s_h1_m =
      (GtkWidget *)g_object_get_data(G_OBJECT(win_add), "spin_h1_m");
  GtkWidget *s_h2_h =
      (GtkWidget *)g_object_get_data(G_OBJECT(win_add), "spin_h2_h");
  GtkWidget *s_h2_m =
      (GtkWidget *)g_object_get_data(G_OBJECT(win_add), "spin_h2_m");

  // Type
  if (combo_type && GTK_IS_ENTRY(combo_type)) {
    strcpy(c.type, gtk_entry_get_text(GTK_ENTRY(combo_type)));
  } else {
    strcpy(c.type, "Autre");
  }

  // Nom
  if (entry_nom)
    strcpy(c.nom, gtk_entry_get_text(GTK_ENTRY(entry_nom)));
  else
    strcpy(c.nom, "Sans nom");

  if (strlen(c.nom) == 0) {
    GtkWidget *msg = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
                                            GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
                                            "Le nom du cours est obligatoire.");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
    return;
  }

  c.jour = spin_jour
               ? gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_jour))
               : 1;
  c.mois = spin_mois
               ? gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_mois))
               : 1;
  c.annee = spin_annee
                ? gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_annee))
                : 2024;

  // DATE VALIDATION
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int current_year = t->tm_year + 1900;
  int current_month = t->tm_mon + 1;
  int current_day = t->tm_mday;

  int invalid_date = 0;
  if (c.annee < current_year)
    invalid_date = 1;
  else if (c.annee == current_year) {
    if (c.mois < current_month)
      invalid_date = 1;
    else if (c.mois == current_month) {
      if (c.jour < current_day)
        invalid_date = 1;
    }
  }

  if (invalid_date) {
    GtkWidget *msg = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
        "Impossible d'ajouter un cours à une date passée !");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
    return;
  }
  c.places =
      spin_places
          ? gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_places))
          : 0;

  if (s_h1_h && s_h1_m)
    sprintf(c.heure_debut, "%02d:%02d",
            gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_h1_h)),
            gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_h1_m)));
  else
    strcpy(c.heure_debut, "00:00");

  if (s_h2_h && s_h2_m)
    sprintf(c.heure_fin, "%02d:%02d",
            gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_h2_h)),
            gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_h2_m)));
  else
    strcpy(c.heure_fin, "00:00");

  // Set defaults/hidden fields
  // Niveau Logic
  GtkWidget *radio_avance = lookup_widget(win_add, "radiobutton29");
  GtkWidget *radio_inter = lookup_widget(win_add, "radiobutton30");
  GtkWidget *radio_debutant = lookup_widget(win_add, "radiobutton31");

  if (radio_avance &&
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_avance))) {
    strcpy(c.niveau, "Avancé");
  } else if (radio_inter &&
             gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_inter))) {
    strcpy(c.niveau, "Intermédiaire");
  } else {
    // Default to Debutant if radio_debutant is checked or nothing valid found
    strcpy(c.niveau, "Débutant");
  }

  // Coach
  if (entry_coach) {
    strcpy(c.coach, gtk_entry_get_text(GTK_ENTRY(entry_coach)));
  } else {
    strcpy(c.coach, "CoachX");
  }

  strcpy(c.categorie, "Enfant");

  ajouter_cours(c);

  GtkWidget *msg =
      gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
                             GTK_BUTTONS_OK, "Cours ajouté avec succès !");
  gtk_dialog_run(GTK_DIALOG(msg));
  gtk_widget_destroy(msg);

  if (win_add)
    gtk_widget_destroy(win_add);
}

// --- MODIFIER ---
void on_button26_clicked(GtkButton *button, gpointer user_data) {
  GtkWidget *win_mod = lookup_widget(GTK_WIDGET(button), "AD_Modifier");
  if (win_mod)
    gtk_widget_destroy(win_mod);
}

void on_button28_clicked(GtkButton *button, gpointer user_data) {
  GtkWidget *win_mod = lookup_widget(GTK_WIDGET(button), "AD_Modifier");
  if (win_mod)
    gtk_widget_destroy(win_mod);
}

void on_button27_clicked(GtkButton *button, gpointer user_data) {
  if (current_modify_id == -1)
    return;

  GtkWidget *win_mod = lookup_widget(GTK_WIDGET(button), "AD_Modifier");
  Cours c;
  c.id = current_modify_id; // Keep same ID

  // READ FIELDS
  GtkWidget *e_type = lookup_widget(win_mod, "entry_mod_type");
  if (e_type)
    strcpy(c.type, gtk_entry_get_text(GTK_ENTRY(e_type)));

  GtkWidget *e_nom = lookup_widget(win_mod, "entry_mod_nom");
  if (e_nom)
    strcpy(c.nom, gtk_entry_get_text(GTK_ENTRY(e_nom)));

  GtkWidget *s_jour = lookup_widget(win_mod, "spinbutton_mod_jour");
  if (s_jour)
    c.jour = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_jour));

  GtkWidget *s_mois = lookup_widget(win_mod, "spinbutton_mod_mois");
  if (s_mois)
    c.mois = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_mois));

  GtkWidget *s_annee = lookup_widget(win_mod, "spinbutton_mod_annee");
  if (s_annee)
    c.annee = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_annee));

  // Time (SpinButtons)
  GtkWidget *s_h1_h = lookup_widget(win_mod, "spin_mod_h1_h");
  GtkWidget *s_h1_m = lookup_widget(win_mod, "spin_mod_h1_m");
  if (s_h1_h && s_h1_m) {
    sprintf(c.heure_debut, "%02d:%02d",
            gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_h1_h)),
            gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_h1_m)));
  } else
    strcpy(c.heure_debut, "00:00");

  GtkWidget *s_h2_h = lookup_widget(win_mod, "spin_mod_h2_h");
  GtkWidget *s_h2_m = lookup_widget(win_mod, "spin_mod_h2_m");
  if (s_h2_h && s_h2_m) {
    sprintf(c.heure_fin, "%02d:%02d",
            gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_h2_h)),
            gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_h2_m)));
  } else
    strcpy(c.heure_fin, "00:00");

  GtkWidget *s_places = lookup_widget(win_mod, "spinbutton_mod_places");
  if (s_places)
    c.places = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(s_places));

  GtkWidget *e_coach = lookup_widget(win_mod, "entry_mod_coach");
  if (e_coach)
    strcpy(c.coach, gtk_entry_get_text(GTK_ENTRY(e_coach)));

  GtkWidget *e_niv = lookup_widget(win_mod, "entry_mod_niveau");
  if (e_niv)
    strcpy(c.niveau, gtk_entry_get_text(GTK_ENTRY(e_niv)));

  GtkWidget *e_cat = lookup_widget(win_mod, "entry_mod_cat");
  if (e_cat)
    strcpy(c.categorie, gtk_entry_get_text(GTK_ENTRY(e_cat)));

  modifier_cours(current_modify_id, c);

  GtkWidget *msg =
      gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
                             GTK_BUTTONS_OK, "Cours modifié avec succès !");
  gtk_dialog_run(GTK_DIALOG(msg));
  gtk_widget_destroy(msg);

  if (win_mod)
    gtk_widget_destroy(win_mod);
  current_modify_id = -1;
}

// --- RECHERCHER ---
void on_button37_clicked(GtkButton *button, gpointer user_data) {
  GtkWidget *win = lookup_widget(GTK_WIDGET(button), "AD_Rechercher");
  if (win)
    gtk_widget_destroy(win);
}

void on_button36_clicked(GtkButton *button, gpointer user_data) {
  GtkWidget *win_rech = lookup_widget(GTK_WIDGET(button), "AD_Rechercher");
  GtkWidget *entry_id = lookup_widget(win_rech, "entry_rech_id");
  GtkWidget *treeview = lookup_widget(win_rech, "treeview_rech_liste");

  if (!win_rech || !entry_id || !treeview) {
    g_print("DEBUG: Widget lookup failed in Rechercher.\n");
    return;
  }

  const gchar *txt_id = gtk_entry_get_text(GTK_ENTRY(entry_id));
  int id = -1;
  if (strlen(txt_id) > 0)
    id = atoi(txt_id);

  afficher_cours_id(GTK_TREE_VIEW(treeview), id);
}

// --- SUPPRIMER ---
void on_button32_clicked(GtkButton *button, gpointer user_data) {
  GtkWidget *win = lookup_widget(GTK_WIDGET(button), "AD_Supprimer");
  if (win)
    gtk_widget_destroy(win);
}

void on_button33_clicked(GtkButton *button, gpointer user_data) {
  // Old delete button flow
}

void on_button34_clicked(GtkButton *button, gpointer user_data) {
  // Confirm YES
  if (current_delete_id != -1) {
    supprimer_cours(current_delete_id);

    GtkWidget *msg =
        gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
                               GTK_BUTTONS_OK, "Cours supprimé avec succès.");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);

    current_delete_id = -1;
  }
  GtkWidget *win = lookup_widget(GTK_WIDGET(button), "SUPP_CONFIRMATION");
  if (win)
    gtk_widget_destroy(win);
}

void on_button35_clicked(GtkButton *button, gpointer user_data) {
  // Confirm NO
  current_delete_id = -1;
  GtkWidget *win = lookup_widget(GTK_WIDGET(button), "SUPP_CONFIRMATION");
  if (win)
    gtk_widget_destroy(win);
}

// Stubs to fix linker errors
void on_button38_clicked(GtkButton *button, gpointer user_data) {}
void on_button39_clicked(GtkButton *button, gpointer user_data) {}
void on_button40_clicked(GtkButton *button, gpointer user_data) {}
void on_button41_clicked(GtkButton *button, gpointer user_data) {}

// Global User ID
char current_member_id[50] = "";

// --- MEMBER LOGIN ---
void on_button43_clicked(GtkButton *button, gpointer user_data) {
  // Back from Member Login
  GtkWidget *login_win = lookup_widget(GTK_WIDGET(button), "Login_membre");
  GtkWidget *main_win = create_Menu_Principal();
  gtk_widget_show(main_win);
  if (login_win)
    gtk_widget_destroy(login_win);
}

void on_button42_clicked(GtkButton *button, gpointer user_data) {
  // Connect Member
  GtkWidget *win = lookup_widget(GTK_WIDGET(button), "Login_membre");
  GtkWidget *entry_id = lookup_widget(win, "entry22");   // ID
  GtkWidget *entry_pass = lookup_widget(win, "entry21"); // Pass

  const gchar *id = gtk_entry_get_text(GTK_ENTRY(entry_id));
  const gchar *pass = gtk_entry_get_text(GTK_ENTRY(entry_pass));

  if (verifier_login_membre((char *)id, (char *)pass)) {
    // Save Current Member ID
    strcpy(current_member_id, id);

    GtkWidget *course_list = create_S_sinscrire____un_cours_();
    gtk_widget_show(course_list);
    if (win)
      gtk_widget_destroy(win);
  } else {
    GtkWidget *msg = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
        "Identifiant ou mot de passe incorrect.");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
  }
}

// --- MEMBER COURSE LIST ---

void on_button44_clicked(GtkButton *button, gpointer user_data) {
  // Back from Course List
  GtkWidget *win =
      lookup_widget(GTK_WIDGET(button), "S_sinscrire____un_cours_");
  GtkWidget *main = create_Menu_Principal();
  gtk_widget_show(main);
  if (win)
    gtk_widget_destroy(win);
}

void on_button47_clicked(GtkButton *button, gpointer user_data) {
  // Search Button in Member Area
  GtkWidget *win =
      lookup_widget(GTK_WIDGET(button), "S_sinscrire____un_cours_");
  GtkWidget *entry = lookup_widget(win, "entry23");

  const gchar *txt_id = gtk_entry_get_text(GTK_ENTRY(entry));
  int id = atoi(txt_id);

  Cours *c = rechercher_cours(id);
  if (c) {
    // Populate Views
    // Based on interface analysis:
    // textview20 (Type), textview21 (Nom)
    // textview22 (Date), textview23 (Horaire), textview24 (Duree/Info)
    // textview25 (Coach), textview26 (Type2?), textview27/28 (Cat/Niveau),
    // textview29 (Places)

    GtkWidget *tv;
    GtkTextBuffer *buf;
    char buffer[256];

    // Nom (Assume TV21 based on pos)
    tv = lookup_widget(win, "textview21");
    if (tv) {
      buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_buffer_set_text(buf, c->nom, -1);
    }

    // Type (TV20 and TV26 seem related, let's fill both or just 20)
    tv = lookup_widget(win, "textview20");
    if (tv) {
      buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_buffer_set_text(buf, c->type, -1);
    }

    // Date (TV22)
    tv = lookup_widget(win, "textview22");
    if (tv) {
      sprintf(buffer, "%02d/%02d/%d", c->jour, c->mois, c->annee);
      buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_buffer_set_text(buf, buffer, -1);
    }

    // Horaire (TV23)
    tv = lookup_widget(win, "textview23");
    if (tv) {
      sprintf(buffer, "%s - %s", c->heure_debut, c->heure_fin);
      buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_buffer_set_text(buf, buffer, -1);
    }

    // Duree (TV24)
    tv = lookup_widget(win, "textview24");
    if (tv) {
      int h1 = 0, m1 = 0, h2 = 0, m2 = 0;
      sscanf(c->heure_debut, "%d:%d", &h1, &m1);
      sscanf(c->heure_fin, "%d:%d", &h2, &m2);
      int minutes = (h2 * 60 + m2) - (h1 * 60 + m1);
      if (minutes < 0)
        minutes += 24 * 60;
      sprintf(buffer, "%d min", minutes);
      buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_buffer_set_text(buf, buffer, -1);
    }

    // Coach (TV25)
    tv = lookup_widget(win, "textview25");
    if (tv) {
      buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_buffer_set_text(buf, c->coach, -1);
    }

    // Type 2 (TV26)
    tv = lookup_widget(win, "textview26");
    if (tv) {
      buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_buffer_set_text(buf, c->type, -1);
    }

    // Niveau (TV28)
    tv = lookup_widget(win, "textview28");
    if (tv) {
      buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_buffer_set_text(buf, c->niveau, -1);
    }

    // Categorie (TV27) (Position-wise TV27/28 are near "Categorie" label 172)
    // Let's assume TV27 is category based on order
    tv = lookup_widget(win, "textview27"); // Actually label 172 is Cat
    // In interface: label172 is at 510, textview28 is at 568? Wait.
    // label172 (Cat) at Y=510. textview27 at Y=504. Match!
    if (tv) {
      buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_buffer_set_text(buf, c->categorie, -1);
    }

    // Label 173 (Niveau) at Y=576. Textview28 at Y=568. Match!
    tv = lookup_widget(win, "textview28");
    if (tv) {
      buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_buffer_set_text(buf, c->niveau, -1);
    }

    // Places (TV29)
    tv = lookup_widget(win, "textview29");
    if (tv) {
      sprintf(buffer, "%d", c->places);
      buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(tv));
      gtk_text_buffer_set_text(buf, buffer, -1);
    }

    free(c);
  } else {
    GtkWidget *msg =
        gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
                               GTK_BUTTONS_OK, "Cours introuvable !");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
  }
}

void on_button45_clicked(GtkButton *button, gpointer user_data) {
  // Subscribe
  GtkWidget *win =
      lookup_widget(GTK_WIDGET(button), "S_sinscrire____un_cours_");
  GtkWidget *entry = lookup_widget(win, "entry23");
  const gchar *txt_id = gtk_entry_get_text(GTK_ENTRY(entry));
  int course_id = atoi(txt_id);

  if (course_id <= 0) {
    GtkWidget *msg = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK,
        "Veuillez d'abord rechercher un cours valide.");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
    return;
  }

  // Verify course exists
  if (!id_existe(course_id)) {
    GtkWidget *msg =
        gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING,
                               GTK_BUTTONS_OK, "Cours invalide.");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
    return;
  }

  // Save to file
  FILE *f = fopen("inscriptions.txt", "a");
  if (f) {
    // Format: ID_Membre;ID_Cours
    fprintf(f, "%s;%d\n", current_member_id, course_id);
    fclose(f);

    GtkWidget *msg = gtk_message_dialog_new(
        NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        "Inscription enregistrée avec succès !");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
  } else {
    GtkWidget *msg =
        gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
                               GTK_BUTTONS_OK, "Erreur d'enregistrement.");
    gtk_dialog_run(GTK_DIALOG(msg));
    gtk_widget_destroy(msg);
  }
}
