#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

#include "member.h"
#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "gestion_entraineurs.h"
#include "reservation.h"
#include <time.h>
#include <ctype.h>

/* Used to remember the member ID selected for modification (search -> modify). */
static int old_id_mod = -1;

/* Current logged-in member id (set at login) */
static int current_member_id = -1;


static int genderH = 0;
static int genderF = 0;
static int genderModH = 0;
static int genderModF = 0;

static int human_ok = 0;

static int adm_filter_mode = 0;

void on_radiobutton_kh_new_hom29_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
    {
        genderModH = 1;
        genderModF = 0;
    }
}

void on_radiobutton_kh_new_fem30_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
    {
        genderModF = 1;
        genderModH = 0;
    }
}

enum
{
    COL_RES_ID = 0,
    COL_MEM_ID,
    COL_COACH_ID,
    COL_DATE,
    COL_SPORT,
    N_COLS
};

static void setup_treeview_kh_2(GtkWidget *treeview)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkListStore *store;

    if (!treeview)
        return;

    /* Si déjà initialisé (model existant), ne refais pas les colonnes */
    if (gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)) != NULL)
        return;

    /* Model */
    store = gtk_list_store_new(N_COLS,
                               G_TYPE_INT,     /* ID_RES */
                               G_TYPE_INT,     /* ID_MEM */
                               G_TYPE_INT,     /* ID_COACH */
                               G_TYPE_STRING,  /* DATE */
                               G_TYPE_STRING   /* SPORT */
                              );

    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);

    /* Colonnes */
    renderer = gtk_cell_renderer_text_new();

    column = gtk_tree_view_column_new_with_attributes("ID_RES", renderer, "text", COL_RES_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("ID_MEM", renderer, "text", COL_MEM_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("ID_COACH", renderer, "text", COL_COACH_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("DATE", renderer, "text", COL_DATE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("SPORT", renderer, "text", COL_SPORT, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

void fill_treeview_reservations(GtkWidget *treeview)
{
    GtkListStore *store;
    GtkTreeIter iter;

    if (!treeview)
        return;

    setup_treeview_kh_2(treeview);

    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview)));
    if (!store)
        return;

    gtk_list_store_clear(store);

    /* Afficher uniquement les reservations du membre connecté */
    for (int i = 0; i < nb_reservations; i++)
    {
        if (current_member_id > 0 && reservations[i].id_membre != current_member_id)
            continue;

        /* Mapping (old columns): 
           DATE  -> reservations[i].jour
           SPORT -> reservations[i].cours
        */
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           COL_RES_ID,   reservations[i].id,
                           COL_MEM_ID,   reservations[i].id_membre,
                           COL_COACH_ID, reservations[i].id_entraineur,
                           COL_DATE,     reservations[i].jour,
                           COL_SPORT,    reservations[i].cours,
                           -1);
    }
}



/* ===== Coach/Reservation TreeView (Demande coach privé) ===== */
enum
{
    TV_COACH_COL_ID,
    TV_COACH_COL_NOM,
    TV_COACH_COL_PRENOM,
    TV_COACH_COL_SEXE,
    TV_COACH_COL_SPORT,
    TV_COACH_COL_CENTRE,
    TV_COACH_COL_JOUR,
    TV_COACH_COL_H_DEB,
    TV_COACH_COL_H_FIN,
    TV_COACH_COL_PRIVE,
    TV_COACH_N_COLS
};

static int cal_sel_day = -1;
static int cal_sel_month = -1;
static int cal_sel_year = -1;

void on_button_kh_refresh_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *win = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *tv  = lookup_widget(win, "treeview_kh_2");

    fill_treeview_reservations(tv);
}


static void coach_ensure_treeview_columns(GtkWidget *treeview)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    if (!treeview) return;

    GList *cols = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
    if (cols)
    {
        g_list_free(cols);
        return;
    }

    renderer = gtk_cell_renderer_text_new();

    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", TV_COACH_COL_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", TV_COACH_COL_NOM, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Prenom", renderer, "text", TV_COACH_COL_PRENOM, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Sexe", renderer, "text", TV_COACH_COL_SEXE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Sport", renderer, "text", TV_COACH_COL_SPORT, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Centre", renderer, "text", TV_COACH_COL_CENTRE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Jour", renderer, "text", TV_COACH_COL_JOUR, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Heure Debut", renderer, "text", TV_COACH_COL_H_DEB, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Heure Fin", renderer, "text", TV_COACH_COL_H_FIN, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Privé", renderer, "text", TV_COACH_COL_PRIVE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

static const char* weekday_fr_from_calendar(int day, int month, int year)
{
    static const char *names[] = {"Dimanche","Lundi","Mardi","Mercredi","Jeudi","Vendredi","Samedi"};
    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_mday = day;
    t.tm_mon  = month - 1;
    t.tm_year = year - 1900;
    if (mktime(&t) == (time_t)-1) return "";
    if (t.tm_wday < 0 || t.tm_wday > 6) return "";
    return names[t.tm_wday];
}

static int str_ieq(const char *a, const char *b)
{
    if (!a || !b) return 0;
    while (*a && *b)
    {
        char ca = (char)tolower((unsigned char)*a);
        char cb = (char)tolower((unsigned char)*b);
        if (ca != cb) return 0;
        a++; b++;
    }
    return (*a == '\0' && *b == '\0');
}

static void fill_treeview_coach_filtered(GtkWidget *treeview,
                                        int use_gender, const char *gender,
                                        int use_sport,  const char *sport,
                                        int use_day,    const char *jour)
{
    GtkListStore *store;
    GtkTreeIter iter;

    coach_ensure_treeview_columns(treeview);

    store = gtk_list_store_new(TV_COACH_N_COLS,
                               G_TYPE_INT,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_INT,
                               G_TYPE_INT,
                               G_TYPE_INT);

    charger_entraineurs(); /* from gestion_entraineurs.c */

    int i;
    for (i = 0; i < nb_entraineurs; i++)
    {
        Entraineur *e = &entraineurs[i];

        /* show only private coaches */
        if (e->prive != 1) continue;

        if (use_gender && gender && gender[0])
        {
            if (!str_ieq(e->sexe, gender)) continue;
        }

        if (use_sport && sport && sport[0])
        {
            if (strcmp(e->specialite, sport) != 0) continue;
        }

        if (use_day && jour && jour[0])
        {
            if (!str_ieq(e->jour_dispo, jour)) continue;
        }

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,
                           TV_COACH_COL_ID, e->id,
                           TV_COACH_COL_NOM, e->nom,
                           TV_COACH_COL_PRENOM, e->prenom,
                           TV_COACH_COL_SEXE, e->sexe,
                           TV_COACH_COL_SPORT, e->specialite,
                           TV_COACH_COL_CENTRE, e->centre_sportif,
                           TV_COACH_COL_JOUR, e->jour_dispo,
                           TV_COACH_COL_H_DEB, e->heure_debut,
                           TV_COACH_COL_H_FIN, e->heure_fin,
                           TV_COACH_COL_PRIVE, e->prive,
                           -1);
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);
}

