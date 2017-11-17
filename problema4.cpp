//---------- Librerías Propias -----------

#include "semaforo.h"
#include "cargarArchivotxt.h"


//---------- Librerías de C -----------

#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>


/*Proyecto de sistemas de operacion (problema 4)
 * Integrantes:
 * - César Bonadío
 * - Miguel Ordoñez
 */


//---------- Cabecera de funciones -----------

void bisRigth();
void bisLeft();
void sendero();

void inicializarVariablesGlobales();
void liberarMemoria();
void* crearMemoriaCompartida(size_t size);

char* aumentarHora(char *horaBici, int aumento);
void contarBicicletaMismoSentido(int sentidoActual);

Bicicleta* obtenerBicicletaConSentido(int sentido);
void guardarBicicleta(char *hora, int sentido);

// --------- Semáforos ---------

const char *SemNameA = "SemA";
const char *SemNameB = "SemB";
const char *SemNameC = "SemC"; 

sem_t *semA = inicializarSemaforo(1,SemNameA);
sem_t *semB = inicializarSemaforo(0,SemNameB);
sem_t *semC = inicializarSemaforo(0,SemNameC);

// -------- Varables "Normales" -----------

const char *ubicacionEntrada = "entrada2.txt";
const char *ubicacionSalida = "salida.txt";

//Estados
int ON = 1;
int OFF = 0; 


// ----- Variables Compartidas -------

char *horaDerecha = (char*)crearMemoriaCompartida(sizeof (char*));
char *horaIzquierda = (char*)crearMemoriaCompartida(sizeof (char*));

int *sentidoSendero = (int*)crearMemoriaCompartida(sizeof (int*));
char *horaSendero = (char*)crearMemoriaCompartida(sizeof (char*));

int *procesoDerecha = (int*)crearMemoriaCompartida(sizeof (int*));
int *procesoIzquierda = (int*)crearMemoriaCompartida(sizeof (int*));
int *procesoSendero = (int*)crearMemoriaCompartida(sizeof (int*));

//Para la condición de las 10 bicicletas en un mismo sentido
int *cantidadMaximaBicicletas = (int*)crearMemoriaCompartida(sizeof (int*));

int *posD = (int*)crearMemoriaCompartida(sizeof (int*));
int *posI = (int*)crearMemoriaCompartida(sizeof (int*));


int main(){

   cargarArchivotxt(ubicacionEntrada);
   //Carga el archivo txt en la estructura Bicicleta

   inicializarVariablesGlobales();

   pid_t pid;
   pid = fork();

   //Padre
   if(pid != 0){
      
      pid = fork();

      //Hijo_2
      if(pid == 0)
         bisRigth();

      else{
         pid = fork();

         //Hijo_3
         if(pid == 0)
            sendero();
         else
            sleep(3);
      }
         
   }

   //Hijo_1
   else
      bisLeft();


   //El padre borra el semáforo una vez terminado el programa
   if(pid != 0)
      liberarMemoria();
      
   printf("\n\n");
   return 0;

} 


void bisRigth(){

   Bicicleta *aux;

   while(1 == 1){

      down(semA);

      //Obtiene una bicicleta con sentido derecho
      aux = obtenerBicicletaConSentido(derecha);

      if(aux != NULL)
         strcpy(horaDerecha,aux->hora);
      else
         *procesoDerecha = OFF;

      up(semB);

      if(*procesoSendero == OFF)
         break;

   }

   printf("HE SALIDO DE BISRIGTH \n"); 

}


void bisLeft(){

   Bicicleta* aux;

   while(1 == 1){

      down(semB);

      //Obtiene una bicicleta con sentido izquierdo
      aux = obtenerBicicletaConSentido(izquierda);

      if(aux != NULL)
         strcpy(horaIzquierda,aux->hora);
      else
         *procesoIzquierda = OFF;

      up(semC);

      if(*procesoSendero == OFF)
         break;

   }

   printf("HE SALIDO DE BISLEFT \n");

}


