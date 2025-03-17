#ifndef _MENSAJE_H_
#define _MENSAJE_H_

#define MAXSTR 256
#define MAXVEC 32

// Estructura para la comunicación mediante colas de mensajes.

struct message {
    int op;                     // Código de operación (1: destroy, 2: set_value, 3: get_value, 4: modify_value, 5: delete_key, 6: exist)
    char q_name[MAXSTR];        // Nombre de la cola del cliente (para enviar la respuesta)
    int key;                    
    char value1[MAXSTR];        
    int N_value2;               
    double V_value2[MAXVEC];    
    struct Coord value3;        
    int res;                    // Resultado de la operación
};

#endif