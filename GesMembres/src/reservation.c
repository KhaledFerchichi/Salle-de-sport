#include "reservation.h"

Reservation reservations[MAX_RESERVATIONS];
int nb_reservations = 0;
int dernier_id_res = 0;

void charger_reservations(void)
{
    FILE *f;
    char ligne[256];

    nb_reservations = 0;
    dernier_id_res = 0;

    f = fopen(FICHIER_RESERVATIONS, "r");
    if (f == NULL)
        return;

    while (fgets(ligne, sizeof(ligne), f))
    {
        Reservation r;
        char *token;
        char *rest = ligne;

        token = strtok_r(rest, "|", &rest);
        if (!token) continue;
        r.id = atoi(token);

        token = strtok_r(NULL, "|", &rest);
        if (!token) continue;
        r.id_membre = atoi(token);

        token = strtok_r(NULL, "|", &rest);
        if (!token) continue;
        r.id_entraineur = atoi(token);

        token = strtok_r(NULL, "|", &rest);
        if (!token) continue;
        strcpy(r.cours, token);

        token = strtok_r(NULL, "|", &rest);
        if (!token) continue;
        strcpy(r.jour, token);

        token = strtok_r(NULL, "|", &rest);
        if (!token) continue;
        r.heure_debut = atoi(token);

        token = strtok_r(NULL, "|", &rest);
        if (!token) continue;
        r.heure_fin = atoi(token);

        token = strtok_r(NULL, "|\n", &rest);
        if (!token) continue;
        r.prive = atoi(token);

        reservations[nb_reservations++] = r;

        if (r.id > dernier_id_res)
            dernier_id_res = r.id;
    }

    fclose(f);
}

void sauvegarder_reservations(void)
{
    FILE *f;
    int i;

    f = fopen(FICHIER_RESERVATIONS, "w");
    if (f == NULL)
        return;

    for (i = 0; i < nb_reservations; i++)
    {
        fprintf(f, "%d|%d|%d|%s|%s|%d|%d|%d\n",
                reservations[i].id,
                reservations[i].id_membre,
                reservations[i].id_entraineur,
                reservations[i].cours,
                reservations[i].jour,
                reservations[i].heure_debut,
                reservations[i].heure_fin,
                reservations[i].prive);
    }

    fclose(f);
}

void ajouter_reservation(Reservation *r)
{
    if (nb_reservations >= MAX_RESERVATIONS)
        return;

    r->id = ++dernier_id_res;
    reservations[nb_reservations++] = *r;

    sauvegarder_reservations();
}

Reservation* rechercher_reservation(int id)
{
    int i;

    for (i = 0; i < nb_reservations; i++)
    {
        if (reservations[i].id == id)
            return &reservations[i];
    }

    return NULL;
}

void modifier_reservation(int id, Reservation *r)
{
    int i;

    for (i = 0; i < nb_reservations; i++)
    {
        if (reservations[i].id == id)
        {
            r->id = id;
            reservations[i] = *r;
            sauvegarder_reservations();
            return;
        }
    }
}

void supprimer_reservation(int id)
{
    int i, j;

    for (i = 0; i < nb_reservations; i++)
    {
        if (reservations[i].id == id)
        {
            for (j = i; j < nb_reservations - 1; j++)
                reservations[j] = reservations[j + 1];

            nb_reservations--;
            sauvegarder_reservations();
            return;
        }
    }
}

void supprimer_reservations_entraineur(int id_entraineur)
{
    int i = 0;

    while (i < nb_reservations)
    {
        if (reservations[i].id_entraineur == id_entraineur)
        {
            int j;
            for (j = i; j < nb_reservations - 1; j++)
                reservations[j] = reservations[j + 1];

            nb_reservations--;
        }
        else
        {
            i++;
        }
    }

    sauvegarder_reservations();
}

static int overlap(int a_deb, int a_fin, int b_deb, int b_fin)
{
    if ((a_deb == 0 && a_fin == 0) || (b_deb == 0 && b_fin == 0))
        return 1;

    if (a_fin < a_deb) { int t = a_deb; a_deb = a_fin; a_fin = t; }
    if (b_fin < b_deb) { int t = b_deb; b_deb = b_fin; b_fin = t; }

    return (a_deb < b_fin && a_fin > b_deb);
}

int coach_disponible(int id_entraineur, const char *jour, int hdeb, int hfin)
{
    int i;

    if (jour == NULL || jour[0] == '\0')
        return 0;

    for (i = 0; i < nb_reservations; i++)
    {
        if (reservations[i].id_entraineur == id_entraineur &&
            strcmp(reservations[i].jour, jour) == 0)
        {
            if (overlap(hdeb, hfin, reservations[i].heure_debut, reservations[i].heure_fin))
                return 0;
        }
    }

    return 1;
}

int membre_disponible(int id_membre, const char *jour, int hdeb, int hfin)
{
    int i;

    if (jour == NULL || jour[0] == '\0')
        return 0;

    for (i = 0; i < nb_reservations; i++)
    {
        if (reservations[i].id_membre == id_membre &&
            strcmp(reservations[i].jour, jour) == 0)
        {
            if (overlap(hdeb, hfin, reservations[i].heure_debut, reservations[i].heure_fin))
                return 0;
        }
    }

    return 1;
}


