#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logic.h"

#define USERS_FILE "users.txt"
#define MAX_LINE 1024

int createUser(user *usr, char *name, char *ip, int port) {
    if (strlen(name) < 1 || strlen(name) > MAXSTR) return -1;
    if (strlen(ip) < 1 || strlen(ip) > MAXSTR) return -1;

    strcpy(usr->name, name);
    usr->conected = 0;
    usr->port = port;
    strcpy(usr->ip, ip);
    usr->contentsLen = 0;
    usr->contentsMaxLen = 0;
    usr->contents = NULL;

    return 0;
}

int userExists(char *username) {
    FILE *fp = fopen(USERS_FILE, "r");
    if (!fp) return 0;

    char name[MAXSTR];
    while (fscanf(fp, "%s", name) == 1) {
        if (strcmp(name, username) == 0) {
            fclose(fp);
            return 1;
        }
        // skip rest of line
        fgets(name, sizeof(name), fp);
    }

    fclose(fp);
    return 0;
}

int addUser(user_list users, char *user_name, char *ip, int port) {
    if (userExists(user_name)) return 1;

    FILE *fp = fopen(USERS_FILE, "a");
    if (!fp) return -1;

    fprintf(fp, "%s %s %d 0\n", user_name, ip, port); // 0 = desconectado
    fclose(fp);

    // Crear fichero vacío para los contenidos
    char fname[MAXSTR + 64];
    sprintf(fname, "contents_%s.txt", user_name);
    FILE *fc = fopen(fname, "w");
    if (fc) fclose(fc);

    return 0;
}

int removeUser(user_list users, char *user_name) {
    FILE *fp = fopen(USERS_FILE, "r");
    if (!fp) return -1;

    FILE *tmp = fopen("temp.txt", "w");
    if (!tmp) {
        fclose(fp);
        return -1;
    }

    char name[MAXSTR], ip[32];
    int port, connected;
    int found = 0;

    while (fscanf(fp, "%s %s %d %d", name, ip, &port, &connected) == 4) {
        if (strcmp(name, user_name) != 0) {
            fprintf(tmp, "%s %s %d %d\n", name, ip, port, connected);
        } else {
            found = 1;
        }
    }

    fclose(fp);
    fclose(tmp);

    remove(USERS_FILE);
    rename("temp.txt", USERS_FILE);

    // Eliminar archivo de contenidos
    char fname[MAXSTR + 64];
    sprintf(fname, "contents_%s.txt", user_name);
    remove(fname);

    return found ? 0 : -1;
}

int searchUser(user_list users, char *user_name) {
    FILE *fp = fopen(USERS_FILE, "r");
    if (!fp) return -1;

    char name[MAXSTR];
    int index = 0;
    while (fscanf(fp, "%s", name) == 1) {
        if (strcmp(name, user_name) == 0) {
            fclose(fp);
            return index;
        }
        fgets(name, sizeof(name), fp);
        index++;
    }

    fclose(fp);
    return -1;
}

int addContent(user_list users, char *user_name, char *file_name, char *description) {
    if (!userExists(user_name)) return 1;

    char fname[MAXSTR + 64];
    sprintf(fname, "contents_%s.txt", user_name);

    // Revisar si ya existe
    FILE *fp = fopen(fname, "r");
    if (!fp) return -1;

    char filebuf[MAXSTR], descbuf[MAXSTR];
    while (fscanf(fp, "%s %[^\n]", filebuf, descbuf) == 2) {
        if (strcmp(filebuf, file_name) == 0) {
            fclose(fp);
            return 3;  // Ya existe
        }
    }
    fclose(fp);

    // Agregar contenido
    fp = fopen(fname, "a");
    if (!fp) return -1;

    fprintf(fp, "%s %s\n", file_name, description);
    fclose(fp);
    return 0;
}

int removeContent(user_list users, char *user_name, char *file_name) {
    if (!userExists(user_name)) return 1;

    char fname[MAXSTR + 64];
    sprintf(fname, "contents_%s.txt", user_name);

    FILE *fp = fopen(fname, "r");
    if (!fp) return -1;

    FILE *tmp = fopen("tmp_content.txt", "w");
    if (!tmp) {
        fclose(fp);
        return -1;
    }

    char f[MAXSTR], desc[MAXSTR];
    int found = 0;
    while (fscanf(fp, "%s %[^\n]", f, desc) == 2) {
        if (strcmp(f, file_name) != 0) {
            fprintf(tmp, "%s %s\n", f, desc);
        } else {
            found = 1;
        }
    }

    fclose(fp);
    fclose(tmp);
    remove(fname);
    rename("tmp_content.txt", fname);

    return found ? 0 : 3;
}

int destroyList(user_list users) {
    // No se necesita liberar memoria, ya que no hay estructuras dinámicas
    return 0;
}
