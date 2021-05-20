/**
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

// -----------------------------------------------------------------------
//  NOMBRE Y APELLIDOS: AITOR GUSTAVO CAMACHO GUTIERREZ
//  DNI:53741197-H        GRUPO : COMPUTADORES C (TARDE)
// 	Repositorio Github: https://github.com/AGCG1991/SSOO2021
// -----------------------------------------------------------------------

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

Para compilar y arrancar el programa:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell          
	(then type ^D to exit program)

**/

#include "job_control.h"   // remember to compile with module job_control.c 
#include <string.h>
#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

job *Lista_tareas; //Lista de tareas (lo declaro aquí para que las funciones tengan visibilidad sobre ella)


/*--> DECLARACIÓN DE FUNCIONES */

void manejador (int senal){
	block_SIGCHLD();
	job *proceso;
	int status,info,indice;
	int pid_wait=0; 
	enum status status_res;

	//List_size -> (Número de trabajos en la lista), esta empieza en el 1
	for (indice = 1 ; indice <= list_size(Lista_tareas) ; indice++)
	{	
		//Haremos un wait a cada uno de los comandos
		proceso= get_item_bypos(Lista_tareas,indice);

		/*Queremos detectar cuando un proceso es suspendido, por ello usamos WUNTRACED
		La única forma que tenemo de saber de que waitpid ha recogido un proceso, es que la variable 
		cambie de valor (pid_wait), por ello lo inicializamos con un valor de 0.
		Con WWNOHANG, compruebo si el proceso ha cambiado de estado.
	*/

		pid_wait = waitpid(proceso->pgid, &status, WUNTRACED | WNOHANG | WCONTINUED) ;

		if (pid_wait == proceso->pgid) 
		// El PID de un proceso recogido por waitpid, nunca será 0, y si no se recoge, no se le asigna valor
		{
			//Como ha cambiado de estado, usamos analyze para conocer en que estado se encuentra
			status_res = analyze_status(status, &info);

			/* (1) Si el estado del proceso es SUSPENDED (Suspendido)
				-Actualizamos la lista con el estado del proceso a STOPPED (Detenido)
			   (2)Si el estado del proceso es EXITED (TERMINADO)
			     -Tenemos que eliminarlo de la lista, con la función delete_job()

				 (*) El código debe de ser reentrante, los procesos en segundo plano irán terminando y lanzando
				 la señal SIGCHLD , usaremos una función bloqueante como block_SIGCHLD();
			
			*/
				if ((status_res == EXITED) || (status_res == SIGNALED))
				{
					printf(" \n El comando '%s' ejecutado en SEGUNDO plano con PID %d ha TERMINADO \n ",
				 proceso->command,proceso->pgid);
				 //Como el proceso ha terminado, lo sacamos de nuestra Lista de tareas
				 delete_job(Lista_tareas, proceso);
				 indice--; //Como hemos sacado una tarea de la Lista de tareas, el que le precedía, toma su lugar
			
					}  
					if (status_res == SUSPENDED) 
					{
				 printf("\n Comando '%s' ejecutado en SEGUNDO plano con PID %d se ha SUSPENDIDO \n ",
				 proceso->command,proceso->pgid);
				 proceso->state = STOPPED;

					} 
					if (status_res ==CONTINUED)
					{
						proceso->state = BACKGROUND;
						}
							}
		
	}
	unblock_SIGCHLD(); //Cuando terminemos con el proceso en curso, se deja entrar otra petición
}

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------

