#ifndef LOGIC_H
#define LOGIC_H

#define MAXSTR 256
#define MAXVEC 1024
#define USERS_FILE "users.txt"

int exist(char *user_name);

int registerUser(char *user_name, char *ip, int port);

int unregisterUser(char *user_name);

int connectUser(const char *user_name);

int disconnectUser(const char *user_name);

int publishContent(char *user_name, char *file_name, char *description);

int deleteContent(char *user_name, char *file_name);

int listUsers();

int listContent(char *user_name);

int getFile(char *user_name, char *file_name);

#endif