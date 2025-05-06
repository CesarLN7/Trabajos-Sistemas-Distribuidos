#ifndef STORAGE_H
#define STORAGE_H

#define MAXSTR 256

typedef struct {
    char name[MAXSTR];
    char description[MAXSTR];
} file;

typedef struct {
    char name[MAXSTR];
    file* contents;
    int contentsLen;
    int contentsMaxLen;
    int conected;
    int port;
    char ip[32];
} user;

typedef struct {
    int size;
    int max_size;
    user* users;
} __user_list;

typedef __user_list* user_list;

/**
 * Crea un usuario en memoria, solo útil si quieres operar con datos temporales.
 *
 * @param usr puntero a la estructura user
 * @param name nombre del usuario
 * @param ip dirección IP
 * @param port puerto
 * @return 0 en caso de éxito, -1 en caso de error
 */
int createUser(user *usr, char *name, char *ip, int port);

/**
 * Agrega un usuario al fichero de usuarios. También crea su fichero de contenidos.
 *
 * @param users ignorado, pero mantenido por compatibilidad
 * @param user_name nombre del usuario
 * @param ip dirección IP
 * @param port puerto
 * @return 0 éxito, 1 si ya existe, -1 en error de fichero
 */
int addUser(user_list users, char *user_name, char *ip, int port);

/**
 * Elimina un usuario y su fichero de contenidos.
 *
 * @param users ignorado
 * @param user_name nombre del usuario
 * @return 0 si éxito, -1 si error o usuario no encontrado
 */
int removeUser(user_list users, char *user_name);

/**
 * Busca un usuario en el fichero de usuarios.
 *
 * @param users ignorado
 * @param user_name nombre del usuario
 * @return índice si se encuentra, -1 si no
 */
int searchUser(user_list users, char *user_name);

/**
 * Añade un contenido al fichero del usuario.
 *
 * @param users ignorado
 * @param user_name nombre del usuario
 * @param file_name nombre del archivo
 * @param description descripción del archivo
 * @return 0 éxito, 1 si el usuario no existe, 3 si el archivo ya existe, -1 error de fichero
 */
int addContent(user_list users, char *user_name, char* file_name, char *description);

/**
 * Elimina un contenido del fichero del usuario.
 *
 * @param users ignorado
 * @param user_name nombre del usuario
 * @param file_name nombre del archivo
 * @return 0 éxito, 3 si el archivo no existe, -1 error
 */
int removeContent(user_list users, char *user_name, char* file_name);

/**
 * Destruye la lista. No hace nada en este contexto.
 *
 * @param users ignorado
 * @return siempre 0
 */
int destroyList(user_list users);

#endif
