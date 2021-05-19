# UNIX SHELL PROJECT

## TAREA 1: 

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

En esta tarea será necesario usar las llamadas al sistema fork, execvp, waitpid y exit. Los mensajes
con información sobre los comandos ejecutados deben presentar la siguiente información para
procesos en primer plano: pid, nombre, tipo de terminación y código de terminación o señal
responsable. Por ejemplo:

<code>
Foreground pid: 5615, command: ls, Exited, info: 0
Foreground pid: 5616, command: vi, Suspended, info: 20 
</code>

Para los comandos de segundo plano habrá que informar de su pid, nombre e indicar que se
continúan ejecutando en segundo plano mientras el Shell continua simultáneamente con la
ejecución de nuevos comandos. Por ejemplo:

<code>Background job running... pid: 5622, command: sleep </code>

Para comandos no encontrados, que no se pueden ejecutar, hay que dar el mensaje de error
correspondiente:
<code> Error, command not found: lss</code>


Para compilar el programa usar:

<code> gcc Shell_project.c job_control.c –o Shell </code>

Para ejecutarlo:
<code>./Shell </code>


Para salir pulsar CONTROL + D (^D).

