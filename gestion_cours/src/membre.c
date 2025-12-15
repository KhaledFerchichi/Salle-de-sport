#include "membre.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMBRES_FILE "membres.txt"

// Utility to remove newline check (handles \r\n)
void trim_newline(char *str) {
  int len = strlen(str);
  while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
    str[len - 1] = '\0';
    len--;
  }
}

int verifier_login_membre(char *id, char *password) {
  FILE *log = fopen("login_debug.txt", "a");
  if (log)
    fprintf(log, "Login attempt: ID='%s', Pass='%s'\n", id, password);

  FILE *f = fopen(MEMBRES_FILE, "r");
  if (!f) {
    if (log) {
      fprintf(log, "Error: Could not open %s\n", MEMBRES_FILE);
      fclose(log);
    }
    return 0;
  }

  char line[512];
  char f_id[50], f_nom[50], f_prenom[50], f_sexe[20], f_email[100];
  char f_abo[50], f_sports[50], f_pass[50];
  int f_j, f_m, f_a;

  while (fgets(line, sizeof(line), f)) {
    trim_newline(line);
    if (log)
      fprintf(log, "Reading line: '%s'\n", line);

    // Parse CSV: ID;Nom;Prenom;Sexe;Email;Abo;Sports;J;M;A;Pass
    int count =
        sscanf(line, "%[^;];%[^;];%[^;];%[^;];%[^;];%[^;];%[^;];%d;%d;%d;%s",
               f_id, f_nom, f_prenom, f_sexe, f_email, f_abo, f_sports, &f_j,
               &f_m, &f_a, f_pass);

    if (log)
      fprintf(log, "Parsed ID='%s', Pass='%s', Count=%d\n", f_id, f_pass,
              count);

    if (strcmp(f_id, id) == 0 && strcmp(f_pass, password) == 0) {
      if (log)
        fprintf(log, "Authentication SUCCESS\n");
      fclose(f);
      if (log)
        fclose(log);
      return 1;
    }
  }
  if (log)
    fprintf(log, "Authentication FAILED - No match found\n");
  fclose(f);
  if (log)
    fclose(log);
  return 0;
}

void ajouter_membre(Membre m) {
  FILE *f = fopen(MEMBRES_FILE, "a");
  if (f) {
    fprintf(f, "%s;%s;%s;%s;%s;%s;%s;%d;%d;%d;%s\n", m.id, m.nom, m.prenom,
            m.sexe, m.email, m.abonnement, m.sports, m.jour, m.mois, m.annee,
            m.password);
    fclose(f);
  }
}

int membre_existe(char *id) {
  FILE *f = fopen(MEMBRES_FILE, "r");
  if (!f)
    return 0;

  char line[512];
  char f_id[50];

  while (fgets(line, sizeof(line), f)) {
    // Just peek at the first token (ID)
    char *token = strtok(line, ";");
    if (token && strcmp(token, id) == 0) {
      fclose(f);
      return 1;
    }
  }
  fclose(f);
  return 0;
}

Membre *rechercher_membre(char *id) {
  FILE *f = fopen(MEMBRES_FILE, "r");
  if (!f)
    return NULL;

  char line[512];
  // Allocate memory for result
  Membre *m = malloc(sizeof(Membre));
  if (!m) {
    fclose(f);
    return NULL;
  }

  while (fgets(line, sizeof(line), f)) {
    trim_newline(line);
    char f_id[50];
    // Read ID first to check match
    sscanf(line, "%[^;]", f_id);

    if (strcmp(f_id, id) == 0) {
      // Re-parse full line
      sscanf(line, "%[^;];%[^;];%[^;];%[^;];%[^;];%[^;];%[^;];%d;%d;%d;%s",
             m->id, m->nom, m->prenom, m->sexe, m->email, m->abonnement,
             m->sports, &m->jour, &m->mois, &m->annee, m->password);
      fclose(f);
      return m;
    }
  }
  free(m);
  fclose(f);
  return NULL;
}
