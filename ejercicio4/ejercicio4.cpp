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

void elegirCamino(Bicicleta *bici, int direccion);

void inicializarVariablesGlobales();
void* crearMemoriaCompartida(size_t size);

char* sumarUNAunidad(char *horaBici);

Bicicleta* obtenerBici(int sentido,int *minimo);
void imprimirBici(Bicicleta *aux);


void contarBicicleta(int sentidoActual);
void sumarDOSunidades();

// --------- Semáforo ---------

sem_t *mutex = inicializarSemaforo(1,"Mutex");

sem_t *semA = inicializarSemaforo(0,"SemA");
sem_t *semB = inicializarSemaforo(0,"SemB");

// -------- Varables "Normales" -----------

const char *ubicacion = "entrada2.txt";

//Estados
int on = 1;
int off = 0; 


// ----- Variables Compartidas -------

char *horaDerecha = (char*)crearMemoriaCompartida(sizeof (char*));
char *horaIzquierda = (char*)crearMemoriaCompartida(sizeof (char*));

int *sentidoSendero = (int*)crearMemoriaCompartida(sizeof (int*));
char *horaSendero = (char*)crearMemoriaCompartida(sizeof (char*));

int *estadoProcesoHermano = (int*)crearMemoriaCompartida(sizeof (int*));

//Para la condición de las 10 bicicletas en un mismo sentido
int *cantidadMaximaBicicletas = (int*)crearMemoriaCompartida(sizeof (int*));



int main(){

   cargarArchivotxt(ubicacion);
   //Se carga el archivo txt en la estructura Bicicleta

   inicializarVariablesGlobales();

   printf("EPALEEEEEEEEE \n\n");

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
      borrarSemaforo(mutex, "Mutex");
      borrarSemaforo(semA, "SemA");
      borrarSemaforo(semB, "SemB");
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
         up(mutex);
         break;
      }

      strcpy(horaDerecha,aux->hora);

      elegirCamino(aux,derecha);


      imprimirBici(aux);
      strcpy(horaSendero,aux->hora);

      contarBicicleta(derecha);

      if(*cantidadMaximaBicicletas == 0){

         if(aux->sentido == izquierda)
            strcpy(horaIzquierda,aux->hora);
         else
            strcpy(horaDerecha,aux->hora); 

      } 

      *sentidoSendero = derecha;
      posD++;

   }

   //Ha "fallecido" terminado el proceso
   *estadoProcesoHermano = off;

   printf("HE SALIDO DE BISRIGTH \n"); 

}


void bisLeft(){


   int posI = 0;
   Bicicleta* aux;

   //sumarDOSunidades();

   while(1 == 1){

      aux = obtenerBici(izquierda,&posI);

      if(aux == NULL){
         up(mutex);
         break;
      }

      //printf("CUANTAS \n");

      strcpy(horaIzquierda,aux->hora);

      elegirCamino(aux,izquierda);


      imprimirBici(aux);
      strcpy(horaSendero,aux->hora);
      
      contarBicicleta(aux->sentido);

      if(*cantidadMaximaBicicletas == 0){

         if(aux->sentido == izquierda)
            strcpy(horaIzquierda,aux->hora);
         else
            strcpy(horaDerecha,aux->hora); 

      }     


      *sentidoSendero = izquierda;
      posI++;

   }

   //Ha "fallecido" terminado el proceso
   *estadoProcesoHermano = off;

   printf("HE SALIDO DE BISLEFT \n");

}


void elegirCamino(Bicicleta *bici, int direccion){

   int repetir = off;

   down(mutex);

   if(*cantidadMaximaBicicletas == 0){

      if(direccion == *sentidoSendero)
         repetir = on;

      else{
         sumarDOSunidades();

         *cantidadMaximaBicicletas = 10;
 
         if(direccion == izquierda)
            strcpy(horaIzquierda,bici->hora);
         else
            strcpy(horaDerecha,bici->hora);

      }
      
   }

   else{

      if(direccion == izquierda)
         strcpy(bici->hora,horaIzquierda);
      else
         strcpy(bici->hora,horaDerecha);

   }


   if(*estadoProcesoHermano == on){
      
      int respuesta = strcmp(horaIzquierda,horaDerecha);
      //printf("HoraI: %s  HoraD: %s Respuesta: %i \n", horaIzquierda, horaDerecha,respuesta);

      if( (respuesta < 0)&&(direccion == derecha) )
         repetir = on;
      else
         if( (respuesta > 0)&&(direccion == izquierda) )
            repetir = on;
         else
            if(respuesta == 0){

               //La segunda parte de la condición solo funciona cuando NO ha cruzado ninguna bicicleta
               if( (bici->sentido == *sentidoSendero)||(*sentidoSendero == -1) ){

                  //("HORAS: %s %s \n",horaIzquierda,horaDerecha);

                  char *horaAumentada = sumarUNAunidad(bici->hora);
                  //bici->hora = sumarUNAunidad(bici->hora);
                  //repetir = on;

                  if(direccion == izquierda)
                     strcpy(horaDerecha,horaAumentada);
                  else
                     strcpy(horaIzquierda,horaAumentada);

               }
               else
                  repetir = on;

            }

   }

   //Mi hora es menor o igual a la última hora en el sendero
   if( (repetir == off)&&(strcmp(bici->hora,horaSendero) <= 0) ){
      bici->hora = sumarUNAunidad(bici->hora);

      if(direccion == derecha)
         strcpy(horaDerecha,bici->hora);
      else
         strcpy(horaIzquierda,bici->hora);

      repetir = on;
   }


   up(mutex);

   if(repetir == on)
      elegirCamino(bici, direccion); 

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

   strcpy(horaDerecha,"");
   strcpy(horaIzquierda,"");

   *cantidadMaximaBicicletas = 10;

}



char* sumarUNAunidad(char *horaBici){

   int horaInt, minutoInt, size = 15;
   char *horaChar, *minutoChar;

   //Extrae las horas y minutos de la hora total de la bici
   horaChar = subString(horaBici,0,1);
   minutoChar = subString(horaBici,3,4);

   //Convierte los char en int
   horaInt = atoi (horaChar);
   minutoInt = atoi (minutoChar);

   minutoInt++;
   if(minutoInt == 60){
      minutoInt = 0;
      horaInt++;
   }

   //Convierte los int en char (Ya aumentados en UNA unidad)
   snprintf(horaChar,size,"%d",horaInt);
   snprintf(minutoChar,size,"%d",minutoInt);

   //Concatena la hora y minuto
   strcat(horaChar,":");
   strcat(horaChar,minutoChar);

   //Retorna la hora Total
   return horaChar;

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



void contarBicicleta(int sentidoActual){

   if(*sentidoSendero != sentidoActual)
      *cantidadMaximaBicicletas = 10;

   (*cantidadMaximaBicicletas)--;

   if(*cantidadMaximaBicicletas == 0){
      printf("ESPERANDO QUE SALGAN LAS 10 BICICLETAS!!! \n ");
      sumarDOSunidades();
   }

}


void sumarDOSunidades(){

   Bicicleta *aux = b;

   while(aux != NULL){

      aux->hora = sumarUNAunidad(aux->hora);
      aux->hora = sumarUNAunidad(aux->hora);

      aux = aux->prox;

   }

}







