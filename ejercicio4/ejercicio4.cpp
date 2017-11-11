#include "semaforo.h"
#include "cargarArchivotxt.h"


//AQUI
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>

void bisRigth();
void bisLeft();
void* crearMemoriaCompartida(size_t size);
Bicicleta* obtenerBicicleta();

sem_t *Final;
const char *semName2 = "Semaphore2";

const char *HoraSistema = (char*)crearMemoriaCompartida(sizeof (char*)); 
//Se inicializa en ""

int *posActual = (int*)crearMemoriaCompartida(sizeof (int*));
//Se inicializa en 0





void compararHora(Bicicleta *aux, sem_t *semPropio, sem_t *semHermano);
void imprimirBici(Bicicleta *aux);
Bicicleta* obtenerBici(int sentido,int *minimo);

sem_t *SemD = inicializarSemaforo(0,"Derecha");
sem_t *SemI = inicializarSemaforo(0,"Izquierda");

//Hasta



sem_t *Semaforo;
const char *semName = "Semaphore";
const char *ubicacion = "entrada2.txt";




int main(){

   cargarArchivotxt(ubicacion);
   Semaforo = inicializarSemaforo(1,semName);
   
   Final = inicializarSemaforo(0,semName2);


/*
pid_t pid;
 int x;
 
 for(x=1;x<=3;x++)
 {
  
  pid=fork();
   printf("PID: %i      ",pid); 
  if(pid)
  {
   printf("PID2: %i     ",pid); 
   printf("Soy el proceso %d \n",getpid());
   sleep(2);
  }

  else
  {
   printf("soy el hijo %d, mi padre es %d\n",getpid(),getppid());
   sleep(2);
   exit(0);
  }
  
 }*/

   b->prox->prox->prox->prox = NULL;
   //borrarSemaforo(Semaforo, semName);
   printf(" HOLA!!!!!!!!!!! \n\n");

   pid_t pid1, pid2;
   Bicicleta *b1, *b2;

   pid1 = fork();

   //Padre
   if(pid1 != 0){
      
      pid2 = fork();

      //Hijo_2
      if(pid2 == 0){
         down(Semaforo);
         b1 = obtenerBicicleta();
         bisRigth();
         up(Semaforo);
      }
      else
         sleep(3);

   }

   //Hijo_1
   else{

      down(Semaforo);
      b2 = obtenerBicicleta();
      bisLeft();
      up(Semaforo);

   }

   //printf("QUE PASA PS \n");
   if(pid1 != 0){
      //down(Semaforo);
      //down(Final);
      //borrarSemaforo(Semaforo, semName);
      //borrarSemaforo(Final, semName2);
      //printf("X: %i", *x);
      //sleep(5);
      borrarSemaforo(SemI, "Izquierda");
      borrarSemaforo(SemD, "Derecha");
   }
   else{
      //down(Semaforo);
      //printf("YA yo termine");
      //printf("X HIJO: %i", *x);
      //up(Final);
   }
      
   

   

   printf("\n\n");
   return 0;

} 



char *horaBicis = (char*)crearMemoriaCompartida(sizeof (char*));
//horaBicis = NULL;


void bisRigth(){

   int posD = 0;
   Bicicleta *aux;

   while(1 == 1){

      aux = obtenerBici(derecha,&posD);

      if(aux == NULL){
         up(SemI);
         break;
      }

      compararHora(aux,SemD,SemI);

      posD++;
      imprimirBici(aux);

   }

   printf("HE SALIDO DE BISRIGTH \n");
   //printf("Sendero Derecha: %s ",aux->hora);
   //sleep(1);

}



void bisLeft(){

   int posI = 0;
   Bicicleta* aux;

   while(1 == 1){

      aux = obtenerBici(izquierda,&posI);

      if(aux == NULL){
         up(SemD);
         break;
      }

      compararHora(aux,SemI,SemD);

      posI++;
      imprimirBici(aux);

   }

   printf("HE SALIDO DE BISLEFT \n");


   //printf("Sendero Izquierda: %s ",aux->hora);
   //sleep(1);

}



void* crearMemoriaCompartida(size_t size) {

   int proteccion = PROT_READ | PROT_WRITE;
   //El buffer de la memoria será de Lectura y Escrituta

   int permiso = MAP_ANONYMOUS | MAP_SHARED;
   /*MAP_SHARED significa que la memoria va a ser compartida entre varios procesos y MAPANONYMOUS significa que NO asocia la región compartida a ningún fichero en disco. Por lo que se concluye que es una memoria que solo padres e hijos prodran usarla*/

   return mmap(NULL, size, proteccion, permiso,0,0);
   /*El parametro NULL indica que el núcleo elige la dirección en la que desea crear el mapeo*/

}



Bicicleta* obtenerBicicleta(){
   
   Bicicleta *aux = b;
   int posicion = 0;

   if( (aux != NULL)&&(posicion != *posActual) ){
      aux = aux->prox;
      posicion++;
   }

   //Aumenta la bicicleta actual
   (*posActual)++;

   return aux;

}



void compararHora(Bicicleta *aux, sem_t *semPropio, sem_t *semHermano){

   if(horaBicis == NULL){
      strcpy(horaBicis,aux->hora);
      down(semPropio);
   }
   else{

      int respuesta = strcmp(aux->hora,horaBicis);

      if(respuesta > 0){
         up(semHermano);
         strcpy(horaBicis,aux->hora);
         down(semPropio);
      }

   }


}



void imprimirBici(Bicicleta *aux){

   printf("  %s  %i  \n ",aux->hora,aux->sentido);

}



Bicicleta* obtenerBici(int sentido,int *minimo){

   Bicicleta *aux = b;
   int posicion = 0;

   if( (aux != NULL) && ( (posicion < *minimo)||(aux->sentido != sentido) ) ){
      aux = aux->prox;
      posicion++;
   }

   *minimo = posicion;

   return aux;

}












