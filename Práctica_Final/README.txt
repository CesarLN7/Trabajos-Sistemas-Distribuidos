Para la compilación y generación de ejecutables se ha utilizado un Makefile.
Será necesario ejecutar en terminal en el directorio Práctica_Final dos comandos en dos terminales.

En la primera:
    ./run-server.sh
Y en la segunda: 
    ./run-client.sh

El primer script corresponde a la compilación del programa y el ejecutable del servidor.

El segundo script ejecuta el archivo de cliente en Python.
Si se quieren ejecutar varios clientes, se debe abrir una terminal diferente para cada cliente ya que si dos o más usuarios 
realizan peticiones desde la misma terminal puede dar lugar a errores no previstos. Cada usuario debe ejecutar el archivo client.py en una terminal diferente.