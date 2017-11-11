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


/*Proyecto de sistemas de operacion (problema 3)
 * Integrantes:
 * - César Bonadío
 * - Miguel Ordoñez
 */


////////////////////////////////////////////////PRINCIPAL//////////////////////////////////////////////////////////////

int main (int argc, char *argv[]){

 int h[3];//vector h va a contener los enteros de entrada


 if (argc!=3) {mostrarerror();} // si no hay 4 argumentos hay error



 else {

  for (int i=1; i<argc; i++)h[i]=atoi(argv[i]); 
  for (int i=0; i<argc; i++)printf ("%i\t",h[i]);
  printf ("%s",argv[2]);
  printf ("\n\n\n");	



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




if (strcmp(argv[2],"-p")==0){
    printf ("\n\nQuiero ejecutar procesos\n");
    printf ("Proceso padre: %i \n\n\n", getpid());

    int i = 0;
    int id = -1;


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
    signal(SIGINT, catch_signal_ctrlC);
    while(1);
   } 


}// es proceso



   }


 }//termina el else de la cantidad de argumentos

  return 0;
}// termina el main 