int main(void)
{
	char inputBuffer[MAX_LINE]; /* Búffer que alberga el comando introducido*/
	int background;             /* Su valor es 1 si el comando introducido finaliza con '&' */
	char *args[MAX_LINE/2];     /* La línea de comando tiene una longitud de 128 argumentos como máximo */

	// VARIABLES DE UTILIDAD :

	int pid_fork, pid_wait; /* pid for created and waited process */
	int status;             /* status returned by wait */
	enum status status_res; /* status processed by analyze_status() */
	int info;				/* info processed by analyze_status() */

	//NUEVAS DECLARACIONES DE VARIABLE
	job *proceso;
	int primerplano=0;
	//OJO
	ignore_terminal_signals(); // *** MACRO TRAIDA DEL JOB_CONTROL

	signal(SIGCHLD, manejador);
	Lista_tareas=new_list("Lista de tareas");

	while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{   		
		printf("\n Shell AGCG : Inserte comando->");
		fflush(stdout);
		get_command(inputBuffer, MAX_LINE, args, &background);  
		/* get_command va a recoger y leer  lo que el usuario introduzca en la línea de comandos, lo va a separar y lo añadirá a "args" desmembrado cada comando.
		(1)args es un array de Strings donde va a introducir cada "pieza" de los comandos introducidos
		 -(1.1) -> Por ejemplo, si introducimos "ls -l", tendremos args[0]="ls", args[1]="-l" y args[2]=NULL.
		(2) -> "background" (porque la va a modificar) es una variable pasada por referencia, se pondrá a 1 (true) si el comando termina con "&"
		clave para diferenciar un programa en primer plano o segundo plano
		*/ 
		
		if(args[0]==NULL) continue;   // Si se introduce un comando vacío, no hará nada, sólo seguirá pidiendo comandos
// -----------------------------------------------------------------------
//                IMPLEMENTACIÓN DE COMANDO CD          
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
			chdir(args[1]);  //Con chdir tratamos de implementar el comando interno "cd"
			continue;

		}
	/* 	if (!strcmp (args[0], "salir")){
			printf("\n Has salido de la Shell de Aitor \n");
			exit(0); //Función para salir de la shell . 
		} */
		// -----------------------------------------------------------------------
//                IMPLEMENTACIÓN DE COMANDO JOBS         
// -----------------------------------------------------------------------
		if (!strcmp (args[0], "jobs")){
			block_SIGCHLD();
			print_job_list(Lista_tareas); //Imprimo la lista de tareas
			unblock_SIGCHLD();
			continue;
		}
	// -----------------------------------------------------------------------
//                IMPLEMENTACIÓN DE COMANDO BG          
// -----------------------------------------------------------------------

		if (!strcmp (args[0], "bg")){
			block_SIGCHLD();
			int posicion =1;
			if (args[1] != NULL) {
				//Si no ha añadido como parámetro un número, es nulo
			posicion = atoi(args[1]) ; //Convertimos un String a un valor numérico
			unblock_SIGCHLD();
			continue;
		} //Tenemos que comprobar que el valor que se pasa por parámetro sea válido (posición de la lista de tareas)
			proceso=get_item_bypos(Lista_tareas,posicion); //<-- la función get_item_byposs comprueba el rango y
			// nos informa de si no se ha encontrado una tarea en esa posición
			if((proceso != NULL) && (proceso->state == STOPPED))
			{
				proceso -> state = BACKGROUND;
				killpg(proceso->pgid, SIGCHLD);
			}
			
		}

// -----------------------------------------------------------------------
//                IMPLEMENTACIÓN DE COMANDO FG          
// -----------------------------------------------------------------------

		if (!strcmp(args[0],"fg")) {
			block_SIGCHLD();
			int posicion = 1;
			primerplano=1; //La ponemos a TRUE
			if (args[1] !=NULL)
			{
				posicion=atoi(args[1]);
			}
			proceso= get_item_bypos(Lista_tareas,posicion);
			if ( proceso != NULL) {
				set_terminal(proceso->pgid); //Estamos cediendo el terminal de un proceso de 2º plano a 1º plano
				if(proceso->state == STOPPED) {
					killpg(proceso->pgid,SIGCONT);
				}
				pid_fork=proceso->pgid;
				delete_job(Lista_tareas,proceso);

			} 
			unblock_SIGCHLD();
			
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
			(2.5) WUNTRACED nos indica si el proceso ha sido suspendido
			(2.6) Con la función analyze_status , analiza el entero status por la llamada waitpid y devuelve la causa de terminación
			de una tarea y la información adicional asociada a dicha terminación (info)

		*/
	/* if(!primerplano) */ 
	pid_fork=fork();

		if(pid_fork >0){
			//Zona del padre
				/*Si viene de PRIMER plano */
			if (background ==0){
				waitpid(pid_fork,&status, WUNTRACED);
				set_terminal(getpid()); /*Con esta función recupero el terminal, getpid me da el PID*/
				
				status_res = analyze_status(status,&info); //Guardamos en una variable el estado actual del proceso

				if(status_res == SUSPENDED) { //En caso de que haya sido SUSPENDIDO
				block_SIGCHLD();
				//Si el trabajo es suspendido, hay que añadirlo a la lista e indicar que ha sido detenido (STOPPED)
				proceso=new_job(pid_fork, args[0], STOPPED);
					
					add_job(Lista_tareas,proceso);

				printf ("\n Comando  ' %s '  ejecutado en PRIMER plano con PID %d . Estado %s. Info %d \n",
				args[0],pid_fork, status_strings[status_res],info);
				unblock_SIGCHLD();

				} else if (status_res == EXITED) //En caso de que haya FINALIZADO
				{
					if(info !=255 ){ //Si es distinto a 255 es que no ha habido un
					printf ("\n Comando  ' %s '  ejecutado en PRIMER plano con pid %d . Estado %s. Info %d \n",
					args[0],pid_fork, status_strings[status_res] ,info);
				}
				}
				primerplano=0; //De esta forma indicamos de que ya no hay un proceso en PRIMER PLANO
			}
			else {		/*Se le pasa como parámedtro (PID,COMANDO (que se en cuentra en la posición 0 del array args))
							y el estado (funcion job_state (state))*/
					block_SIGCHLD();
					proceso=new_job(pid_fork, args[0], BACKGROUND);
					//Inserto el nuevo trabajo a la lista de tareas y el proceso
					add_job(Lista_tareas,proceso);
					printf("\n comando ' %s '  ejecutado en SEGUNDO plano con pid %d ",args[0],pid_fork);	
					unblock_SIGCHLD();
				}
		}
		else{
			//Zona del hijo
			new_process_group(getpid());
			if (background == 0){
				set_terminal(getpid());
			}
			restore_terminal_signals();
			execvp(args[0], args); /* <-- Sustituye todo el código por el comando que introduzcamos, de esta manera no tenemos que
										 desarrollar los comandos, tipo ls,pwd etc*/
			printf("\nError, comando ' %s ' no encontrado \n", args[0]);
			exit(-1); //Si el comando introducido es erróneo o no existe, saldrá con un error.

		}

	} /*Final del bucle infinito WHILE*/
}
