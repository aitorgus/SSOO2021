/**
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

// -----------------------------------------------------------------------
//  NOMBRE Y APELLIDOS: AITOR GUSTAVO CAMACHO GUTIERREZ
//  DNI:53741197-H        GRUPO : COMPUTADORES C (TARDE)
// -----------------------------------------------------------------------

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

To compile and run the program:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell          
	(then type ^D to exit program)

**/

#include "job_control.h"   // remember to compile with module job_control.c 
#include <string.h>

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------

int main(void)
{
	char inputBuffer[MAX_LINE]; /* Búffer que alberga el comando introducido*/
	int background;             /* Su valor es 1 si el comando introducido finaliza con '&' */
	char *args[MAX_LINE/2];     /* La línea de comando tiene una longitud de 128 argumentos como máximo */
	// VARIABLES DE UTILIZAD :

	int pid_fork, pid_wait; /* pid for created and waited process */
	int status;             /* status returned by wait */
	enum status status_res; /* status processed by analyze_status() */
	int info;				/* info processed by analyze_status() */

	while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{   		
		printf("\n Inserte comando->");
		fflush(stdout);
		get_command(inputBuffer, MAX_LINE, args, &background);  
		/* get_command va a recoger y leer  lo que el usuario introduzca en la línea de comandos, lo va a separar y lo añadirá a "args" desmembrado cada comando.
		(1)args es un array de Strings donde va a introducir cada "pieza" de los comandos introducidos
		 -(1.1) -> Por ejemplo, si introducimos "ls -l", tendremos args[0]="ls", args[1]="-l" y args[2]=NULL.
		(2) -> "background" (porque la va a modificar) es una variable pasada por referencia, se pondrá a 1 (true) si el comando termina con "&"
		clave para diferenciar un programa en primer plano o segundo plano
		*/ 
		
		if(args[0]==NULL) continue;   // if empty command

// -----------------------------------------------------------------------
//                IMPLEMENTACIÓN DE COMANDOS CD          
// -----------------------------------------------------------------------

		/* (1) --> Para implementar el comando cd, usaremos "strcmp" para comparar strings. En este caso,
		comprobamos un string que ha sido alojado en arg[0], es decir un comando y lo comparamos con la palabra clave
		"cd" que es el comando que queremos implementar
		  (2) -> Con esta fórmula podemos saber si el usuario ha usado el comando cd.
		  (3) -> Como cd se usa sin opciones ni atributos, el siguiente fragmento que coge getcommand 
		  es la ruta del directorio, ubicada en args[1].
		  (4) strcmp hace una comparación . Devuelve 0 si son iguales, los booleanos se interpretan como false,
		  lo que en este caso podría crear un cortocircuito en el if, por ello negamos if(!strcmp(args[0],"cd"))

		*/

		if(!strcmp(args[0], "cd")){ 
			chdir(args[1]); 
			continue;

		}
		if (!strcmp (args[0], "salir")){
			exit(0); //Función para salir de la shell . 
		}
// -----------------------------------------------------------------------
//                 GENERAR UN PROCESO HIJO CON FORK         
// -----------------------------------------------------------------------

	/* (1) Generar un proceso hijo con fork ()
			(1.1) fork crea una copia exacta del programa (un proceso hijo)
			(1.2) Si pid_fork es igual a 0, es la zona de ejecución del hijo, si es distinto, es del padre
			(1.3) En la zona del hijo ejecutaremos los programas. Le pasamos como argumentos la primera posición del array de comandos y argumentos
		
		(2) Padre
			(2.1) waitpid() es una función que indica a que comando quieres esperar , en nuestro caso, provoca que el padre quede a la espera
			de que el hijo cambie de estado. Cuando esto sucede, waitpid() lo recoge
			(2.2) Le pasamos como parámetros 1º pid_fork, ahí se almacena el pid de nuestro hijo , 2º &status y 3º opcion 0 por defecto
				*OJO* -> en la variable status, se guarda un valor que indica la causa de terminación de la tarea (Si ha finalizado, si ha finalizado con errores)
			(2.3) Para analizar esto, usamos la función "analyze_status" que se encuentra en "job_control", el resultado de ello lo guardamos en la variable
			ya creada en la plantilla la cual es "status_res"
			(2.4) En el printf con args[0] mostramos la primera posición del array donde se ha guardado el comando, el pid del hijo con la variable pid_fork, y con info,
			información sobre el estado de la misma. En info se guarda el estado, si es 255 significa que ha habido un error o el comando no existe../M
			

		*/
	pid_fork=fork();

		if(pid_fork >0){
			//Zona del padre
				/*Si viene de PRIMER plano */
			if (background ==0){
				waitpid(pid_fork,&status, 0);
				status_res = analyze_status(status,&info);

				if(info !=255 ){ //Si es distinto a 255 es que no ha habido un
					printf ("\n Comando  ' %s '  ejecutado en PRIMER plano con pid %d . Estado FINALIZADO. Info %d\n",args[0],pid_fork,info);
					
				}
				
			}
			else {
					printf("\n comando ' %s '  ejecutado en SEGUNDO plano con pid %d ",args[0],pid_fork);	
				}
		}
		else{
			//Zona del hijo
				
			execvp(args[0], args); /* <-- Sustituye todo el código por el comando que introduzcamos, de esta manera no tenemos que
										 desarrollar los comandos, tipo ls,pwd etc*/
			printf("\nError, comando ' %s ' no encontrado \n", args[0]);
			exit(-1); //Si el comando introducido es erróneo o no existe, saldrá con un error.

		}

	} 
}
