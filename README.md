# Tarea 1 Comunicación de Datos y Redes

# INTEGRANTES
## Brandon Reyes y Esteban Rivas


# INSTRCCUIONES DE EJECUCION

### Primero se ejcuta el servidor, para ello se abre una terminal y se entra a la carpeta Servidor utilizando el comando: cd Servidor, debemos estar si o si posicionados en la carpeta Servidor. Luego ingresamos el comando: make. Luego ingresamos el comando: ./servidor <Numeropuerto>

### Luego en una terminal distinta, ingresamos a la carpeta Cliente con el comando: cd Cliente, debemos asegurarnos de estar dentro de la carpeta si o si. Luego ingresamos el comando: make

### Si queremos ingresar con localhost (este si funciona) escribimos el comando: telnet localhost <pNumeropuerto>

### Si queremos ingresar la IP (es el que hay que arreglar) escibimos el comando: ./cliente <IP> <numeropuerto> (La IP se muestra cuando ejecutamos el servidor)