enum
{
    TV_COL_ID,
    TV_COL_NOM,
    TV_COL_PRENOM,
    TV_COL_COURS,
    TV_COL_HAUTEUR,
    TV_COL_POIDS,
    TV_COL_DATE,
    TV_COL_TEL,
    TV_COL_MAIL,
    TV_N_COLS
};

static void ensure_treeview_columns(GtkWidget *treeview)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

    if (!treeview) return;
GList *cols = gtk_tree_view_get_columns(GTK_TREE_VIEW(treeview));
if (cols)
{
    g_list_free(cols);
    return;
}


    renderer = gtk_cell_renderer_text_new();

    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text", TV_COL_ID, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text", TV_COL_NOM, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Prenom", renderer, "text", TV_COL_PRENOM, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Cours", renderer, "text", TV_COL_COURS, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Hauteur", renderer, "text", TV_COL_HAUTEUR, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Poids", renderer, "text", TV_COL_POIDS, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("JJ/MM/AAAA", renderer, "text", TV_COL_DATE, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Tel", renderer, "text", TV_COL_TEL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);

    column = gtk_tree_view_column_new_with_attributes("Mail", renderer, "text", TV_COL_MAIL, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
}

static void set_admin_filter_ui(GtkWidget *any_widget_in_window)
{
    GtkWidget *w = gtk_widget_get_toplevel(any_widget_in_window);
    GtkWidget *combo = lookup_widget(w, "combobox_kh_cour_adm3");
    GtkWidget *entry = lookup_widget(w, "entry_kh_1");

    if (combo) gtk_widget_set_sensitive(combo, adm_filter_mode == 1);
    if (entry) gtk_widget_set_sensitive(entry, adm_filter_mode == 2);
}

static void fill_treeview_filtered(GtkWidget *treeview, const char *cours_filter, const char *id_filter)
{
    GtkListStore *store;
    GtkTreeIter iter;

    ensure_treeview_columns(treeview);

    store = gtk_list_store_new(TV_N_COLS,
                               G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                               G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING,
                               G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

    FILE *f = fopen("member.txt", "r");
    if (f)
    {
        char line[512];

        while (fgets(line, sizeof(line), f))
        {
            char buf[512];
            char *save = NULL;
            char *tok = NULL;

            char id_s[64] = "", nom[64] = "", prenom[64] = "", gender[32] = "";
            char cours[64] = "", haut_s[32] = "", poid_s[32] = "", date_s[32] = "";
            char tel[64] = "", mail[128] = "", pwd[64] = "";

            strncpy(buf, line, sizeof(buf) - 1);
            buf[sizeof(buf) - 1] = '\0';

            tok = strtok_r(buf, ";\n\r", &save); if (!tok) continue; strncpy(id_s, tok, sizeof(id_s)-1);
            tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(nom, tok, sizeof(nom)-1);
            tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(prenom, tok, sizeof(prenom)-1);
            tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(gender, tok, sizeof(gender)-1);
            tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(cours, tok, sizeof(cours)-1);
            tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(haut_s, tok, sizeof(haut_s)-1);
            tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(poid_s, tok, sizeof(poid_s)-1);
            tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(date_s, tok, sizeof(date_s)-1);
            tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(tel, tok, sizeof(tel)-1);
            tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(mail, tok, sizeof(mail)-1);
            tok = strtok_r(NULL, ";\n\r", &save); if (tok) strncpy(pwd, tok, sizeof(pwd)-1);

            if (cours_filter && cours_filter[0] != '\0')
            {
                if (strcmp(cours, cours_filter) != 0) continue;
            }

            if (id_filter && id_filter[0] != '\0')
            {
                if (strcmp(id_s, id_filter) != 0) continue;
            }

            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                               TV_COL_ID, id_s,
                               TV_COL_NOM, nom,
                               TV_COL_PRENOM, prenom,
                               TV_COL_COURS, cours,
                               TV_COL_HAUTEUR, haut_s,
                               TV_COL_POIDS, poid_s,
                               TV_COL_DATE, date_s,
                               TV_COL_TEL, tel,
                               TV_COL_MAIL, mail,
                               -1);
        }

        fclose(f);
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);
}


static void ensure_admin_file(void)
{
    FILE *f = fopen("admin.txt", "r");
    if (f)
    {
        fclose(f);
        return;
    }
    f = fopen("admin.txt", "w");
    if (f)
    {
        fprintf(f, "admin 123\n");
        fclose(f);
    }
}

static int check_admin(const char *id, const char *pwd)
{
    ensure_admin_file();

    FILE *f = fopen("admin.txt", "r");
    if (!f) return 0;

    char a[64], b[64];
    int ok = 0;
    if (fscanf(f, "%63s %63s", a, b) == 2)
    {
        if (id && pwd && strcmp(id, a) == 0 && strcmp(pwd, b) == 0)
            ok = 1;
    }
    fclose(f);
    return ok;
}

static int is_digits_only(const char *s)
{
    if (!s || !*s) return 0;
    for (int i = 0; s[i]; i++)
        if (s[i] < '0' || s[i] > '9') return 0;
    return 1;
}

static int is_letters_spaces_only(const char *s)
{
    if (!s || !*s) return 0;
    for (int i = 0; s[i]; i++)
    {
        char c = s[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' '))
            return 0;
    }
    return 1;
}

static void show_error_window(GtkWidget *from_any_widget)
{
    GtkWidget *w = create_Error();
    gtk_widget_show(w);
}

static void close_top(GtkWidget *widget)
{
    GtkWidget *w = gtk_widget_get_toplevel(widget);
    if (GTK_IS_WIDGET(w))
        gtk_widget_destroy(w);
}

static void open_only(GtkWidget *current_any, GtkWidget *next)
{
    if (current_any)
    {
        GtkWidget *cur = gtk_widget_get_toplevel(current_any);
        if (GTK_IS_WIDGET(cur))
            gtk_widget_hide(cur);
    }
    gtk_widget_show(next);
}

