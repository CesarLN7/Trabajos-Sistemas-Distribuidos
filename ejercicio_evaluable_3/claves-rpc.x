struct Coord {
    double x;
    double y;
};

struct args_in {
    int res;
    string value1<256>;
    int N_value2;
    double V_value2<32>;
    struct Coord value3;
};

struct args_out {
    int res;
    string value1<256>;
    int N_value2;
    double V_value2<32>;
    struct Coord value3;
    int status;
};

program INTERFAZ {
    version FUNCIONESVER {
        int rpc_destroy() = 1;
        int rpc_set_value(int key, struct args_in) = 2;
        struct args_out rpc_get_value(int key) = 3;
        int rpc_modify_value(int key, struct args_in) = 4;
        int rpc_delete_key(int key) = 5;
        int rpc_exist(int key) = 6;
    } = 1;
} = 99;
