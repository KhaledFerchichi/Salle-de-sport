#include "member.h"
#include <stdlib.h>

static void date_to_str(Date d, char *out, size_t n)
{
    if (!out || n == 0) return;
    snprintf(out, n, "%02d/%02d/%04d", d.day, d.month, d.year);
}

static int parse_line(const char *line, Member *m)
{
    if (!line || !m) return 0;

    char buf[512];
    strncpy(buf, line, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';

    char *tok;
    char *save = NULL;

    tok = strtok_r(buf, ";\n\r", &save);
    if (!tok) return 0;
    m->ID = atoi(tok);

    tok = strtok_r(NULL, ";\n\r", &save);
    if (!tok) return 0;
    strncpy(m->nom, tok, sizeof(m->nom) - 1);
    m->nom[sizeof(m->nom) - 1] = '\0';

    tok = strtok_r(NULL, ";\n\r", &save);
    if (!tok) return 0;
    strncpy(m->prenom, tok, sizeof(m->prenom) - 1);
    m->prenom[sizeof(m->prenom) - 1] = '\0';

    tok = strtok_r(NULL, ";\n\r", &save);
    if (!tok) return 0;
    strncpy(m->gender, tok, sizeof(m->gender) - 1);
    m->gender[sizeof(m->gender) - 1] = '\0';

    tok = strtok_r(NULL, ";\n\r", &save);
    if (!tok) return 0;
    strncpy(m->cours, tok, sizeof(m->cours) - 1);
    m->cours[sizeof(m->cours) - 1] = '\0';

    tok = strtok_r(NULL, ";\n\r", &save);
    if (!tok) return 0;
    m->taille_cm = atoi(tok);

    tok = strtok_r(NULL, ";\n\r", &save);
    if (!tok) return 0;
    m->poids_kg = atoi(tok);

    tok = strtok_r(NULL, ";\n\r", &save);
    if (!tok) return 0;
    {
        int jj = 0, mm = 0, aa = 0;
        sscanf(tok, "%d/%d/%d", &jj, &mm, &aa);
        m->date_naissance.day = jj;
        m->date_naissance.month = mm;
        m->date_naissance.year = aa;
    }

    tok = strtok_r(NULL, ";\n\r", &save);
    if (!tok) return 0;
    strncpy(m->phone, tok, sizeof(m->phone) - 1);
    m->phone[sizeof(m->phone) - 1] = '\0';

    tok = strtok_r(NULL, ";\n\r", &save);
    if (!tok) return 0;
    strncpy(m->email, tok, sizeof(m->email) - 1);
    m->email[sizeof(m->email) - 1] = '\0';

    tok = strtok_r(NULL, ";\n\r", &save);
    if (!tok) return 0;
    strncpy(m->pwd, tok, sizeof(m->pwd) - 1);
    m->pwd[sizeof(m->pwd) - 1] = '\0';

    return 1;
}

static int id_exists(const char *filename, int id)
{
    FILE *f = fopen(filename, "r");
    if (!f) return 0;

    char line[512];
    Member m;
    while (fgets(line, sizeof(line), f))
    {
        if (parse_line(line, &m) && m.ID == id)
        {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

int ajouter(const char *filename, Member m)
{
    if (!filename) return 0;
    if (m.ID <= 0) return 0;
    if (id_exists(filename, m.ID)) return 0;

    FILE *f = fopen(filename, "a");
    if (!f) return 0;

    char d[16];
    date_to_str(m.date_naissance, d, sizeof(d));

    fprintf(f, "%d;%s;%s;%s;%s;%d;%d;%s;%s;%s;%s\n",
            m.ID, m.nom, m.prenom, m.gender, m.cours,
            m.taille_cm, m.poids_kg, d, m.phone, m.email, m.pwd);

    fclose(f);
    return 1;
}

int modifier(const char *filename, int id, Member nouv)
{
    if (!filename) return 0;

    int tr = 0;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("tmp.txt", "w");
    if (!f || !f2)
    {
        if (f) fclose(f);
        if (f2) fclose(f2);
        return 0;
    }

    char line[512];
    Member cur;
    while (fgets(line, sizeof(line), f))
    {
        if (!parse_line(line, &cur))
            continue;

        if (cur.ID == id)
        {
            char d[16];
            date_to_str(nouv.date_naissance, d, sizeof(d));

            fprintf(f2, "%d;%s;%s;%s;%s;%d;%d;%s;%s;%s;%s\n",
                    nouv.ID, nouv.nom, nouv.prenom, nouv.gender, nouv.cours,
                    nouv.taille_cm, nouv.poids_kg, d, nouv.phone, nouv.email, nouv.pwd);
            tr = 1;
        }
        else
        {
            char d[16];
            date_to_str(cur.date_naissance, d, sizeof(d));

            fprintf(f2, "%d;%s;%s;%s;%s;%d;%d;%s;%s;%s;%s\n",
                    cur.ID, cur.nom, cur.prenom, cur.gender, cur.cours,
                    cur.taille_cm, cur.poids_kg, d, cur.phone, cur.email, cur.pwd);
        }
    }

    fclose(f);
    fclose(f2);

    remove(filename);
    rename("tmp.txt", filename);

    return tr;
}

int supprimer(const char *filename, int id)
{
    if (!filename) return 0;

    int tr = 0;
    FILE *f = fopen(filename, "r");
    FILE *f2 = fopen("tmp.txt", "w");
    if (!f || !f2)
    {
        if (f) fclose(f);
        if (f2) fclose(f2);
        return 0;
    }

    char line[512];
    Member cur;
    while (fgets(line, sizeof(line), f))
    {
        if (!parse_line(line, &cur))
            continue;

        if (cur.ID == id)
        {
            tr = 1;
        }
        else
        {
            char d[16];
            date_to_str(cur.date_naissance, d, sizeof(d));

            fprintf(f2, "%d;%s;%s;%s;%s;%d;%d;%s;%s;%s;%s\n",
                    cur.ID, cur.nom, cur.prenom, cur.gender, cur.cours,
                    cur.taille_cm, cur.poids_kg, d, cur.phone, cur.email, cur.pwd);
        }
    }

    fclose(f);
    fclose(f2);

    remove(filename);
    rename("tmp.txt", filename);

    return tr;
}

Member chercher(const char *filename, int id)
{
    Member m;
    memset(&m, 0, sizeof(m));
    m.ID = -1;

    if (!filename) return m;

    FILE *f = fopen(filename, "r");
    if (!f) return m;

    char line[512];
    Member cur;
    while (fgets(line, sizeof(line), f))
    {
        if (parse_line(line, &cur) && cur.ID == id)
        {
            fclose(f);
            return cur;
        }
    }

    fclose(f);
    return m;
}