static void fill_treeview_all(GtkWidget *treeview)
{
    GtkListStore *store;
    GtkTreeIter iter;

    store = gtk_list_store_new(10,
                               G_TYPE_INT,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_INT,
                               G_TYPE_INT,
                               G_TYPE_STRING,
                               G_TYPE_STRING,
                               G_TYPE_STRING);

    FILE *f = fopen("member.txt", "r");
    if (f)
    {
        char line[512];
        while (fgets(line, sizeof(line), f))
        {
            Member m;
            memset(&m, 0, sizeof(m));
            m.ID = -1;

            Member tmp = chercher("member.txt", -999999);
            (void)tmp;

            {
                char buf[512];
                strncpy(buf, line, sizeof(buf) - 1);
                buf[sizeof(buf) - 1] = '\0';

                char *save = NULL;
                char *tok = strtok_r(buf, ";\n\r", &save);
                if (!tok) continue;
                m.ID = atoi(tok);

                tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(m.nom, tok, sizeof(m.nom)-1);
                tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(m.prenom, tok, sizeof(m.prenom)-1);
                tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(m.gender, tok, sizeof(m.gender)-1);
                tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(m.cours, tok, sizeof(m.cours)-1);
                tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; m.taille_cm = atoi(tok);
                tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; m.poids_kg = atoi(tok);
                tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue;
                char date_str[16]; strncpy(date_str, tok, sizeof(date_str)-1); date_str[sizeof(date_str)-1]='\0';
                tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(m.phone, tok, sizeof(m.phone)-1);
                tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(m.email, tok, sizeof(m.email)-1);
                tok = strtok_r(NULL, ";\n\r", &save); if (!tok) continue; strncpy(m.pwd, tok, sizeof(m.pwd)-1);

                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter,
                                   0, m.ID,
                                   1, m.nom,
                                   2, m.prenom,
                                   3, m.gender,
                                   4, m.cours,
                                   5, m.taille_cm,
                                   6, m.poids_kg,
                                   7, date_str,
                                   8, m.phone,
                                   9, m.email,
                                   -1);
            }
        }
        fclose(f);
    }

    gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
    g_object_unref(store);
}

void on_button_kh_adm_int33_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = create_Log_In_Ges_Mem();
    open_only(GTK_WIDGET(button), w);
}

void on_button_kh_mem_int34_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = create_Log_In_Ges_Mem();
    open_only(GTK_WIDGET(button), w);
}

void on_button_kh_annul_login35_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = create_Int_Ges_Mem();
    open_only(GTK_WIDGET(button), w);
}

void on_checkbutton_kh_human_login1_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    human_ok = gtk_toggle_button_get_active(togglebutton) ? 1 : 0;
}

void on_button_kh_connecter_login36_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *e_id  = lookup_widget(GTK_WIDGET(button), "entry_kh_pass_login63");
    GtkWidget *e_pwd = lookup_widget(GTK_WIDGET(button), "entry_kh_MDP_login64");

    const char *id  = e_id  ? gtk_entry_get_text(GTK_ENTRY(e_id))  : "";
    const char *pwd = e_pwd ? gtk_entry_get_text(GTK_ENTRY(e_pwd)) : "";

    if (!human_ok)
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }

    if (check_admin(id, pwd))
    {
        GtkWidget *w = create_Int_Adm_Mem();
        open_only(GTK_WIDGET(button), w);

        GtkWidget *tv = lookup_widget(w, "treeview_kh_liste_mem1");
        adm_filter_mode = 0;
        set_admin_filter_ui(w);
        if (tv) fill_treeview_filtered(tv, NULL, NULL);

        return;
    }

    if (is_digits_only(id))
    {
        int mid = atoi(id);
        Member m = chercher("member.txt", mid);
        if (m.ID != -1 && strcmp(m.pwd, pwd) == 0)
        {
            current_member_id = mid;
            GtkWidget *w = create_Int_Membre();
            open_only(GTK_WIDGET(button), w);

            /* Fill reservations list for this member (treeview_kh_2) */
            {
                GtkWidget *tv2 = lookup_widget(w, "treeview_kh_2");
                if (tv2) fill_treeview_reservations(tv2);
            }

            return;
        }
    }

    show_error_window(GTK_WIDGET(button));
}

void on_button_kh_logout_int_adm37_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = create_Int_Ges_Mem();
    open_only(GTK_WIDGET(button), w);
}

void on_button_kh_aj_mem38_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = create_Int_Aj_Mem();
    gtk_widget_show(w);
}

void on_button_kh_Mod_Mem39_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = create_Rech_Mod_Mem();
    gtk_widget_show(w);
}

void on_button_kh_del_mem40_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = create_Rech_DEL_Mem();
    gtk_widget_show(w);
}

void on_radiobutton_kh_homme25_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
    {
        genderH = 1;
        genderF = 0;
    }
}

void on_radiobutton_kh_femme_mem26_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
    {
        genderF = 1;
        genderH = 0;
    }
}

void on_radiobutton_hom_kh_new29_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
    {
        genderModH = 1;
        genderModF = 0;
    }
}

