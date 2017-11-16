// ------- Librerías --------

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include <fcntl.h>
//Incluye los indicadores de estado del archivo como O_CREAT

#include <sys/stat.h> 
//Incluye las constantes de modo como mode_t (0777,0666,..)


// ------ Variables globales --------

const mode_t constanteModo = 0666;
/* 0666 incluye 2 permisos básicos (r y w), mientras que 0777 incluye los 3

    r - Lectura
    w - Escritura
    x - Ejecución

*/


// ------- Cabecera de funciones --------

sem_t* inicializarSemaforo(int valor, const char *nombreSemaforo);
void borrarSemaforo(sem_t *semaforo, const char *nombreSemaforo);
void down(sem_t *semaforo);
void up(sem_t *semaforo);
int getValor(sem_t *semaforo);


// ------- Implementación --------

sem_t* inicializarSemaforo(int valor, const char *nombreSemaforo){

   sem_t *semaforo;

   semaforo = sem_open(nombreSemaforo, O_CREAT, constanteModo, valor);
   /*Nombre del semaforo, O_CREAT indica que se va a crear un semaforo, la constante de modo y por último el valor de inicialización*/

   //Si sem_open presenta agún error devuelve SEM_FAILED
   if(semaforo == SEM_FAILED){
      printf("ERROR al inicializar el semaforo '%s' \n\n" , nombreSemaforo);
      exit(EXIT_FAILURE);
   }

   return semaforo;

}



void borrarSemaforo(sem_t *semaforo, const char *nombreSemaforo){

   int error;
   error = sem_close(semaforo);

   //El semáforo se cerró con éxito
   if(error == 0){

      error = sem_unlink(nombreSemaforo);

      //El semáforo se eliminó con éxito
      if(error == 0)
         printf("El semaforo '%s' se elimino con éxito!! \n\n",nombreSemaforo);

      else{
         printf("ERROR al eliminar el semaforo '%s' \n\n" , nombreSemaforo);
         exit(EXIT_FAILURE);
      }

   }
   else{
      printf("ERROR al cerrar el semaforo '%s' \n\n" , nombreSemaforo);
      exit(EXIT_FAILURE);
   }

}



void down(sem_t *semaforo){
   
   int error;
   error = sem_wait(semaforo);

   if(error != 0)
      printf("ERROR al disminuir el semaforo");

}



void up(sem_t *semaforo){
   
   int error;
   error = sem_post(semaforo);

   if(error != 0)
      printf("ERROR al aumentar el semaforo");

}



int getValor(sem_t *semaforo){

   int valor;
   sem_getvalue (semaforo, &valor);
   return valor;

}
