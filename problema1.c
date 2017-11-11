#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>

#include "cabecera123.h"


/*Proyecto de sistemas de operacion (problema 1)
 * Integrantes:
 * - César Bonadío
 * - Miguel Ordoñez
 */



struct timeval t0 , t1 ;
double media = 0.0;





void *fun(void *entero){ //funcion que ejecutan los hilos (propia del problema 1). por eso no esta en el header

 struct intervalo *inter = entero;
 unsigned int p;
 p = pthread_self();


 printf("SOY EL HILO %i DE ID %x, ", inter->numero, p);
 printf("CREO CON: %i %i \n", inter->desde,inter->hasta);


     FILE *archivo;
     char subi = (char)(inter->numero)+'0';

     char stor[5] = ".txt"; 
     char str1[1];
     str1[0]= subi;

     if (inter->numero == 10){
     archivo = fopen ("10.txt", "w+");}
     else{
     archivo = fopen (str1, "w+");
     }


     for (int i=(inter->desde)-1; i<(inter->hasta); i++){
           if (esprimo(numeros_archivo[i])){
            fprintf(archivo, "%i %i\n", numeros_archivo[i], 1);
          }else{
            fprintf(archivo, "%i %i\n", numeros_archivo[i], 0);
          }   
     }

   
     
     free(inter);
  
     //esto esta malo por ahora
     gettimeofday (& t1 , NULL );
     unsigned int ut1 = t1 . tv_sec *1000000+ t1 . tv_usec ;
     unsigned int ut0 = t0 . tv_sec *1000000+ t0 . tv_usec ;
     media += ( ut1 - ut0 );
     pthread_exit(NULL); 
}







////////////////////////////////////////////////PRINCIPAL//////////////////////////////////////////////////////////////

int main (int argc, char *argv[]){

 int h[4];//vector h va a contener los enteros de entrada


 if (argc!=4) {mostrarerror();} // si no hay 4 argumentos hay error



 else {

  for (int i=1; i<argc; i++)h[i]=atoi(argv[i]); 
  for (int i=0; i<argc; i++)printf ("%i\t",h[i]);
  printf ("%s",argv[2]);
  printf ("\n\n\n");	


  	/*Documentacion de los vectores argv , h y el entero argc
  	 *
  	 * argv guarda todos los datos de entrada por espacios, sean strings , caracteres o enteros.
  	 * h guarda los enteros de argv , cuya posicion del entero no cambia en ambos, sigue siendo la misma.
  	 * argc guarda la cantidad de datos de entrada(entero). Ejemplo: ./ejecutable 87 p => argc= 3.
  	 */



 ///se lee archivo y se asignan los numeros a un vector
   int cant_lineas = 0;
   cant_lineas = contar_lineas_archivo(argv[1]);
   numeros_archivo = malloc(cant_lineas*sizeof(int));
   int tamano= sizeof(numeros_archivo)/sizeof(int);
   abrir_archivo(argv[1],cant_lineas,numeros_archivo);





 if (((strcmp(argv[2],"-t")!=0)&&((strcmp(argv[2],"-p")!=0)))||(!((h[3]>=1)&&(h[3]<=10)))){mostrarerror();}



 else{

  int cant_trabajadores = h[3]; //cantidad de trabajadores procesos/hilos del proceso principal
  int trabajo = cant_lineas/cant_trabajadores; 
  int trabajo_n_1 = trabajo + (cant_lineas%cant_trabajadores);
  int d1 = 1 - trabajo; //d1 es desde donde va a leer el hilo/proceso
  int d2 = 0; //d2 es hasta donde va a leer el hilo/proceso





  if (strcmp(argv[2],"-t")==0){
    printf ("\n\nQuiero ejecutar Hilos\n");

    struct timeval t0 , t1 ;
    int i = 0;
    int id = -1;
    double media = 0.0;


  pthread_t hilos[cant_trabajadores]; // vector de hilos




  //////////////////////////////////se crean y unen hilos//////////////////////////////////
  for (int i=1; i<=cant_trabajadores; i++){


  if (i == cant_trabajadores){
  d1 = d1 + trabajo;
  d2 = i * trabajo + (cant_lineas%cant_trabajadores);}
  else{
  d1 = d1 + trabajo;
  d2 = i * trabajo;
  }

  struct intervalo *inter;
  inter = malloc(sizeof(struct intervalo));
  inter->desde=d1;
  inter->hasta=d2;
  inter->numero=i;


  pthread_create(&hilos[i],NULL,fun,inter);
  }
   
  


  for (int i=1; i<=cant_trabajadores; i++){
  pthread_join(hilos[i],NULL);// se van uniendo todos los hilos
  }


  //sleep(1);
 
  printf ( " \n\n Tiempo de ejecucion con %i hilos: %f microsegundos\n" , h[3] , (media/cant_trabajadores));
  /* Tiempo medio medido en microsegundos*/ 
  ///////////////////////////////////////////////////////////////////////////////////////////////////////


  }//termina el caso de ser hilos









else if (strcmp(argv[2],"-p")==0){
    printf ("\n\nQuiero ejecutar procesos\n");
    printf ("Proceso padre: %i \n\n\n", getpid());

    struct timeval t0 , t1 ;
    int i = 0;
    int id = -1;
    int procesos[cant_trabajadores];
    gettimeofday (& t0 , NULL ) ;


   for ( i=1 ; i <=cant_trabajadores; i++) {


    
         if (i == cant_trabajadores){
        d1 = d1 + trabajo; 
        d2 = i * trabajo + (cant_lineas%cant_trabajadores);
        }else{
        d1 = d1 + trabajo;
        d2 = i * trabajo;
        }

        struct intervalo inter;
        inter.desde=d1;
        inter.hasta=d2;
        inter.numero=i;


  id = fork ();

      if ( id == 0) {
     funpro(inter);
      break;
    }
  }




   if ( id != 0) {
    sleep(1);
    gettimeofday (& t1 , NULL );
    unsigned int ut1 = t1 . tv_sec *1000000+ t1 . tv_usec ;
    unsigned int ut0 = t0 . tv_sec *1000000+ t0 . tv_usec ;
    printf ( " \n\n Tiempo de ejecucion con %i procesos: %i microsegundos\n" , h[3] ,(( ut1 - ut0 )/cant_trabajadores)-10000);
    /* Tiempo medio medido en microsegundos*/ 
   } 

} // termina el caso de ser procesos







   } // termina el else de si la entrada es valida


 }//termina el else de la cantidad de argumentos

  return 0;
}// termina el main 