void
on_button_kh_val_aj_mem35_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
    Member M;
    int x;

    memset(&M, 0, sizeof(M));          

    GtkWidget *input1 = lookup_widget(GTK_WIDGET(button), "entry_kh_nom_membre63");
    GtkWidget *input2 = lookup_widget(GTK_WIDGET(button), "entry_kh_prenm_mem64");
    GtkWidget *input3 = lookup_widget(GTK_WIDGET(button), "entry_kh_id_aj_mem65");
    GtkWidget *input4 = lookup_widget(GTK_WIDGET(button), "entry_kh_pwd_aj_mem66");
    GtkWidget *input5 = lookup_widget(GTK_WIDGET(button), "entry_kh_mdp_aj_mem67");

    GtkWidget *spin1  = lookup_widget(GTK_WIDGET(button), "spinbutton_kh_jr_date_mem32");
    GtkWidget *spin2  = lookup_widget(GTK_WIDGET(button), "spinbutton_kh_mois_aj33");
    GtkWidget *spin3  = lookup_widget(GTK_WIDGET(button), "spinbutton_kh_34");

    GtkWidget *input6 = lookup_widget(GTK_WIDGET(button), "entry_kh_num_mem68");
    GtkWidget *input7 = lookup_widget(GTK_WIDGET(button), "entry_kh_mail_mem69");
    GtkWidget *input8 = lookup_widget(GTK_WIDGET(button), "entry_kh_haut_mem70");
    GtkWidget *input9 = lookup_widget(GTK_WIDGET(button), "entry_kh_poid_mem71");

    GtkComboBox *combo = GTK_COMBO_BOX(
        lookup_widget(GTK_WIDGET(button), "combobox_kh_cour_mem4")
    );


    if (!input1 || !input2 || !input3 || !input4 || !input5 ||
        !spin1  || !spin2  || !spin3  ||
        !input6 || !input7 || !input8 || !input9 || !combo)
    {
        g_print("Erreur : un widget est NULL ! Vérifie les noms dans Glade.\n");
        return;
    }


    M.date_naissance.day   = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin1));
    M.date_naissance.month = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin2));
    M.date_naissance.year  = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin3));


    const char *txt_nom   = gtk_entry_get_text(GTK_ENTRY(input1));
    const char *txt_pren  = gtk_entry_get_text(GTK_ENTRY(input2));
    const char *txt_id    = gtk_entry_get_text(GTK_ENTRY(input3));
    const char *txt_pwd   = gtk_entry_get_text(GTK_ENTRY(input4));
    const char *txt_pwd2  = gtk_entry_get_text(GTK_ENTRY(input5)); 
    const char *txt_tel   = gtk_entry_get_text(GTK_ENTRY(input6));
    const char *txt_mail  = gtk_entry_get_text(GTK_ENTRY(input7));
    const char *txt_haut  = gtk_entry_get_text(GTK_ENTRY(input8));
    const char *txt_poid  = gtk_entry_get_text(GTK_ENTRY(input9));

    g_print("DEBUG: got entry texts\n");     

    M.ID       = txt_id ? atoi(txt_id) : 0;
    strcpy(M.nom,    txt_nom  ? txt_nom  : "");
    strcpy(M.prenom, txt_pren ? txt_pren : "");
    strcpy(M.pwd,    txt_pwd  ? txt_pwd  : "");

    strcpy(M.phone,  txt_tel  ? txt_tel  : "");
    strcpy(M.email,  txt_mail ? txt_mail : "");

    M.taille_cm = txt_haut ? atoi(txt_haut) : 0;
    M.poids_kg  = txt_poid ? atoi(txt_poid) : 0;


    char *cour_text = gtk_combo_box_get_active_text(combo);
    if (cour_text != NULL) {
        strcpy(M.cours, cour_text);
        g_free(cour_text);
    } else {
        strcpy(M.cours, "");
    }


    if (genderH == 1) {
        strcpy(M.gender, "homme");
    } else if (genderF == 1) {
        strcpy(M.gender, "femme");
    } else {
        strcpy(M.gender, "");
    }


    x = ajouter("member.txt", M);

    g_print("DEBUG: after ajouter(), x=%d\n", x); 


}

void on_button_kh_41_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *e = lookup_widget(GTK_WIDGET(button), "entry_id_kh_sr83");
    const char *sid = e ? gtk_entry_get_text(GTK_ENTRY(e)) : "";

    if (!is_digits_only(sid))
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }

    int id = atoi(sid);
    Member m = chercher("member.txt", id);
    if (m.ID == -1)
    {
        gtk_widget_show(create_Error());
        return;
    }

    old_id_mod = m.ID;

    GtkWidget *w = create_Modifier_Membre();
    gtk_widget_show_all(w);

    while (gtk_events_pending())
        gtk_main_iteration();

    GtkWidget *top = gtk_widget_get_toplevel(w);

    char buf[64];
    char date_buf[32];
    snprintf(date_buf, sizeof(date_buf), "%02d/%02d/%04d",
             m.date_naissance.day, m.date_naissance.month, m.date_naissance.year);

    GtkWidget *lb_nom  = lookup_widget(top, "label_kh_anc_nom145");
    GtkWidget *lb_pre  = lookup_widget(top, "label_kh_anc_pre146");
    GtkWidget *lb_id   = lookup_widget(top, "label_kh_old_id147");
    GtkWidget *lb_pwd  = lookup_widget(top, "label_kh_old_pass148");
    GtkWidget *lb_date = lookup_widget(top, "label_kh_old_date150");
    GtkWidget *lb_tel  = lookup_widget(top, "label_kh_old_num151");
    GtkWidget *lb_mail = lookup_widget(top, "label_kh_old_mail152");
    GtkWidget *lb_cour = lookup_widget(top, "label_kh_old_cour153");
    GtkWidget *lb_h    = lookup_widget(top, "label_kh_old_haut154");
    GtkWidget *lb_p    = lookup_widget(top, "label_kh_old_pd155");
    GtkWidget *lb_sex  = lookup_widget(top, "label_kh_sex_old156");

    if (lb_nom)  gtk_label_set_text(GTK_LABEL(lb_nom),  m.nom);
    if (lb_pre)  gtk_label_set_text(GTK_LABEL(lb_pre),  m.prenom);
    if (lb_id)   { snprintf(buf, sizeof(buf), "%d", m.ID); gtk_label_set_text(GTK_LABEL(lb_id), buf); }
    if (lb_pwd)  gtk_label_set_text(GTK_LABEL(lb_pwd),  m.pwd);
    if (lb_date) gtk_label_set_text(GTK_LABEL(lb_date), date_buf);
    if (lb_tel)  gtk_label_set_text(GTK_LABEL(lb_tel),  m.phone);
    if (lb_mail) gtk_label_set_text(GTK_LABEL(lb_mail), m.email);
    if (lb_cour) gtk_label_set_text(GTK_LABEL(lb_cour), m.cours);
    if (lb_h)    { snprintf(buf, sizeof(buf), "%d", m.taille_cm); gtk_label_set_text(GTK_LABEL(lb_h), buf); }
    if (lb_p)    { snprintf(buf, sizeof(buf), "%d", m.poids_kg);  gtk_label_set_text(GTK_LABEL(lb_p), buf); }

    if (lb_sex)
    {
        if (strcmp(m.gender, "homme") == 0) gtk_label_set_text(GTK_LABEL(lb_sex), "Homme");
        else if (strcmp(m.gender, "femme") == 0) gtk_label_set_text(GTK_LABEL(lb_sex), "Femme");
        else gtk_label_set_text(GTK_LABEL(lb_sex), m.gender);
    }

    GtkWidget *entry_nom       = lookup_widget(top, "entry_kh_nv_nom73");
    GtkWidget *entry_prenom    = lookup_widget(top, "entry_kh_nv_pre74");
    GtkWidget *entry_id        = lookup_widget(top, "entry_kh_new_id75");
    GtkWidget *entry_pwd       = lookup_widget(top, "entry_kh_new_pwd76");
    GtkWidget *entry_phone     = lookup_widget(top, "entry_kh_new_num78");
    GtkWidget *entry_email     = lookup_widget(top, "entry_new_mail_kh79");
    GtkWidget *entry_taille_cm = lookup_widget(top, "entry_kh_new_haut80");
    GtkWidget *entry_poids_kg  = lookup_widget(top, "entry_kh_new_pd81");

    if (entry_id)     { snprintf(buf, sizeof(buf), "%d", m.ID); gtk_entry_set_text(GTK_ENTRY(entry_id), buf); }
    if (entry_nom)    gtk_entry_set_text(GTK_ENTRY(entry_nom), m.nom);
    if (entry_prenom) gtk_entry_set_text(GTK_ENTRY(entry_prenom), m.prenom);
    if (entry_pwd)    gtk_entry_set_text(GTK_ENTRY(entry_pwd), m.pwd);
    if (entry_phone)  gtk_entry_set_text(GTK_ENTRY(entry_phone), m.phone);
    if (entry_email)  gtk_entry_set_text(GTK_ENTRY(entry_email), m.email);
    if (entry_taille_cm) { snprintf(buf, sizeof(buf), "%d", m.taille_cm); gtk_entry_set_text(GTK_ENTRY(entry_taille_cm), buf); }
    if (entry_poids_kg)  { snprintf(buf, sizeof(buf), "%d", m.poids_kg);  gtk_entry_set_text(GTK_ENTRY(entry_poids_kg), buf); }

    GtkWidget *spin_jj = lookup_widget(top, "spinbutton_kh_new_day35");
    GtkWidget *spin_mm = lookup_widget(top, "spinbutton_kh_new_month36");
    GtkWidget *spin_aa = lookup_widget(top, "spinbutton_kh_new_year37");
    if (spin_jj) gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_jj), m.date_naissance.day);
    if (spin_mm) gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_mm), m.date_naissance.month);
    if (spin_aa) gtk_spin_button_set_value(GTK_SPIN_BUTTON(spin_aa), m.date_naissance.year);

    GtkWidget *combo = lookup_widget(top, "combobox_kh_new_cour5");
    if (combo && m.cours[0])
    {
        GtkTreeModel *model = gtk_combo_box_get_model(GTK_COMBO_BOX(combo));
        GtkTreeIter iter;
        int idx = 0;
        gboolean valid = gtk_tree_model_get_iter_first(model, &iter);
        while (valid)
        {
            gchar *val = NULL;
            gtk_tree_model_get(model, &iter, 0, &val, -1);
            if (val && strcmp(val, m.cours) == 0)
            {
                gtk_combo_box_set_active(GTK_COMBO_BOX(combo), idx);
                g_free(val);
                break;
            }
            if (val) g_free(val);
            idx++;
            valid = gtk_tree_model_iter_next(model, &iter);
        }
    }

    GtkWidget *rb_h = lookup_widget(top, "radiobutton_hom_kh_new29");
    GtkWidget *rb_f = lookup_widget(top, "radiobutton_kh_new_fem30");

    if (strcmp(m.gender, "homme") == 0)
    {
        genderModH = 1; genderModF = 0;
        if (rb_h) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_h), TRUE);
    }
    else if (strcmp(m.gender, "femme") == 0)
    {
        genderModF = 1; genderModH = 0;
        if (rb_f) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rb_f), TRUE);
    }
    else
    {
        genderModH = 0; genderModF = 0;
    }
}




