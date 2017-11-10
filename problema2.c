#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#include <stdint.h>



/*Proyecto de sistemas de operacion (problema 2)
 * Integrantes:
 * - César Bonadío
 * - Miguel Ordoñez
 */

struct intervalo{ //struct intervalo de los numeros a leer por los hilos/procesos
  int desde;
  int hasta;
  int numero;
};
int *numeros_archivo; //vector de ambito global para los numeros del archivo
int numero_primos;


int esprimo(int n){//dice si un numero es primo o no

 int cont = 0;

  for (int i = 1; i<=n; i++){
    if (n%i==0)cont++;
  }

 if (cont == 2) return 1;
 return 0;
}


int contar_lineas_archivo(char *ubicacion){ //cuenta las lineas del fichero de entrada
  FILE *archivo;
   char caracter;
   int i = 0;

  archivo = fopen (ubicacion, "r");


  if (archivo==NULL){
  	printf("\n\nError en la apertura del archivo\n\n");
  }
  else{

    while((caracter = fgetc(archivo)) != EOF){
		if (caracter=='\n') i++;
	    }
	    fclose(archivo);
  }
 return (i);
}


void abrir_archivo(char *ubicacion, int lineas, int *numeros){ //asigna los numeros del archivo al vector global numeros_archivo

   FILE *archivo;

  archivo = fopen (ubicacion, "r");


  if (archivo==NULL){
  	printf("\n\nError en la apertura del archivo\n\n");
  }
  else{

    for (int i = 0; i<lineas; i++){
 	fscanf(archivo,"%d",&numeros[i]);
    }
	    fclose(archivo);
  }
}




void *fun(void *entero){ //funcion que ejecutan los hilos

 struct intervalo *inter = entero;

 printf("CREO CON: %i %i \n", inter->desde,inter->hasta);


     /*fprintf(archivo,"%i %i\n" , numeros[0],numeros[1]);*/

     for (int i=(inter->desde)-1; i<(inter->hasta); i++){
           if (esprimo(numeros_archivo[i])){
            numero_primos++;}
     }


     unsigned int p;
     p = pthread_self(); // id del hilo
     printf("Soy el hiloo %i de id %x \n", inter->numero,p);//el hilo saluda

     pthread_exit(NULL); 
}



void imprimir (int *vector, int tamano){
 for (int i=0; i<tamano; i++){
 	printf ("%i",vector[i]);
 }
}


void crearvector(int tamano){ // asigna un tamaño de memoria al vector global numeros_archivo, todo a partir de la cant de lineas registrada
  numeros_archivo = malloc(tamano*sizeof(int));
}




int main (int argc, char *argv[]){

 int h[3];//vector h va a contener los enteros de entrada


 if (argc!=3) {//
    printf ("\n ERROR. La ejecucion debe tener el siguiente formato:\n\n");
    printf (" ./primosn entrada.txt [n] donde:\n");
    printf ("\n * entrada.txt son los numeros a procesar\n");
    printf ("\n * n es la cantidad de hilos a crear\n\n\n");
    printf ("\n * los hilos se encargaran de devolver la cantidad de primos hallados\n");
  }


 else{

  for (int i=1; i<argc; i++)h[i]=atoi(argv[i]); 
  for (int i=0; i<argc; i++)printf ("%i\t",h[i]);
  for (int i=0; i<argc; i++)printf ("%s\t",argv[i]);
  printf ("\n\n\n");	


  	/*Documentacion de los vectores argv , h y el entero argc
  	 *
  	 * argv guarda todos los datos de entrada por espacios, sean strings , caracteres o enteros.
  	 * h guarda los enteros de argv , cuya posicion del entero no cambia en ambos, sigue siendo la misma.
  	 * argc guarda la cantidad de datos de entrada(entero). Ejemplo: ./ejecutable 87 p => argc= 3.
  	 */



 ///se lee archivo y se asignan los numeros a un vector
   int cant_lineas = contar_lineas_archivo(argv[1]);
   crearvector(cant_lineas);
   int tamano= sizeof(numeros_archivo)/sizeof(int);
   abrir_archivo(argv[1],cant_lineas,numeros_archivo); // abre el archivo



  if (!((h[2]>=1)&&(h[2]<=10))){ //evalua si n hilos cumplen que 1<=n<=10
    printf("\n Error. Hilos deben estar entre 1 y 10\n\n");
  }




  else{
  int cant_trabajadores = h[2]; //cantidad de trabajadores procesos/hilos del proceso principal
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
  inter->desde=d1;
  inter->hasta=d2;
  inter->numero=i;

    /*printf("MANDO EL: %i %i", inter->desde,inter->hasta);*/


  pthread_create(&hilos[i], NULL, /*(void *)*/fun, /*(void *)(intptr_t)i*/inter);
  }
   

  for (int i=1; i<=cant_trabajadores; i++){	
  pthread_join(hilos[i],NULL);// se van uniendo todos los hilos
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////////////


  printf ("\n\n Cantidad de primos en el archivo: %i\n\n\n", numero_primos);
  
  }
 }
  return 0;
}// termina el main 

