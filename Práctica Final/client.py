from enum import Enum

import argparse
import socket
import os
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

    _listener_socket = None             # socket de escucha para transferencia de archivos
    _current_user = None                # usuario actualmente conectado
    _listener_thread = None             # hilo para escuchar conexiones entrantes
    _stop_listener = threading.Event()  # evento para detener el hilo de escucha

    # ******************** METHODS *******************

    @staticmethod
    
    def _send_string(sock, string):
        sock.sendall(string.encode() + b'\0')

    @staticmethod
    
    def _recv_string(sock):
        data = b''
        while True:
            byte = sock.recv(1)
            if not byte or byte == b'\0':
                break
            data += byte
        return data.decode()

    @staticmethod
    
    def _connect_to_server():
        try:
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.connect((client._server, client._port))
            return sock
        except:
            return None
    
    @staticmethod
    
    def _handle_incoming_file_request(conn):
        try:
            op = client._recv_string(conn)
            if op != "GET FILE":
                conn.sendall(bytes([2]))  # operación no válida
                conn.close()
                return

            path = client._recv_string(conn)

            if not os.path.isfile(path):
                conn.sendall(bytes([1]))  # archivo no existe
                conn.close()
                return

            # Enviar código OK
            conn.sendall(bytes([0]))

            # Enviar tamaño del archivo
            size = os.path.getsize(path)
            client._send_string(conn, str(size))

            # Enviar contenido
            with open(path, "rb") as f:
                while True:
                    chunk = f.read(4096)
                    if not chunk:
                        break
                    conn.sendall(chunk)

        except Exception as e:
            print(f"c> ERROR handling GET FILE: {e}")
        finally:
            conn.close()

    @staticmethod

    def  register(user) :

        sock = client._connect_to_server()
        if not sock:
            print("c> REGISTER FAIL")
            return client.RC.ERROR

        try:
            client._send_string(sock, "REGISTER")
            client._send_string(sock, user)
            code = sock.recv(1)[0]

            if code == 0:
                print("c> REGISTER OK")
                return client.RC.OK
            elif code == 1:
                print("c> USERNAME IN USE")
                return client.RC.USER_ERROR
            else:
                print("c> REGISTER FAIL")
                return client.RC.ERROR
        finally:
            sock.close()

    @staticmethod

    def  unregister(user) :

        sock = client._connect_to_server()
        if not sock:
            print("c> UNREGISTER FAIL")
            return client.RC.ERROR

        try:
            client._send_string(sock, "UNREGISTER")
            client._send_string(sock, user)
            code = sock.recv(1)[0]

            if code == 0:
                print("c> UNREGISTER OK")
                return client.RC.OK
            elif code == 1:
                print("c> USER DOES NOT EXIST")
                return client.RC.USER_ERROR
            else:
                print("c> UNREGISTER FAIL")
                return client.RC.ERROR
        finally:
            sock.close()

    @staticmethod

    def connect(user):

        # Creamos un socket temporal para obtener un puerto libre
        temp_listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        temp_listener.bind(('', 0))  # Puerto dinámico
        port = temp_listener.getsockname()[1]
        temp_listener.close()

        # Conectamos con el servidor
        sock = client._connect_to_server()
        if not sock:
            print("c> CONNECT FAIL")
            return client.RC.ERROR

        try:
            client._send_string(sock, "CONNECT")
            client._send_string(sock, user)
            client._send_string(sock, str(port))
            resp = sock.recv(1)
            if len(resp) < 1:
                print("c> CONNECT FAIL (no response)")
                return client.RC.ERROR
            code = resp[0]

            if code == 0:
                print("c> CONNECT OK")
                
                # Solo si ha ido bien, entonces lanzamos el listener
                listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                listener.bind(('', port))
                listener.listen(5)

                client._listener_socket = listener
                client._current_user = user
                client._stop_listener.clear()

                def listen_loop():
                    while not client._stop_listener.is_set():
                        listener.settimeout(1.0)
                        try:
                            conn, addr = listener.accept()
                            threading.Thread(target=client._handle_incoming_file_request, args=(conn,), daemon=True).start()
                        except socket.timeout:
                            continue

                thread = threading.Thread(target=listen_loop, daemon=True)
                thread.start()
                client._listener_thread = thread

                return client.RC.OK

            elif code == 1:
                print("c> CONNECT FAIL, USER DOES NOT EXIST")
            elif code == 2:
                print("c> USER ALREADY CONNECTED")
            else:
                print("c> CONNECT FAIL")
            return client.RC.ERROR
        finally:
            sock.close()

    @staticmethod

    def disconnect(user):
        sock = client._connect_to_server()
        if not sock:
            print("c> DISCONNECT FAIL")
            return client.RC.ERROR

        try:
            client._send_string(sock, "DISCONNECT")
            client._send_string(sock, user)
            resp = sock.recv(1)
            if len(resp) < 1:
                print("c> DISCONNECT FAIL (no response)")
                return client.RC.ERROR
            code = resp[0]

            if code == 0:
                print("c> DISCONNECT OK")

                # Detenemos hilo primero
                client._stop_listener.set()
                if client._listener_thread:
                    client._listener_thread.join()
                    client._listener_thread = None

                # Ahora cerramos el socket
                if client._listener_socket:
                    client._listener_socket.close()
                    client._listener_socket = None

                client._current_user = None
                return client.RC.OK
            elif code == 1:
                print("c> DISCONNECT FAIL, USER DOES NOT EXIST")
            elif code == 2:
                print("c> DISCONNECT FAIL, USER NOT CONNECTED")
            else:
                print("c> DISCONNECT FAIL")

            return client.RC.ERROR
        finally:
            sock.close()


    @staticmethod

    def  publish(fileName,  description) :

        sock = client._connect_to_server()
        if not sock:
            print("c> PUBLISH FAIL")
            return client.RC.ERROR

        user = client._current_user
        if not user:
            print("c> PUBLISH FAIL, USER NOT CONNECTED")
            return client.RC.USER_ERROR

        if not os.path.exists(fileName):
            print("c> PUBLISH FAIL, FILE DOES NOT EXIST")
            return client.RC.ERROR

        try:
            client._send_string(sock, "PUBLISH")
            client._send_string(sock, user)
            client._send_string(sock, fileName)
            client._send_string(sock, description)
            code = sock.recv(1)[0]

            if code == 0:
                print("c> PUBLISH OK")
                return client.RC.OK
            elif code == 1:
                print("c> PUBLISH FAIL, USER DOES NOT EXIST")
            elif code == 2:
                print("c> PUBLISH FAIL, USER NOT CONNECTED")
            elif code == 3:
                print("c> PUBLISH FAIL, CONTENT ALREADY PUBLISHED")
            else:
                print("c> PUBLISH FAIL")
            return client.RC.ERROR
        finally:
            sock.close()

    @staticmethod

    def  delete(fileName) :

        sock = client._connect_to_server()
        if not sock:
            print("c> DELETE FAIL")
            return client.RC.ERROR

        user = client._current_user
        if not user:
            print("c> DELETE FAIL, USER NOT CONNECTED")
            return client.RC.USER_ERROR

        try:
            client._send_string(sock, "DELETE")
            client._send_string(sock, user)
            client._send_string(sock, fileName)
            code = sock.recv(1)[0]

            if code == 0:
                print("c> DELETE OK")
                return client.RC.OK
            elif code == 1:
                print("c> DELETE FAIL, USER DOES NOT EXIST")
            elif code == 2:
                print("c> DELETE FAIL, USER NOT CONNECTED")
            elif code == 3:
                print("c> DELETE FAIL, CONTENT NOT PUBLISHED")
            else:
                print("c> DELETE FAIL")
            return client.RC.ERROR
        finally:
            sock.close()

    @staticmethod
    
    def listusers():
        
        sock = client._connect_to_server()
        if not sock:
            print("c> LIST_USERS FAIL")
            return client.RC.ERROR
        
        user = client._current_user
        if not user:
            print("c> LIST_USERS FAIL, USER NOT CONNECTED")
            return client.RC.USER_ERROR

        try:
            client._send_string(sock, "LIST_USERS")
            client._send_string(sock, user)
            code = sock.recv(1)[0]

            if code == 0:
                count = int(client._recv_string(sock))
                print("c> LIST_USERS OK")
                for _ in range(count):
                    name = client._recv_string(sock)
                    ip = client._recv_string(sock)
                    port = client._recv_string(sock)
                    print(f"{name} {ip} {port}")
                return client.RC.OK
            elif code == 1:
                print("c> LIST_USERS FAIL, USER DOES NOT EXIST")
            elif code == 2:
                print("c> LIST_USERS FAIL, USER NOT CONNECTED")
            else:
                print("c> LIST_USERS FAIL")
            return client.RC.ERROR

        finally:
            sock.close()
            
    @staticmethod
    
    def listcontent(target_user):

        sock = client._connect_to_server()
        if not sock:
            print("c> LIST_CONTENT FAIL")
            return client.RC.ERROR
        
        user = client._current_user
        if not user:
            print("c> LIST_CONTENT FAIL, USER NOT CONNECTED")
            return client.RC.USER_ERROR
        
        try:
            client._send_string(sock, "LIST_CONTENT")
            client._send_string(sock, user)
            client._send_string(sock, target_user)
            code = sock.recv(1)[0]

            if code == 0:
                count = int(client._recv_string(sock))
                print(f"c> LIST_CONTENT OK ({count} items)")
                for _ in range(count):
                    entry = client._recv_string(sock)
                    print(entry)
                return client.RC.OK
            elif code == 1:
                print("c> LIST_CONTENT FAIL, USER DOES NOT EXIST")
            elif code == 2:
                print("c> LIST_CONTENT FAIL, USER HAS NO CONTENT")
            else:
                print("c> LIST_CONTENT FAIL")
            return client.RC.ERROR
        finally:
            sock.close()


    @staticmethod
    
    def getfile(user, remote_FileName, local_FileName):
        # Paso 1: obtener IP y puerto del usuario mediante LIST_USERS
        target_ip = None
        target_port = None

        sock = client._connect_to_server()
        if not sock:
            print("c> GET_FILE FAIL")
            return client.RC.ERROR

        try:
            client._send_string(sock, "LIST_USERS")
            client._send_string(sock, client._current_user)
            code = sock.recv(1)[0]

            if code != 0:
                print("c> GET_FILE FAIL")
                return client.RC.ERROR

            count = int(client._recv_string(sock))
            for _ in range(count):
                name = client._recv_string(sock)
                ip = client._recv_string(sock)
                port = client._recv_string(sock)

                if name == user:
                    target_ip = ip
                    target_port = int(port)
                    break
        finally:
            sock.close()

        if not target_ip or not target_port:
            print("c> GET_FILE FAIL, USER NOT FOUND OR NOT CONNECTED")
            return client.RC.ERROR

        # Paso 2: conectarse al cliente remoto
        try:
            conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            conn.connect((target_ip, target_port))

            client._send_string(conn, "GET FILE")
            client._send_string(conn, os.path.abspath(remote_FileName))

            code = conn.recv(1)[0]

            if code == 0:
                # Recibir tamaño
                size_str = client._recv_string(conn)
                try:
                    size = int(size_str)
                except:
                    print("c> GET_FILE FAIL, INVALID SIZE")
                    conn.close()
                    return client.RC.ERROR

                with open(local_FileName, "wb") as f:
                    remaining = size
                    while remaining > 0:
                        data = conn.recv(min(4096, remaining))
                        if not data:
                            break
                        f.write(data)
                        remaining -= len(data)

                if remaining == 0:
                    print("c> GET_FILE OK")
                    conn.close()
                    return client.RC.OK
                else:
                    f.close()
                    os.remove(local_FileName)
                    print("c> GET_FILE FAIL, INCOMPLETE TRANSFER")
                    conn.close()
                    return client.RC.ERROR

            elif code == 1:
                print("c> GET_FILE FAIL, FILE NOT EXIST")
            else:
                print("c> GET_FILE FAIL")

            conn.close()
            return client.RC.ERROR

        except Exception as e:
            if os.path.exists(local_FileName):
                os.remove(local_FileName)
            print(f"c> GET_FILE FAIL ({e})")
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