void sendero(){

   int respuesta;

   while(1 == 1){

      down(semC);

      /*En caso de que existan 2 bicicletas (R y L) se procede a compararlas para ver cual tiene mas prioridad*/
      if( (*procesoDerecha == ON)&&(*procesoIzquierda == ON) ){

         respuesta = strcmp(horaIzquierda,horaDerecha);

         //horahoraIzquierda MENOR QUE horaDerecha
         if(respuesta < 0){
            (*posI)++;
            guardarBicicleta(horaIzquierda,izquierda);
         }
         else
            //horahoraIzquierda MAYOR QUE horaDerecha
            if(respuesta > 0){
               (*posD)++;
               guardarBicicleta(horaDerecha,derecha);
            }
            //horahoraIzquierda IGUAL QUE horaDerecha
            else{

               (*posD)++;
               (*posI)++;

               if(*sentidoSendero == derecha){
                  guardarBicicleta(horaDerecha,derecha);
                  guardarBicicleta(horaIzquierda,izquierda);
               }
               else{
                  guardarBicicleta(horaIzquierda,izquierda);
                  guardarBicicleta(horaDerecha,derecha);
               }

            }

      }//Fin de la Comparación

      //En caso de que solo exista una sola bicicleta, es decir, un solo sentido
      else{

         //Si es una bicicleta derecha
         if(*procesoDerecha == ON){
            (*posD)++;
            guardarBicicleta(horaDerecha,derecha);
         }
         else
            //Si es una bicicleta izquierda
            if(*procesoIzquierda == ON){
               (*posI)++;
               guardarBicicleta(horaIzquierda,izquierda);
            }

      }

      up(semA);

      //Caso de que ya no hayan bicicletas
      if( (*procesoDerecha == OFF)&&(*procesoIzquierda == OFF) ){
         *procesoSendero = OFF;
         break;
      }

   }//Fin del While

}


void inicializarVariablesGlobales(){

   *procesoIzquierda = ON;
   *procesoDerecha = ON;
   *procesoSendero = ON;

   *sentidoSendero = -1;
   strcpy(horaSendero,"");

   strcpy(horaDerecha,"");
   strcpy(horaIzquierda,"");

   *posD = 0;
   *posI = 0;

   *cantidadMaximaBicicletas = 10;

}


void liberarMemoria(){

   /*munmap se encarga de liberar la memoria compartida creada con mmap, en caso de error devuelve -1 y en caso de éxito devuelve 0*/

   if(munmap(horaDerecha, sizeof (char*)) != 0)
      printf("ERROR... Al eliminar horaDerecha \n"); 

   if(munmap(horaIzquierda, sizeof (char*)) != 0)
      printf("ERROR... Al eliminar horaIzquierda \n"); 

   if(munmap(sentidoSendero, sizeof (int*)) != 0)
      printf("ERROR... Al eliminar sentidoSendero \n"); 

   if(munmap(horaSendero, sizeof (char*)) != 0)
      printf("ERROR... Al eliminar horaSendero \n"); 

   if(munmap(procesoDerecha, sizeof (int*)) != 0)
      printf("ERROR... Al eliminar procesoDerecha \n"); 

   if(munmap(procesoIzquierda, sizeof (int*)) != 0)
      printf("ERROR... Al eliminar procesoIzquierda \n");

   if(munmap(procesoSendero, sizeof (int*)) != 0)
      printf("ERROR... Al eliminar procesoSendero \n");

   if(munmap(cantidadMaximaBicicletas, sizeof (int*)) != 0)
      printf("ERROR... Al eliminar cantidadMaximaBicicletas \n");

   if(munmap(posD, sizeof (int*)) != 0)
      printf("ERROR... Al eliminar posD \n");

   if(munmap(posI, sizeof (int*)) != 0)
      printf("ERROR... Al eliminar posI \n");

   //Se libera el puntero a la estructura de bicicletas
   free(b);

   //Se libera los semáforos
   borrarSemaforo(semA, SemNameA);
   borrarSemaforo(semB, SemNameB);
   borrarSemaforo(semC, SemNameC);

}


