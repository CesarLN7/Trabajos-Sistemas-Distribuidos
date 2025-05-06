from enum import Enum

import argparse

import socket
import threading


class client :



    # ******************** TYPES *********************

    # *

    # * @brief Return codes for the protocol methods

    class RC(Enum) :

        OK = 0

        ERROR = 1

        USER_ERROR = 2



    # ****************** ATTRIBUTES ******************

    _server = None
    _port = -1

    _sock = None
    _username = None
    _published = set()
    _p2p_thread = None
    _stop_event = threading.Event()

    # ******************** METHODS *******************

    @staticmethod
    def _read_string(sock):
        """ Lee de un socket dado hasta un final de cadena """
        message = ""
        while True:
            msg = sock.recv(1)
            if(msg == b'\0'):
                break
            message += msg.decode()
        message = message

        return message
    @staticmethod
    def write_file_to_socket(sock, file_path):
        """ Escribe un fichero entero en un socket dado """
        with open(file_path, 'rb') as file:
            while True:
                data = file.read(1024)
                if not data:
                    # EOF, no more data to read
                    break
                sock.sendall(data)

    @staticmethod
    def _read_until_eof(sock):
        """Lee hasta el final de archivo de un socket dado de un fichero dado"""
        data = ""
        while True:
            chunk = sock.recv(1024)
            if not chunk:
                break
            data += chunk.decode()
        return data

    @staticmethod
    def _get_socket(ip,port):
        """ Obtiene un socket dado una ip y un puerto """
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_address = (ip,port)
        sock.connect(server_address)
        return sock

    @staticmethod
    def register(user: str):
        """Función que se encarga de registrar un usuario"""
        serv_sock = client._get_socket(client._server, client._port)
        try:
            serv_sock.sendall(b"REGISTER\0")
            serv_sock.sendall((user + '\0').encode())

            answer = int.from_bytes(serv_sock.recv(1), 'big')

        except Exception as e:
            answer = None
            print(e)

        finally:
            serv_sock.close()

        match answer:
            case 0:
                print("REGISTER OK")
            case 1:
                print("USERNAME IN USE")
            case _:
                print("REGISTER FAIL")

        return answer


    @staticmethod
    def unregister(user: str):
        """Función que se encarga de dar de baja a un usuario"""
        serv_sock = client._get_socket(client._server, client._port)
        try:
            serv_sock.sendall(b"UNREGISTER\0")
            serv_sock.sendall((user + '\0').encode())

            answer = int.from_bytes(serv_sock.recv(1), 'big')

        except Exception as e:
            answer = None
            print(e)

        finally:
            serv_sock.close()

        match answer:
            case 0:
                print("UNREGISTER OK")
            case 1:
                print("USER DOES NOT EXIST")
            case _:
                print("UNREGISTER FAIL")

        return answer

    @staticmethod
    def _client_listen(sock):
        """ Función que realiza el hilo secundario del cliente para atender a otros clientes"""

        sock.listen(5)
        while not client._stop_event.is_set():

            try:
                connection, client_address = sock.accept()
                try:
                    message = client._read_string(connection)
                    #print("conectado")

                    if message == "GET_FILE":
                        
                        message = client._read_string(connection)

                        namefile = message
                        try:
                            answer = 0
                            connection.sendall(answer.to_bytes(1,'big'))
                            # Leo el fichero que me han pedido y lo envío
                            with open(namefile, mode="rb") as file:
                                while True:
                                    chunk = file.read(1024)
                                    if not chunk:
                                        break
                                    connection.sendall(chunk)
                        except FileNotFoundError:
                            answer = 1
                            connection.sendall(answer.to_bytes(1,'big'))
                        except Exception as e:
                            print(e)
                    else:
                        #print("not operation")
                        answer = 2
                        connection.sendall(answer.to_bytes(1,'big'))

                finally:
                    connection.close()
            except OSError as e:
                if e.errno == 22:
                    return 
        

    @staticmethod
    def  connect(user) :
        """Función que se encarga de conectar a los usuarios al servicio"""

        if (client._username):
            print("CONNECT FAIL")  # Si ya hay un cliente conectado
            return 
        
        serv_sock = client._get_socket(client._server,client._port)
        time = client._read_time()

        try:
            message = b'CONNECT\0'
            serv_sock.sendall(message)

            message = (time + '\0').encode()
            serv_sock.sendall(message)

            message = f"{user}\0".encode()
            serv_sock.sendall(message)

            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            
            client._sock = sock # Guardo el socket para poder cerrarlo en el disconnect

            ip_address = socket.gethostbyname(socket.gethostname())
            #server_address = (ip_address,0) # el puerto 0 toma un puerto libre
            sock.bind(("", 0)) # puerto 0 y escucha cualquier interfaz de red
            address, port = sock.getsockname() # Obtengo el puerto asignado


            message = f"{port}\0".encode()
            serv_sock.sendall(message)

            message = ""

            answer = int.from_bytes(serv_sock.recv(1), 'big')
            #print("port:", port)

        except Exception as e:
            print(e)
            answer = None

        finally:
            serv_sock.close()

        
        match answer:
            case 0:
                print("CONNECT OK")
                client._username = user
                client._p2p_thread = threading.Thread(target=client._client_listen,args=(sock,))
                client._p2p_thread.start()
            case 1:
                print("CONNECT FAIL, USER DOES NOT EXIST")
                sock.close()
            case 2:
                print("USER ALREADY CONNECTED")
                sock.close()
            case _:
                print("CONNECT FAIL")
                sock.close()

        

        return answer
    
    @staticmethod
    def  disconnect(user) :
        """Función que se encarga de desconectar a los clientes del servicio"""

        serv_sock = client._get_socket(client._server,client._port)
        time = client._read_time()

        try:
            message = b'DISCONNECT\0'
            serv_sock.sendall(message)

            message = (time + '\0').encode()
            serv_sock.sendall(message)

            message = f"{user}\0".encode()
            serv_sock.sendall(message)

            message = ""

            answer = int.from_bytes(serv_sock.recv(1), 'big')

        except Exception as e:
            answer = None

        finally:
            serv_sock.close()

        match answer:
                case 0:
                    print("DISCONNECT OK")
                    # Termino la ejecución del hilo que atiende peticiones
                    client._stop_event.set()
                    client._sock.shutdown(socket.SHUT_RDWR)
                    # Cierro el socket
                    client._sock.close()
                    client._p2p_thread.join()

                    client._username = None
                case 1:
                    print("DISCONNECT FAIL / USER DOES NOT EXIST")
                case 2:
                    print("DISCONNECT FAIL / USER NOT CONNECTED")
                case _:
                    print("DISCONNECT FAIL")

        return answer

    @staticmethod
    def  publish(fileName,  description) :
        """Función que se encarga de publicar un archivo dado su nombre y su descripción"""
        
        serv_sock = client._get_socket(client._server,client._port)
        time = client._read_time()

        try:
            message = b'PUBLISH\0'
            serv_sock.sendall(message)

            message = (time + '\0').encode()
            serv_sock.sendall(message)

            message = f"{client._username}\0".encode()
            serv_sock.sendall(message)

            message = f"{fileName}\0".encode()
            serv_sock.sendall(message)

            message = f"{description}\0".encode()
            serv_sock.sendall(message)

            message = ""

            answer = int.from_bytes(serv_sock.recv(1), 'big')
        except Exception as e:
            answer = None

        finally:
            serv_sock.close()

        match answer:
            case 0:
                print("PUBLISH OK")
            case 1:
                print("PUBLISH FAIL, USER DOES NOT EXIST")
            case 2:
                print("PUBLISH FAIL, USER NOT CONNECTED")
            case 3:
                print("PUBLISH FAIL, CONTENT ALREADY PUBLISHED")
            case _:
                print("PUBLISH FAIL")
    
        
        return answer

    @staticmethod
    def  delete(fileName) :
        """Función que se encarga de borrar un archivo indicado"""

        serv_sock = client._get_socket(client._server,client._port)
        time = client._read_time()

        try:
            message = b'DELETE\0'
            serv_sock.sendall(message)

            message = (time + '\0').encode()
            serv_sock.sendall(message)

            message = f"{client._username}\0".encode()
            serv_sock.sendall(message)

            message = f"{fileName}\0".encode()
            serv_sock.sendall(message)

            message = ""

            answer = int.from_bytes(serv_sock.recv(1), 'big')

        except Exception as e:
            answer = None

        finally:
            serv_sock.close()

        match answer:
            case 0:
                print("DELETE OK")
            case 1:
                print("DELETE FAIL, USER DOES NOT EXIST")
            case 2:
                print("DELETE FAIL, USER NOT CONNECTED")
            case 3:
                print("DELETE FAIL, CONTENT NOT PUBLISHED")
            case _:
                print("DELETE FAIL")
        
        return answer

    @staticmethod
    def get_list_users()->tuple:
        """ 
         @return Devuelve un tupla, tupla[0] es el código de error y tupla[1] es una lista de diccionarios 
        """
        serv_sock = client._get_socket(client._server,client._port)
        time = client._read_time()

        try:
            message = b'LIST_USERS\0'
            serv_sock.sendall(message)

            message = (time + '\0').encode()
            serv_sock.sendall(message)

            message = f"{client._username}\0".encode()
            serv_sock.sendall(message)

            message = ""

            answer = int.from_bytes(serv_sock.recv(1), 'big')
            ret_list = []
            match answer:
                case 0:

                    message = client._read_string(serv_sock)
                    num = int(message)

                    for i in range(num):
                        ret_dict = {}
                        # Imprimo el listado de usuarios
                        ret_dict["username"] = client._read_string(serv_sock)
                        ret_dict["ip"] = client._read_string(serv_sock)
                        ret_dict["port"] = client._read_string(serv_sock)
                        ret_list.append(ret_dict)
                    return 0,ret_list

                case 1:
                    return 1,ret_list
                case 2:
                    return 2,ret_list
                case _:
                    return None,ret_list
        except Exception as e:
            print("Excepción en get_list_users",e)
            return None,[]
        
        finally:
            serv_sock.close()



    @staticmethod
    def  listusers():
        """Función que lista los usuarios actualmente conectados"""

        answer, list_users = client.get_list_users()

        match answer:
            case 0:
                print("LIST_USERS OK")

                for dict_user in list_users:
                    # Imprimo el listado de usuarios
                    print(f"{dict_user['username']} {dict_user['ip']} {dict_user['port']}")

            case 1:
                print("LIST_USERS FAIL, USER DOES NOT EXIST")
            case 2:
                print("LIST_USERS FAIL, USER NOT CONNECTED")
            case _:
                print("LIST_USERS FAIL")

        return answer

    @staticmethod
    def  listcontent(user) :
        """Funcion que lista el contenido de un usuario dado"""
        serv_sock = client._get_socket(client._server,client._port)
        time = client._read_time()

        try:
            message = b'LIST_CONTENT\0'
            serv_sock.sendall(message)

            message = (time + '\0').encode()
            serv_sock.sendall(message)

            message = f"{client._username}\0".encode()
            serv_sock.sendall(message)

            message = f"{user}\0".encode()
            serv_sock.sendall(message)

            answer = int.from_bytes(serv_sock.recv(1), 'big')

            match answer:
                case 0:
                    print("LIST_CONTENT OK")

                    message = client._read_string(serv_sock)
                    num = int(message)

                    for i in range(num):
                        filename = client._read_string(serv_sock)
                        file_descr = client._read_string(serv_sock)

                        print(f"{filename} {file_descr}")

                case 1:
                    print("LIST_CONTENT FAIL, USER DOES NOT EXIST")
                case 2:
                    print("LIST_CONTENT FAIL, USER NOT CONNECTED")
                case 3:
                    print("LIST_CONTENT FAIL, REMOTE USER DOES NOT EXIST")
                case _:
                    print("LIST_CONTENT FAIL")
        
        except Exception as e:
            print("LIST_CONTENT FAIL")
            answer = None
        
        finally:
            serv_sock.close()

        return answer

    @staticmethod
    def  getfile(user,  remote_FileName,  local_FileName) :
        """Función que obtiene un archivo publicado por un usuario"""
        answer, list_users = client.get_list_users()
        time = client._read_time()

        if answer != 0:
            print("No se ha podido acceder a la lista de usuarios")
            return answer
        for dict_user in list_users:
            if dict_user["username"] == user:
                client_sock = client._get_socket(dict_user["ip"],int(dict_user["port"]))
                #print(">>", dict_user["ip"], dict_user["port"])
                break
        
        try:
            message = b'GET_FILE\0'
            client_sock.sendall(message)

            message = f"{remote_FileName}\0".encode()
            client_sock.sendall(message)

            answer = int.from_bytes(client_sock.recv(1), 'big')
            print(answer)
            match answer:
                case 0:
                    print("GET_FILE OK")

                    str_fichero_remoto = client._read_until_eof(client_sock)
                    with open(local_FileName, mode="w", encoding="utf-8") as fichero_local:
                        fichero_local.write(str_fichero_remoto)

                case 1:
                    print("GET_FILE FAIL, FILE NOT EXIST")
                case _:
                    print("GET_FILE FAIL")
        finally:
            client_sock.close()
        
        return answer





    

    @staticmethod

    def  connect(user) :

        #  Write your code here

        return client.RC.ERROR





    

    @staticmethod

    def  disconnect(user) :

        #  Write your code here

        return client.RC.ERROR



    @staticmethod

    def  publish(fileName,  description) :

        #  Write your code here

        return client.RC.ERROR



    @staticmethod

    def  delete(fileName) :

        #  Write your code here

        return client.RC.ERROR



    @staticmethod

    def  listusers() :

        #  Write your code here

        return client.RC.ERROR



    @staticmethod

    def  listcontent(user) :

        #  Write your code here

        return client.RC.ERROR



    @staticmethod

    def  getfile(user,  remote_FileName,  local_FileName) :

        #  Write your code here

        return client.RC.ERROR



    # *

    # **

    # * @brief Command interpreter for the client. It calls the protocol functions.

    @staticmethod

    def shell():



        while (True) :

            try :

                command = input("c> ")

                line = command.split(" ")

                if (len(line) > 0):



                    line[0] = line[0].upper()



                    if (line[0]=="REGISTER") :

                        if (len(line) == 2) :

                            client.register(line[1])

                        else :

                            print("Syntax error. Usage: REGISTER <userName>")



                    elif(line[0]=="UNREGISTER") :

                        if (len(line) == 2) :

                            client.unregister(line[1])

                        else :

                            print("Syntax error. Usage: UNREGISTER <userName>")



                    elif(line[0]=="CONNECT") :

                        if (len(line) == 2) :

                            client.connect(line[1])

                        else :

                            print("Syntax error. Usage: CONNECT <userName>")

                    

                    elif(line[0]=="PUBLISH") :

                        if (len(line) >= 3) :

                            #  Remove first two words

                            description = ' '.join(line[2:])

                            client.publish(line[1], description)

                        else :

                            print("Syntax error. Usage: PUBLISH <fileName> <description>")



                    elif(line[0]=="DELETE") :

                        if (len(line) == 2) :

                            client.delete(line[1])

                        else :

                            print("Syntax error. Usage: DELETE <fileName>")



                    elif(line[0]=="LIST_USERS") :

                        if (len(line) == 1) :

                            client.listusers()

                        else :

                            print("Syntax error. Use: LIST_USERS")



                    elif(line[0]=="LIST_CONTENT") :

                        if (len(line) == 2) :

                            client.listcontent(line[1])

                        else :

                            print("Syntax error. Usage: LIST_CONTENT <userName>")



                    elif(line[0]=="DISCONNECT") :

                        if (len(line) == 2) :

                            client.disconnect(line[1])

                        else :

                            print("Syntax error. Usage: DISCONNECT <userName>")



                    elif(line[0]=="GET_FILE") :

                        if (len(line) == 4) :

                            client.getfile(line[1], line[2], line[3])

                        else :

                            print("Syntax error. Usage: GET_FILE <userName> <remote_fileName> <local_fileName>")



                    elif(line[0]=="QUIT") :

                        if (len(line) == 1) :

                            break

                        else :

                            print("Syntax error. Use: QUIT")

                    else :

                        print("Error: command " + line[0] + " not valid.")

            except Exception as e:

                print("Exception: " + str(e))



    # *

    # * @brief Prints program usage

    @staticmethod

    def usage() :

        print("Usage: python3 client.py -s <server> -p <port>")





    # *

    # * @brief Parses program execution arguments

    @staticmethod

    def  parseArguments(argv) :

        parser = argparse.ArgumentParser()

        parser.add_argument('-s', type=str, required=True, help='Server IP')

        parser.add_argument('-p', type=int, required=True, help='Server Port')

        args = parser.parse_args()



        if (args.s is None):

            parser.error("Usage: python3 client.py -s <server> -p <port>")

            return False



        if ((args.p < 1024) or (args.p > 65535)):

            parser.error("Error: Port must be in the range 1024 <= port <= 65535");

            return False;

        

        client._server = args.s

        client._port = args.p



        return True





    # ******************** MAIN *********************

    @staticmethod

    def main(argv) :

        if (not client.parseArguments(argv)) :

            client.usage()

            return



        #  Write code here

        client.shell()

        print("+++ FINISHED +++")

    



if __name__=="__main__":

    client.main([])