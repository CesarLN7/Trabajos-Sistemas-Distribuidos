#ifndef STORAGE_H
#define STORAGE_H

#define MAXSTR 256
#define MAXVEC 1024

typedef struct {
    char name[MAXSTR];
    char description[MAXSTR];
} file;

/**
 * Agrega un usuario al fichero de usuarios. También crea su fichero de contenidos.
 *
 * @param user_name nombre del usuario
 * @param ip dirección IP
 * @param port puerto
 * @return 0 éxito, 1 si ya existe, -1 en error de fichero
 */
int registerUser(char *user_name, char *ip, int port);

/**
 * Elimina un usuario y su fichero de contenidos.
 *
 * @param user_name nombre del usuario
 * @return 0 si éxito, -1 si error o usuario no encontrado
 */
int unregisterUser(char *user_name);

/**
 * Busca un usuario en el fichero de usuarios.
 *
 * @param user_name nombre del usuario
 * @return índice si se encuentra, -1 si no
 */
int searchUser(char *user_name);

/**
 * Añade un contenido al fichero del usuario.
 *
 * @param user_name nombre del usuario
 * @param file_name nombre del archivo
 * @param description descripción del archivo
 * @return 0 éxito, 1 si el usuario no existe, 3 si el archivo ya existe, -1 error de fichero
 */
int publish(char *user_name, char* file_name, char *description);

/**
 * Elimina un contenido del fichero del usuario.
 *
 * @param user_name nombre del usuario
 * @param file_name nombre del archivo
 * @return 0 éxito, 3 si el archivo no existe, -1 error
 */
int deleteContent(char *user_name, char* file_name);

/**
 * Checks whether a user is connected.
 * 
 * @param user_name user name
 * 
 * @return 0 if connected, 1 if not connected, -1 if user doesn't exist
*/
int isConnected(const char *user_name);

/**
 * Modifies the connection status of a user in the file system.
 *
 * @param user_name user name
 *
 * @return 0 if updated, 1 if already in that state, -1 if user not found or error
 */
int connectUser(const char *user_name);

/**
 * Modifies the disconnection status of a user in the file system.
 *
 * @param user_name user name
 *
 * @return 0 if updated, 1 if already in that state, -1 if user not found or error
 */
int disconnectUser(const char *user_name);

#endif
