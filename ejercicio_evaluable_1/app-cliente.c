#include <stdio.h>
#include "claves/claves.h"

int main (int argc, char **argv)
{
    int key = 5;
    char *v1 = "ejemplo de valor 1";
    double v2[] = {2.3, 0.5, 23.45};
    struct Coord v3;
    v3.x = 10;
    v3.y = 5;

    int err = destroy();
    if (err == -1) {
        printf("Error al destruir las tuplas anteriores\n");
    }

    return 0;
}