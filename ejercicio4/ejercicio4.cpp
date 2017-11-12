//---------- Librerías Propias -----------

#include "semaforo.h"
#include "cargarArchivotxt.h"


//---------- Librerías de C -----------

#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>


//---------- Funciones de Principales -----------

void bisRigth();
void bisLeft();
void* crearMemoriaCompartida(size_t size);


//---------- Funciones de Secundarias -----------

void inicializarVariablesGlobales();

void verificarHora(Bicicleta *aux, sem_t *semPropio, sem_t *semHermano);
int verificarEstadoProcesoHermano();

Bicicleta* obtenerBici(int sentido,int *minimo);
void imprimirBici(Bicicleta *aux);

//int verificarSentido(Bicicleta *aux);


// --------- Semáforos ---------

sem_t *SemD = inicializarSemaforo(0,"Derecha");
sem_t *SemI = inicializarSemaforo(0,"Izquierda");


// -------- Varables "Normales" -----------

const char *ubicacion = "entrada2.txt";

//Estados
int on = 1;
int off = 0; 


// ----- Variables Compartidas -------

char *horaSendero = (char*)crearMemoriaCompartida(sizeof (char*));
int *sentidoSendero = (int*)crearMemoriaCompartida(sizeof (int*));
int *estadoProcesoHermano = (int*)crearMemoriaCompartida(sizeof (int*));


int main(){

   cargarArchivotxt(ubicacion);
   //Se carga el archivo txt en la estructura Bicicleta

   inicializarVariablesGlobales();



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
         bisRigth();

         //Para que no se haga pasar por el padre
         pid1 = 0;
      }
      else
         sleep(3);

   }

   //Hijo_1
   else
      bisLeft();


   if(pid1 != 0){
      borrarSemaforo(SemI, "Izquierda");
      borrarSemaforo(SemD, "Derecha");
   }
      
   printf("\n\n");
   return 0;

} 




void bisRigth(){

   int posD = 0;
   Bicicleta *aux;

   while(1 == 1){

      aux = obtenerBici(derecha,&posD);

      if(aux == NULL){
         up(SemI);
         break;
      }

      verificarHora(aux,SemD,SemI);

      //Asignar un sendero al sendero
      *sentidoSendero = aux->sentido;

      posD++;
      imprimirBici(aux);

   }

   //Ha "fallecido" terminado el proceso
   *estadoProcesoHermano = off;

   printf("HE SALIDO DE BISRIGTH \n"); 

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

      verificarHora(aux,SemI,SemD);

      //Asignar un sendero al sendero
      *sentidoSendero = aux->sentido;

      posI++;
      imprimirBici(aux);

   }

   //Ha "fallecido" terminado el proceso
   *estadoProcesoHermano = off;

   printf("HE SALIDO DE BISLEFT \n");

}



void* crearMemoriaCompartida(size_t size) {

   int proteccion = PROT_READ | PROT_WRITE;
   //El buffer de la memoria será de Lectura y Escrituta

   int permiso = MAP_ANONYMOUS | MAP_SHARED;
   /*MAP_SHARED significa que la memoria va a ser compartida entre varios procesos y MAPANONYMOUS significa que NO asocia la región compartida a ningún fichero en disco. Por lo que se concluye que es una memoria que solo padres e hijos prodran usarla*/

   return mmap(NULL, size, proteccion, permiso,0,0);
   /*El parametro NULL indica que el núcleo elige la dirección en la que desea crear el mapeo*/

}



void inicializarVariablesGlobales(){

   *estadoProcesoHermano = on;
   *sentidoSendero = -1;
   strcpy(horaSendero,"");

}


void verificarHora(Bicicleta *aux, sem_t *semPropio, sem_t *semHermano){

   //Entra la primera vez
   if(strcmp("",horaSendero) == 0){
      strcpy(horaSendero,aux->hora);
      down(semPropio); 
   }

   //Ya existe una hora en el sendero
   else{

      int respuesta = strcmp(aux->hora,horaSendero);

      if( (respuesta > 0)&&(verificarEstadoProcesoHermano() == 0) ){

         strcpy(horaSendero,aux->hora);
         up(semHermano);
         down(semPropio);

      }

   }


}


int verificarEstadoProcesoHermano(){

   //Se debe verificar que el proceso hermano siga vivo
   if(*estadoProcesoHermano == off)
      return 1;
   else
      return 0;

}


Bicicleta* obtenerBici(int sentido,int *minimo){

   Bicicleta *aux = b;
   int posicion = 0;

   while( (aux != NULL) && ( (posicion < *minimo)||(aux->sentido != sentido) ) ){
      aux = aux->prox;
      posicion++;
   }

   *minimo = posicion;

   return aux;

}


void imprimirBici(Bicicleta *aux){

   printf("  %s  %i  \n ",aux->hora,aux->sentido);

}


int verificarSentido(Bicicleta *aux){

   //Se debe verificar que el proceso hermano siga vivo
   if(*estadoProcesoHermano == off)
      return 1;
   else
      return 0;

   //if(*sentidoSendero == aux->sentido)
     // return 1;
   //else
     // return 0; 

}






