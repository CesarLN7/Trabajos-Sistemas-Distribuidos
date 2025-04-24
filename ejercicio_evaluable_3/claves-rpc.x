struct Coord_RPC {
    int x;
    int y;
};

struct args_struct {
    int key;
    string value1<256>;
    int N_value2;
    double V_value2<32>;
    struct Coord_RPC value3;
    int res;
};

program INTERFAZ {
    version FUNCIONESVER {
        int rpc_destroy() = 1;
        int rpc_set_value(args_struct aux) = 2;
        args_struct rpc_get_value(int key) = 3;
        int rpc_modify_value(args_struct aux) = 4;
        int rpc_delete_key(int key) = 5;
        int rpc_exist(int key) = 6;
    } = 1;
} = 99;