void on_button_valider_modif_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = gtk_widget_get_toplevel(GTK_WIDGET(button));

    Member m;
    memset(&m, 0, sizeof(m));

    GtkWidget *entry_id        = lookup_widget(w, "entry_kh_new_id75");
    GtkWidget *entry_nom       = lookup_widget(w, "entry_kh_nv_nom73");
    GtkWidget *entry_prenom    = lookup_widget(w, "entry_kh_nv_pre74");
    GtkWidget *entry_pwd       = lookup_widget(w, "entry_kh_new_pwd76");
    GtkWidget *combo_type      = lookup_widget(w, "combobox_kh_new_cour5");
    GtkWidget *spin_jj         = lookup_widget(w, "spinbutton_kh_new_day35");
    GtkWidget *spin_mm         = lookup_widget(w, "spinbutton_kh_new_month36");
    GtkWidget *spin_aa         = lookup_widget(w, "spinbutton_kh_new_year37");
    GtkWidget *entry_phone     = lookup_widget(w, "entry_kh_new_num78");
    GtkWidget *entry_email     = lookup_widget(w, "entry_new_mail_kh79");
    GtkWidget *entry_taille_cm = lookup_widget(w, "entry_kh_new_haut80");
    GtkWidget *entry_poids_kg  = lookup_widget(w, "entry_kh_new_pd81");

    if (!entry_id || !entry_nom || !entry_prenom || !entry_pwd || !combo_type ||
        !spin_jj || !spin_mm || !spin_aa ||
        !entry_phone || !entry_email || !entry_taille_cm || !entry_poids_kg)
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }

    const char *sid  = gtk_entry_get_text(GTK_ENTRY(entry_id));
    const char *nom  = gtk_entry_get_text(GTK_ENTRY(entry_nom));
    const char *pre  = gtk_entry_get_text(GTK_ENTRY(entry_prenom));
    const char *pwd  = gtk_entry_get_text(GTK_ENTRY(entry_pwd));
    const char *tel  = gtk_entry_get_text(GTK_ENTRY(entry_phone));
    const char *mail = gtk_entry_get_text(GTK_ENTRY(entry_email));
    const char *haut = gtk_entry_get_text(GTK_ENTRY(entry_taille_cm));
    const char *poid = gtk_entry_get_text(GTK_ENTRY(entry_poids_kg));

    if (!sid || !sid[0] || !is_digits_only(sid)) { show_error_window(GTK_WIDGET(button)); return; }
    if (!nom || !nom[0] || !is_letters_spaces_only(nom)) { show_error_window(GTK_WIDGET(button)); return; }
    if (!pre || !pre[0] || !is_letters_spaces_only(pre)) { show_error_window(GTK_WIDGET(button)); return; }
    if (!pwd || !pwd[0]) { show_error_window(GTK_WIDGET(button)); return; }
    if (!tel || !tel[0] || !is_digits_only(tel)) { show_error_window(GTK_WIDGET(button)); return; }
    if (!haut || !haut[0] || !is_digits_only(haut)) { show_error_window(GTK_WIDGET(button)); return; }
    if (!poid || !poid[0] || !is_digits_only(poid)) { show_error_window(GTK_WIDGET(button)); return; }

    char *cour_text = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo_type));
    if (!cour_text || cour_text[0] == '\0')
    {
        if (cour_text) g_free(cour_text);
        show_error_window(GTK_WIDGET(button));
        return;
    }

    if (!genderModH && !genderModF)
    {
        g_free(cour_text);
        show_error_window(GTK_WIDGET(button));
        return;
    }

    m.ID = atoi(sid);

    strncpy(m.nom, nom, sizeof(m.nom)-1);
    strncpy(m.prenom, pre, sizeof(m.prenom)-1);
    strncpy(m.pwd, pwd, sizeof(m.pwd)-1);

    strncpy(m.phone, tel, sizeof(m.phone)-1);
    strncpy(m.email, mail ? mail : "", sizeof(m.email)-1);

    m.taille_cm = atoi(haut);
    m.poids_kg  = atoi(poid);

    m.date_naissance.day   = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_jj));
    m.date_naissance.month = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_mm));
    m.date_naissance.year  = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_aa));

    strncpy(m.cours, cour_text, sizeof(m.cours)-1);
    g_free(cour_text);

    if (genderModH) strncpy(m.gender, "homme", sizeof(m.gender)-1);
    else            strncpy(m.gender, "femme", sizeof(m.gender)-1);

    if (!modifier("member.txt", old_id_mod, m))
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }

    gtk_widget_show(create_Conf_Add_Chang());
}



