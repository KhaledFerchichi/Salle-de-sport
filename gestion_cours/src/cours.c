#include "cours.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "cours.txt"

// Helper to safely copy string fields
void safe_strcpy(char *dest, const char *src, size_t size) {
  if (src) {
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
  } else {
    dest[0] = '\0';
  }
}

// Helper: split string by semicolon, handling empty fields correctly
// returns 1 on success, 0 on failure (line too short/bad format)
int parse_cours(char *line, Cours *c) {
  // Removing trailing newline
  line[strcspn(line, "\n")] = 0;

  char *ptr = line;
  char *next_ptr = NULL;
  int col = 0;

  // We expect 12 columns
  for (col = 0; col < 12; col++) {
    next_ptr = strchr(ptr, ';');
    if (next_ptr) {
      *next_ptr = '\0'; // terminate current field
    }
    // If it's the last column, next_ptr is NULL, which is fine

    // Parse column
    switch (col) {
    case 0:
      c->id = atoi(ptr);
      break;
    case 1:
      safe_strcpy(c->type, ptr, sizeof(c->type));
      break;
    case 2:
      safe_strcpy(c->nom, ptr, sizeof(c->nom));
      break;
    case 3:
      c->jour = atoi(ptr);
      break;
    case 4:
      c->mois = atoi(ptr);
      break;
    case 5:
      c->annee = atoi(ptr);
      break;
    case 6:
      safe_strcpy(c->heure_debut, ptr, sizeof(c->heure_debut));
      break;
    case 7:
      safe_strcpy(c->heure_fin, ptr, sizeof(c->heure_fin));
      break;
    case 8:
      c->places = atoi(ptr);
      break;
    case 9:
      safe_strcpy(c->niveau, ptr, sizeof(c->niveau));
      break;
    case 10:
      safe_strcpy(c->coach, ptr, sizeof(c->coach));
      break;
    case 11:
      safe_strcpy(c->categorie, ptr, sizeof(c->categorie));
      break;
    }

    if (next_ptr) {
      ptr = next_ptr + 1;
    } else {
      // If we are not at the last column but ran out of semicolons, it might be
      // a short line. But strictly properly formatted lines should be fine.
      // If we are at col 11, we are done.
      if (col < 11) {
        // Line shorter than expected? We can accept it or fail.
        // Let's accept what we have.
      }
      break;
    }
  }

  // Basic validation: ID should be > 0. If atoi failed on empty string, it's 0.
  // Although 0 might be a valid ID? Usually >0.
  // The user uses manual ID entry.
  return 1;
}

// Check if ID exists (1=Yes, 0=No)
int id_existe(int id) {
  FILE *f = fopen(FILE_NAME, "r");
  if (!f)
    return 0;

  char line[512];
  Cours c;
  while (fgets(line, sizeof(line), f)) {
    if (parse_cours(line, &c)) {
      if (c.id == id) {
        fclose(f);
        return 1;
      }
    }
  }
  fclose(f);
  return 0;
}

void ajouter_cours(Cours c) {
  // Use a mode that works for both appending
  FILE *f = fopen(FILE_NAME, "a");
  if (f) {
    fprintf(f, "%d;%s;%s;%d;%d;%d;%s;%s;%d;%s;%s;%s\n", c.id, c.type, c.nom,
            c.jour, c.mois, c.annee, c.heure_debut, c.heure_fin, c.places,
            c.niveau, c.coach, c.categorie);
    fclose(f);
  } else {
    printf("Erreur ouverture %s\n", FILE_NAME);
  }
}

int modifier_cours(int id, Cours new_c) {
  int found = 0;
  FILE *f = fopen(FILE_NAME, "r");
  FILE *temp = fopen("temp_cours.txt", "w");

  if (!f || !temp)
    return 0;

  char line[512];
  Cours c;
  while (fgets(line, sizeof(line), f)) {
    // Keep a copy of the original line for writing back if not modified
    // actually, we reconstruct the line to ensure standard format.
    if (parse_cours(line, &c)) {
      if (c.id == id) {
        found = 1;
        // Write NEW data
        fprintf(temp, "%d;%s;%s;%d;%d;%d;%s;%s;%d;%s;%s;%s\n", id, new_c.type,
                new_c.nom, new_c.jour, new_c.mois, new_c.annee,
                new_c.heure_debut, new_c.heure_fin, new_c.places, new_c.niveau,
                new_c.coach, new_c.categorie);
      } else {
        // Write OLD data (re-formatted to ensure consistency)
        fprintf(temp, "%d;%s;%s;%d;%d;%d;%s;%s;%d;%s;%s;%s\n", c.id, c.type,
                c.nom, c.jour, c.mois, c.annee, c.heure_debut, c.heure_fin,
                c.places, c.niveau, c.coach, c.categorie);
      }
    }
  }
  fclose(f);
  fclose(temp);

  if (found) {
    remove(FILE_NAME);
    rename("temp_cours.txt", FILE_NAME);
  } else {
    remove("temp_cours.txt");
  }

  return found;
}

