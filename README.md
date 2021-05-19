# UNIX SHELL PROJECT

##TAREA 1

El código de partida que se suministra (Shell_project.c listado al final de este enunciado) entra en
un bucle donde se leen y analizan líneas de comando desde el teclado. En esta primera etapa
simplificaremos el problema y nos centraremos en la tarea esencial de crear un proceso y ejecutar
un nuevo programa. Se debe añadir el código necesario al programa inicial para:

1. Ejecutar los comandos en un proceso independiente
2. esperar o no a la finalización del comando dependiendo de si el comando era de primer o de
segundo plano (foreground/background)
3. dar un mensaje que informe de la terminación o no del comando y de sus datos
identificativos
4. continuar con el bucle para tratar el siguiente comando


Para compilar el programa usar:
<code> gcc Shell_project.c job_control.c –o Shell </code>

Para ejecutarlo:
<code>./Shell </code>


Para salir pulsar CONTROL + D (^D).