void on_button_kh_42_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *e = lookup_widget(GTK_WIDGET(button), "entry_kh_84");
    const char *sid = e ? gtk_entry_get_text(GTK_ENTRY(e)) : "";

    if (!is_digits_only(sid)) { show_error_window(GTK_WIDGET(button)); return; }

    int id = atoi(sid);
    Member m = chercher("member.txt", id);
    if (m.ID == -1) { gtk_widget_show(create_Error()); return; }

    GtkWidget *w = create_Supp_Mem();
    gtk_widget_show(w);

    g_object_set_data(G_OBJECT(w), "del_id", GINT_TO_POINTER(id));

    char buf[64];
    char date_buf[32];
    snprintf(date_buf, sizeof(date_buf), "%02d/%02d/%04d",
             m.date_naissance.day, m.date_naissance.month, m.date_naissance.year);

    GtkWidget *lb_nom  = lookup_widget(w, "label_kh173");
    GtkWidget *lb_pre  = lookup_widget(w, "label_kh_pr_del174");
    GtkWidget *lb_id   = lookup_widget(w, "label_kh_175");
    GtkWidget *lb_pwd  = lookup_widget(w, "label_kh176");
    GtkWidget *lb_date = lookup_widget(w, "label_kh177");
    GtkWidget *lb_tel  = lookup_widget(w, "label_kh178");
    GtkWidget *lb_mail = lookup_widget(w, "label_kh_179");
    GtkWidget *lb_h    = lookup_widget(w, "label_kh_181");
    GtkWidget *lb_p    = lookup_widget(w, "label_kh_182");
    GtkWidget *lb_sex  = lookup_widget(w, "label_kh_159");
    GtkWidget *lb_cour = lookup_widget(w, "label_kh_180");

    if (lb_nom)  gtk_label_set_text(GTK_LABEL(lb_nom),  m.nom);
    if (lb_pre)  gtk_label_set_text(GTK_LABEL(lb_pre),  m.prenom);

    if (lb_id)   { snprintf(buf, sizeof(buf), "%d", m.ID); gtk_label_set_text(GTK_LABEL(lb_id), buf); }
    if (lb_pwd)  gtk_label_set_text(GTK_LABEL(lb_pwd),  m.pwd);
    if (lb_date) gtk_label_set_text(GTK_LABEL(lb_date), date_buf);
    if (lb_tel)  gtk_label_set_text(GTK_LABEL(lb_tel),  m.phone);
    if (lb_mail) gtk_label_set_text(GTK_LABEL(lb_mail), m.email);
    if (lb_cour) gtk_label_set_text(GTK_LABEL(lb_cour), m.cours);

    if (lb_h) { snprintf(buf, sizeof(buf), "%d", m.taille_cm); gtk_label_set_text(GTK_LABEL(lb_h), buf); }
    if (lb_p) { snprintf(buf, sizeof(buf), "%d", m.poids_kg);  gtk_label_set_text(GTK_LABEL(lb_p), buf); }

    if (lb_sex)
    {
        if (strcmp(m.gender, "homme") == 0) gtk_label_set_text(GTK_LABEL(lb_sex), "Homme");
        else if (strcmp(m.gender, "femme") == 0) gtk_label_set_text(GTK_LABEL(lb_sex), "Femme");
        else gtk_label_set_text(GTK_LABEL(lb_sex), m.gender);
    }
}

void
on_button_kh_annul_aj_mem36_clicked (GtkButton *button, gpointer user_data)
{
    GtkWidget *w = gtk_widget_get_toplevel(GTK_WIDGET(button));
    if (w)
        gtk_widget_destroy(w);
}


void on_button_kh_43_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = gtk_widget_get_toplevel(GTK_WIDGET(button));
    gpointer p = g_object_get_data(G_OBJECT(w), "del_id");

    if (!p) { show_error_window(GTK_WIDGET(button)); return; }

    int id = GPOINTER_TO_INT(p);

    if (!supprimer("member.txt", id))
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }

    gtk_widget_show(create_Conf_Del());
    gtk_widget_destroy(w); 
}


void on_button_kh_3_clicked(GtkButton *button, gpointer user_data)
{
    close_top(GTK_WIDGET(button));
}

void on_button_kh_search_mem41_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = gtk_widget_get_toplevel(GTK_WIDGET(button));
    GtkWidget *tree = lookup_widget(w, "treeview_kh_liste_mem1");
    GtkWidget *combo = lookup_widget(w, "combobox_kh_cour_adm3");
    GtkWidget *entry = lookup_widget(w, "entry_kh_1");

    if (!tree) return;

    if (adm_filter_mode == 0)
    {
        fill_treeview_filtered(tree, NULL, NULL);
        return;
    }

    if (adm_filter_mode == 1)
    {
        char *c = combo ? gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo)) : NULL;
        if (!c || c[0] == '\0')
        {
            if (c) g_free(c);
            show_error_window(GTK_WIDGET(button));
            return;
        }
        fill_treeview_filtered(tree, c, NULL);
        g_free(c);
        return;
    }

    if (adm_filter_mode == 2)
    {
        const char *id = entry ? gtk_entry_get_text(GTK_ENTRY(entry)) : "";
        if (!id || id[0] == '\0' || !is_digits_only(id))
        {
            show_error_window(GTK_WIDGET(button));
            return;
        }
        fill_treeview_filtered(tree, NULL, id);
        return;
    }
}