void* crearMemoriaCompartida(size_t size) {

   int proteccion = PROT_READ | PROT_WRITE;
   //El buffer de la memoria será de Lectura y Escrituta

   int permiso = MAP_ANONYMOUS | MAP_SHARED;
   /*MAP_SHARED significa que la memoria va a ser compartida entre varios procesos y MAPANONYMOUS significa que NO asocia la región compartida a ningún fichero en disco. Por lo que se concluye que es una memoria que solo padres e hijos prodran usarla*/

   return mmap(NULL, size, proteccion, permiso,0,0);
   /*El parametro NULL indica que el núcleo elige la dirección en la que desea crear el mapeo*/

}


char* aumentarHora(char *horaBici, int aumento){

   int horaInt, minutoInt, size = 15;
   char *horaChar, *minutoChar;

   //Extrae las horas y minutos de la hora total de la bici
   horaChar = subString(horaBici,0,1);
   minutoChar = subString(horaBici,3,4);

   //Convierte los char en int
   horaInt = atoi (horaChar);
   minutoInt = atoi (minutoChar);

   minutoInt += aumento;

   //Convierte los int en char (Ya aumentados en UNA unidad)
   snprintf(horaChar,size,"%d",horaInt);
   snprintf(minutoChar,size,"%d",minutoInt);

   //Concatena la hora y minuto
   strcat(horaChar,":");
   strcat(horaChar,minutoChar);

   //Retorna la hora Total
   return horaChar;

}


void contarBicicletaMismoSentido(int sentidoActual){

   //Sentido contrario
   if(*sentidoSendero != sentidoActual)
      *cantidadMaximaBicicletas = 10;

   (*cantidadMaximaBicicletas)--;

   if(*cantidadMaximaBicicletas == 0){

      FILE *archivo;
      archivo = fopen (ubicacionSalida, "a");

      if (archivo == NULL){
         printf("ERROR en la apertura del archivo \n\n");
         exit(EXIT_FAILURE);
      }
      else{

         //Se imprime el mensaje en el archivo
         fputs("Esperando que salgan las últimas 10 \n",archivo);

         fclose(archivo);

      }

      //Se aumenta la hora del sendero en 2 unidades, para que todas las bicicletas esperen
      strcpy( horaSendero,aumentarHora(horaSendero,2) );
      *cantidadMaximaBicicletas = 10;
   }

}



void guardarBicicleta(char *hora, int sentido){

   FILE *archivo;
   archivo = fopen (ubicacionSalida, "a");

   if (archivo == NULL){
      printf("ERROR en la apertura del archivo \n\n");
      exit(EXIT_FAILURE);
   }
   else{

      //NO ha circulado ninguna bicicleta
      if( strcmp(horaSendero,"") == 0 ) 
          strcpy(horaSendero,hora);
      else{

         //La hora actual es menor que la del sendero
         if( strcmp(hora,horaSendero) <= 0 )
            horaSendero = aumentarHora(horaSendero, 1);
         else
            strcpy(horaSendero,hora);
      }

      fputs(horaSendero,archivo);
      fputs(" ",archivo);

      if(sentido == izquierda)
         fputs("L\n",archivo);
      else
         fputs("R\n",archivo);

      fclose(archivo);

   }

   //Verifica si han circulado 10 bicicletas en el mismo sentido
   contarBicicletaMismoSentido(sentido);
   *sentidoSendero = sentido;

}


Bicicleta* obtenerBicicletaConSentido(int sentido){

   Bicicleta *aux = b;
   int posicion = 0;

   int minimo;


   if(sentido == derecha)
      minimo = *posD;
   else
      minimo = *posI;
   

   while( (aux != NULL) && ( (posicion < minimo)||(aux->sentido != sentido) ) ){
      aux = aux->prox;
      posicion++;
   }

   if(sentido == derecha)
       *posD = posicion;
    else
       *posI = posicion;

   return aux;

}


