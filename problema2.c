#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

#include "cabecera123.h"



/*Proyecto de sistemas de operacion (problema 2)
 * Integrantes:
 * - César Bonadío
 * - Miguel Ordoñez
 */




void *fun(void *entero){ //funcion que ejecutan los hilos (propia del pronlema 2) - por eso no esta en el header

 struct intervalo *inter = entero;


     for (int i=(inter->desde)-1; i<(inter->hasta); i++){
           if (esprimo(numeros_archivo[i])){
            numero_primos++;}
     }

     pthread_exit(NULL); // termina la función del hilo
}




////////////////////////////////////////////////PRINCIPAL//////////////////////////////////////////////////////////////

int main (int argc, char *argv[]){

 int h[4];//vector h va a contener los enteros de entrada


 if (argc!=4) {mostrarerror2();}


 else{

  for (int i=1; i<argc; i++){
    h[i]=atoi(argv[i]);
    } 
  for (int i=0; i<argc; i++){
    printf ("%i\t",h[i]);
    }
  for (int i=0; i<argc; i++){
    printf ("%s\t",argv[i]);
  }
  printf ("\n\n\n");	



 ///se lee archivo y se asignan los numeros a un vector
   int cant_lineas = contar_lineas_archivo(argv[1]);
   numeros_archivo = malloc(cant_lineas*sizeof(int));
   int tamano= sizeof(numeros_archivo)/sizeof(int);
   abrir_archivo(argv[1],cant_lineas,numeros_archivo); // abre el archivo



  if (!((h[3]>=1)&&(h[3]<=10))){ //evalua si n hilos cumplen que 1<=n<=10
    printf("\n Error. Hilos deben estar entre 1 y 10\n\n");
  }




  else{
  int cant_trabajadores = h[3]; //cantidad de trabajadores procesos/hilos del proceso principal
  int trabajo = cant_lineas/cant_trabajadores; 
  int trabajo_n_1 = trabajo + (cant_lineas%cant_trabajadores);



  pthread_t hilos[cant_trabajadores]; // vector de hilos



  int d1 = 1 - trabajo; //d1 es desde donde va a leer el hilo/proceso
  int d2 = 0; //d2 es hasta donde va a leer el hilo/proceso





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

if (!inter){
    printf ("\n Error. Sin memoria\n "); //excepcion si no hay memoria
    exit(-1);
  }

  inter->desde=d1;
  inter->hasta=d2;
  inter->numero=i;



  int posible_error_hilo = pthread_create(&hilos[i], NULL, fun, inter);

    if (posible_error_hilo){
      printf ("\n Error creando un hilo\n"); //excepcion si no se crea un hilo
      free (inter);
      exit(-1);
    }


     pthread_join(hilos[i],NULL);// se van uniendo todos los hilos. Espera por el termino de todos los hilos

  }
   





  printf ("\n\n Cantidad de primos en el archivo: %i\n\n\n", numero_primos);
  
  }
 }
  return 0;
}// termina el main 