void on_button_kh_49_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = create_dem_coach_prv();
    gtk_widget_show(w);
}

void on_button_kh_1_clicked(GtkButton *button, gpointer user_data) { }
void on_button_kh_2_clicked(GtkButton *button, gpointer user_data) { close_top(GTK_WIDGET(button)); }


void on_radiobutton_kh_all_adm23_group_changed(GtkRadioButton *radiobutton, gpointer user_data)
{
    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton))) return;
    adm_filter_mode = 0;
    set_admin_filter_ui(GTK_WIDGET(radiobutton));
}

void on_radiobutton_kh_spo_adm24_group_changed(GtkRadioButton *radiobutton, gpointer user_data)
{
    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton))) return;
    adm_filter_mode = 1;
    set_admin_filter_ui(GTK_WIDGET(radiobutton));
}

void on_radiobutton_kh_par_ID_27_group_changed(GtkRadioButton *radiobutton, gpointer user_data)
{
    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton))) return;
    adm_filter_mode = 2;
    set_admin_filter_ui(GTK_WIDGET(radiobutton));
}

void on_combobox_kh_cour_adm3_popup(GtkComboBox *combobox, gpointer user_data)
{
    FILE *f;
    char line[512];
    char *vals[256];
    int n = 0;

    for (int i = 0; i < 256; i++) vals[i] = NULL;

    {
        GtkTreeModel *m = gtk_combo_box_get_model(combobox);
        if (m && GTK_IS_LIST_STORE(m))
            gtk_list_store_clear(GTK_LIST_STORE(m));
    }

    f = fopen("member.txt", "r");
    if (!f) return;

    while (fgets(line, sizeof(line), f))
    {
        char buf[512];
        char *save = NULL;
        char *tok;
        int i;

        strncpy(buf, line, sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';

        tok = strtok_r(buf, ";\n\r", &save);
        for (i = 0; tok && i < 5; i++)
        {
            if (i == 4)
            {
                int exists = 0;
                for (int k = 0; k < n; k++)
                {
                    if (vals[k] && strcmp(vals[k], tok) == 0) { exists = 1; break; }
                }
                if (!exists && n < 256)
                {
                    vals[n] = strdup(tok);
                    n++;
                }
                break;
            }
            tok = strtok_r(NULL, ";\n\r", &save);
        }
    }
    fclose(f);

    for (int k = 0; k < n; k++)
    {
        if (vals[k])
        {
            gtk_combo_box_append_text(combobox, vals[k]);
            free(vals[k]);
        }
    }
}

void on_combobox_kh_cour_mem4_popup(GtkComboBox *combobox, gpointer user_data) { }

void on_button_kh_39_clicked(GtkButton *button, gpointer user_data) { on_button_valider_modif_clicked(button, user_data); }
void on_button_kh_40_clicked(GtkButton *button, gpointer user_data) { close_top(GTK_WIDGET(button)); }
void on_button_kh_4_clicked(GtkButton *button, gpointer user_data) { close_top(GTK_WIDGET(button)); }
void on_button_kh_44_clicked(GtkButton *button, gpointer user_data) { close_top(GTK_WIDGET(button)); }

void on_calendar_kh_2_day_selected(GtkCalendar *calendar, gpointer user_data)
{
    guint y, m, d;
    gtk_calendar_get_date(calendar, &y, &m, &d);
    cal_sel_year = (int)y;
    cal_sel_month = (int)m + 1;
    cal_sel_day = (int)d;
}
gboolean on_treeview_kh_1_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    return FALSE;
}

void on_button_kh_5_clicked(GtkButton *button, gpointer user_data) { close_top(GTK_WIDGET(button)); }

void on_checkbutton_kh_1_toggled(GtkToggleButton *togglebutton, gpointer user_data) { }
void on_checkbutton_kh_1_released(GtkButton *button, gpointer user_data) { }
void on_checkbutton_kh_2_toggled(GtkToggleButton *togglebutton, gpointer user_data) { }
void on_checkbutton_kh_2_released(GtkButton *button, gpointer user_data) { }
void on_checkbutton_kh_3_toggled(GtkToggleButton *togglebutton, gpointer user_data) { }
void on_checkbutton_kh_3_released(GtkButton *button, gpointer user_data) { }

void on_button_kh_46_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = gtk_widget_get_toplevel(GTK_WIDGET(button));

    GtkWidget *cb_course = lookup_widget(w, "checkbutton_kh_1");
    GtkWidget *cb_day    = lookup_widget(w, "checkbutton_kh_2");
    GtkWidget *cb_gender = lookup_widget(w, "checkbutton_kh_3");

    GtkWidget *combo_sport = lookup_widget(w, "combobox_kh_6");
    GtkWidget *cal         = lookup_widget(w, "calendar_kh_2");

    GtkWidget *rb_h = lookup_widget(w, "radiobutton_kh_33");
    GtkWidget *rb_f = lookup_widget(w, "radiobutton_kh_34");

    GtkWidget *tv = lookup_widget(w, "treeview_kh_1");
    if (!tv) tv = lookup_widget(w, "treeview_kh_2");
    if (!tv) return;

    int use_course = (cb_course && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb_course))) ? 1 : 0;
    int use_day    = (cb_day    && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb_day)))    ? 1 : 0;
    int use_gender = (cb_gender && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb_gender))) ? 1 : 0;

    const char *gender = "";
    if (use_gender)
    {
        if (rb_h && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_h))) gender = "homme";
        else if (rb_f && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(rb_f))) gender = "femme";
        else
        {
            show_error_window(GTK_WIDGET(button));
            return;
        }
    }

    char *sport = NULL;
    if (use_course)
    {
        sport = combo_sport ? gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo_sport)) : NULL;
        if (!sport || sport[0] == '\0')
{
    if (sport) g_free(sport);
    show_error_window(GTK_WIDGET(button));
    return;
}
}

if (use_day)
{
    if (cal && GTK_IS_CALENDAR(cal))
    {
        guint y, m, d;
        gtk_calendar_get_date(GTK_CALENDAR(cal), &y, &m, &d);
        cal_sel_year  = (int)y;
        cal_sel_month = (int)m + 1;
        cal_sel_day   = (int)d;
    }

    if (cal_sel_day <= 0 || cal_sel_month <= 0 || cal_sel_year <= 0)
    {
        if (sport) g_free(sport);
        show_error_window(GTK_WIDGET(button));
        return;
    }
}