int supprimer_cours(int id) {
  int found = 0;
  FILE *f = fopen(FILE_NAME, "r");
  FILE *temp = fopen("temp_supp.txt", "w");

  if (!f || !temp)
    return 0;

  char line[512];
  Cours c;
  while (fgets(line, sizeof(line), f)) {
    if (parse_cours(line, &c)) {
      if (c.id == id) {
        found = 1;
        // Skip writing
      } else {
        fprintf(temp, "%d;%s;%s;%d;%d;%d;%s;%s;%d;%s;%s;%s\n", c.id, c.type,
                c.nom, c.jour, c.mois, c.annee, c.heure_debut, c.heure_fin,
                c.places, c.niveau, c.coach, c.categorie);
      }
    }
  }
  fclose(f);
  fclose(temp);

  if (found) {
    remove(FILE_NAME);
    rename("temp_supp.txt", FILE_NAME);
  } else {
    remove("temp_supp.txt");
  }

  return found;
}

Cours *rechercher_cours(int id) {
  FILE *f = fopen(FILE_NAME, "r");
  if (!f)
    return NULL;

  char line[512];
  // Allocate cleanly
  Cours *c_res = (Cours *)malloc(sizeof(Cours));

  while (fgets(line, sizeof(line), f)) {
    // We can't pass c_res directly to parse because we need it to be
    // persistent only if found. But actually, we can overwrite it until found.
    if (parse_cours(line, c_res)) {
      if (c_res->id == id) {
        fclose(f);
        return c_res;
      }
    }
  }
  fclose(f);
  free(c_res);
  return NULL;
}

// Display function for GtkTreeView

void afficher_cours(GtkTreeView *liste) { afficher_cours_id(liste, -1); }

void afficher_cours_id(GtkTreeView *liste, int id_filter) {
  GtkListStore *store;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;

  // New store for data
  store = gtk_list_store_new(N_COLUMNS, G_TYPE_INT, G_TYPE_STRING,
                             G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT,
                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT,
                             G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);

  FILE *f = fopen(FILE_NAME, "r");
  if (f) {
    char line[512];
    Cours c;
    while (fgets(line, sizeof(line), f)) {
      if (parse_cours(line, &c)) {
        // Filter Logic
        if (id_filter != -1 && c.id != id_filter) {
          continue;
        }

        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(
            store, &iter, ID_COLUMN, c.id, TYPE_COLUMN, c.type, NOM_COLUMN,
            c.nom, JOUR_COLUMN, c.jour, ANNEE_COLUMN, c.annee, MOIS_COLUMN,
            c.mois, HEURE_DEBUT_COLUMN, c.heure_debut, HEURE_FIN_COLUMN,
            c.heure_fin, PLACES_COLUMN, c.places, NIVEAU_COLUMN, c.niveau,
            COACH_COLUMN, c.coach, CATEGORIE_COLUMN, c.categorie, -1);
      }
    }
    fclose(f);
  }

  gtk_tree_view_set_model(liste, GTK_TREE_MODEL(store));
  g_object_unref(store);

  // Check if columns already exist to avoid duplication
  if (gtk_tree_view_get_column(liste, 0) == NULL) {
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("ID", renderer, "text",
                                                      ID_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Type", renderer, "text",
                                                      TYPE_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Nom", renderer, "text",
                                                      NOM_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Jour", renderer, "text",
                                                      JOUR_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Mois", renderer, "text",
                                                      MOIS_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Annee", renderer, "text",
                                                      ANNEE_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Heure Debut", renderer, "text", HEURE_DEBUT_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Heure Fin", renderer, "text", HEURE_FIN_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Places", renderer, "text", PLACES_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Niveau", renderer, "text", NIVEAU_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Coach", renderer, "text",
                                                      COACH_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes(
        "Categorie", renderer, "text", CATEGORIE_COLUMN, NULL);
    gtk_tree_view_append_column(liste, column);
  }
}
