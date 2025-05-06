#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logic.h"

#define USERS_FILE "users.txt"
#define MAX_LINE 1024

int exist(char *username) {
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

int registerUser(char *user_name, char *ip, int port) {
    if (exist(user_name)) return 1;

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

int unregisterUser(char *user_name) {
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

int searchUser(char *user_name) {
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

int publish(char *user_name, char *file_name, char *description) {
    if (!exist(user_name)) return 1;

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

int deleteContent(char *user_name, char *file_name) {
    if (!exist(user_name)) return 1;

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

int isConnected(const char *user_name) {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) {
        return -1;  // No se puede abrir el archivo
    }

    char name[MAXSTR], ip[32];
    int port, connected;

    while (fscanf(fp, "%s %s %d %d", name, ip, &port, &connected) == 4) {
        if (strcmp(name, user_name) == 0) {
            fclose(fp);
            return connected ? 0 : 1;
        }
    }

    fclose(fp);
    return -1; // Usuario no encontrado
}

int connect(const char *user_name) {
    FILE *fp = fopen("users.txt", "r");
    if (!fp) return -1;

    FILE *temp = fopen("users_temp.txt", "w");
    if (!temp) {
        fclose(fp);
        return -1;
    }

    char name[MAXSTR], ip[32];
    int port, connected;
    int found = 0;
    int connected_status = 0;

    while (fscanf(fp, "%s %s %d %d", name, ip, &port, &connected) == 4) {
        if (strcmp(name, user_name) == 0) {
            found = 1;
            if (connected == 1) {
                connected_status = 1; // Ya estaba conectado
            } else {
                connected = 1; // Cambiar a conectado
            }
        }
        fprintf(temp, "%s %s %d %d\n", name, ip, port, connected);
    }

    fclose(fp);
    fclose(temp);

    if (!found) {
        remove("users_temp.txt");
        return -1; // Usuario no encontrado
    }

    remove("users.txt");
    rename("users_temp.txt", "users.txt");
    if (connected_status == 1) {
        return 1; // Ya estaba conectado
    } else {
        return 0; // Conectado exitosamente
    }
}
