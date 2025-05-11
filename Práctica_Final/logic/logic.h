#ifndef LOGIC_H
#define LOGIC_H

#define MAXSTR 256
#define USERS_FILE "users.txt"

int exist(char *user_name);

int updateUserIPPort(const char* user, const char* ip, int port);

int registerUser(char *user_name, char *ip, int port);

int unregisterUser(char *user_name);

int connectUser(const char *user_name);

int disconnectUser(const char *user_name);

int publishContent(char *user_name, char *file_name, char *description);

int deleteContent(char *user_name, char *file_name);

#endif