const char *jour = "";
if (use_day)
    jour = weekday_fr_from_calendar(cal_sel_day, cal_sel_month, cal_sel_year);

fill_treeview_coach_filtered(
    tv,
    use_gender, gender,
    use_course, sport ? sport : "",
    use_day, jour
);

if (sport) g_free(sport);
}




/* ===== Missing callbacks stubs / input filters (CODE-V minimal) ===== */

static void entry_allow_digits_only(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position)
{
    /* Filter: allow only digits 0-9 */
    for (int i = 0; i < new_text_length; i++)
    {
        if (new_text[i] < '0' || new_text[i] > '9')
        {
            g_signal_stop_emission_by_name(editable, "insert-text");
            return;
        }
    }
}

static void entry_allow_letters_only(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position)
{
    /* Filter: allow letters + space */
    for (int i = 0; i < new_text_length; i++)
    {
        char c = new_text[i];
        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == ' '))
        {
            g_signal_stop_emission_by_name(editable, "insert-text");
            return;
        }
    }
}

void on_entry_kh_pass_login63_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_MDP_login64_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    /* Password: allow everything (do nothing) */
    (void)editable; (void)new_text; (void)new_text_length; (void)position; (void)user_data;
}

void on_entry_kh_nom_membre63_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_letters_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_prenm_mem64_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_letters_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_id_aj_mem65_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_pwd_aj_mem66_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_mdp_aj_mem67_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    /* second password field: allow everything */
    (void)editable; (void)new_text; (void)new_text_length; (void)position; (void)user_data;
}

void on_entry_kh_num_mem68_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_mail_mem69_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    /* email: allow everything */
    (void)editable; (void)new_text; (void)new_text_length; (void)position; (void)user_data;
}

void on_entry_kh_haut_mem70_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_poid_mem71_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_nv_nom73_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_letters_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_nv_pre74_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_letters_only(editable, new_text, new_text_length, position);
}

void on_spinbutton_kh_new_day35_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_spinbutton_kh_new_month36_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_spinbutton_kh_new_year37_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_new_id75_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_new_pwd76_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_new_num78_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_new_mail_kh79_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    (void)editable; (void)new_text; (void)new_text_length; (void)position; (void)user_data;
}

void on_entry_kh_new_haut80_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_new_pd81_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_id_kh_sr83_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_84_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_entry_kh_93_insert_text(GtkEditable *editable, const gchar *new_text, gint new_text_length, gint *position, gpointer user_data)
{
    entry_allow_digits_only(editable, new_text, new_text_length, position);
}

void on_combobox_kh_6_popup(GtkComboBox *combobox, gpointer user_data)
{
    /* If you fill items in Glade, do nothing. */
    (void)combobox; (void)user_data;
}

void on_radiobutton_kh_34_group_changed(GtkRadioButton *radiobutton, gpointer user_data)
{
    (void)radiobutton; (void)user_data;
}

void on_radiobutton_kh_33_group_changed(GtkRadioButton *radiobutton, gpointer user_data)
{
    (void)radiobutton; (void)user_data;
}

gboolean on_treeview_kh_liste_mem1_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    (void)widget; (void)event; (void)user_data;
    return FALSE;
}

gboolean on_treeview_kh_2_button_press_event(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
    (void)widget; (void)event; (void)user_data;
    return FALSE;
}
void on_button_kh_47_clicked(GtkButton *button, gpointer user_data)
{
    Reservation r;
    GtkWidget *w;

    GtkWidget *e_id;
    GtkWidget *cal;
    GtkWidget *combo;
    GtkWidget *cb_prive;

    const char *sid;
    int coach_id;

    guint y, m, d;
    gchar *t;

    (void)user_data;

    w = gtk_widget_get_toplevel(GTK_WIDGET(button));

    e_id     = lookup_widget(w, "entry_kh_93");
    cal      = lookup_widget(w, "calendar_kh_2");
    combo    = lookup_widget(w, "combobox_kh_6");
    cb_prive = lookup_widget(w, "checkbutton_kh_2");

    sid = e_id ? gtk_entry_get_text(GTK_ENTRY(e_id)) : "";
    if (sid == NULL || sid[0] == '\0' || !is_digits_only(sid))
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }

    if (current_member_id <= 0)
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }

    if (!cal || !GTK_IS_CALENDAR(cal))
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }

    coach_id = atoi(sid);
    if (coach_id <= 0)
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }

    memset(&r, 0, sizeof(r));
    r.id_membre     = current_member_id;
    r.id_entraineur = coach_id;

    /* Date -> jour */
    gtk_calendar_get_date(GTK_CALENDAR(cal), &y, &m, &d);
    sprintf(r.jour, "%02u/%02u/%04u", d, m + 1, y);

    /* Cours/Sport */
    r.cours[0] = '\0';
    if (combo)
    {
        t = gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo));
        if (t)
        {
            strncpy(r.cours, t, sizeof(r.cours) - 1);
            r.cours[sizeof(r.cours) - 1] = '\0';
            g_free(t);
        }
    }

    if (r.cours[0] == '\0')
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }

    /* Privé (0/1) */
    r.prive = (cb_prive && gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(cb_prive))) ? 1 : 0;

    /* Heures non présentes dans l'interface actuelle */
    r.heure_debut = 0;
    r.heure_fin   = 0;

    /* ===== prevent double booking ===== */
    if (!coach_disponible(r.id_entraineur, r.jour, r.heure_debut, r.heure_fin))
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }

    if (!membre_disponible(r.id_membre, r.jour, r.heure_debut, r.heure_fin))
    {
        show_error_window(GTK_WIDGET(button));
        return;
    }
    /* ================================= */

    ajouter_reservation(&r);

    /* confirmation */
    gtk_widget_show(create_Conf_Add_Chang());

    /* refresh if treeview exists */
    {
        GtkWidget *tv = lookup_widget(w, "treeview_kh_2");
        if (tv) fill_treeview_reservations(tv);
    }
}

void on_button_kh_48_clicked(GtkButton *button, gpointer user_data)
{
    GtkWidget *w = create_Int_Ges_Mem();
    gtk_widget_show(w);

    close_top(GTK_WIDGET(button));
}

void on_button_kh_refrech_clicked(GtkButton *button, gpointer user_data)
{
    on_button_kh_refresh_clicked(button, user_data);
}

void on_Int_Membre_show(GtkWidget *widget, gpointer user_data)
{
    GtkWidget *tv;

    (void)user_data;

    tv = lookup_widget(widget, "treeview_kh_2");
    if (tv) fill_treeview_reservations(tv);
}

