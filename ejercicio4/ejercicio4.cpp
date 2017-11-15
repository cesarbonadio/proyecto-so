//---------- Librerías Propias -----------

#include "semaforo.h"
#include "cargarArchivotxt.h"


//---------- Librerías de C -----------


#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>


//---------- Cabecera de funciones -----------

void bisRigth();
void bisLeft();

void elegirCamino(Bicicleta *bici, int direccion);

void inicializarVariablesGlobales();
void* crearMemoriaCompartida(size_t size);

char* aumentarHora(char *horaBici, int aumento);
void contarBicicletaMismoSentido(int sentidoActual);

void guardarBicicleta(Bicicleta *aux);

// --------- Semáforo ---------

sem_t *mutex = inicializarSemaforo(1,"Mutex");

// -------- Varables "Normales" -----------

const char *ubicacion = "entrada2.txt";
const char *ubicacion2 = "salida.txt";

//Estados
int ON = 1;
int OFF = 0; 


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
      else
         sleep(3);

   }

   //Hijo_1
   else
      bisLeft();


   //El padre borra el semáforo una vez terminado el programa
   if(pid != 0)
      borrarSemaforo(mutex, "Mutex");
      
   printf("\n\n");
   return 0;

} 


void bisRigth(){

   int posD = 0;
   Bicicleta *aux;

   while(1 == 1){

      //Obtiene una bicicleta con sentido derecho
      aux = obtenerBicicletaConSentido(derecha,&posD);

      //En caso de que NO consiga una bicicleta
      if(aux == NULL){
         up(mutex);
         break;
      }

      strcpy(horaDerecha,aux->hora);

      //Decide que bicicleta cruza en sendero
      elegirCamino(aux,derecha);

      //Se escribe la bicicleta en el archivo de salida
      guardarBicicleta(aux);

      //Se actualiza la hora del sendero
      strcpy(horaSendero,aux->hora);

      //Se verifica la exclusión de las 10 bicicletas en el mismo sentido
      contarBicicletaMismoSentido(derecha);

      //Se actualiza la hora del sentido, en caso de haber ejecutado el caso anterior
      strcpy(horaDerecha,aux->hora);  
      *sentidoSendero = derecha;
      posD++;

   }

   //Ha "fallecido" terminado el proceso
   *estadoProcesoHermano = OFF;

   printf("HE SALIDO DE BISRIGTH \n"); 

}


void bisLeft(){

   int posI = 0;
   Bicicleta* aux;

   while(1 == 1){

      //Obtiene una bicicleta con sentido izquierdo
      aux = obtenerBicicletaConSentido(izquierda,&posI);

      //En caso de que NO consiga una bicicleta
      if(aux == NULL){
         up(mutex);
         break;
      }

      strcpy(horaIzquierda,aux->hora);

      //Decide que bicicleta cruza en sendero
      elegirCamino(aux,izquierda);

      //Se escribe la bicicleta en el archivo de salida
      guardarBicicleta(aux);

      //Se actualiza la hora del sendero
      strcpy(horaSendero,aux->hora);
      
      //Se verifica la exclusión de las 10 bicicletas en el mismo sentido
      contarBicicletaMismoSentido(izquierda);

      //Se actualiza la hora del sentido, en caso de haber ejecutado el caso anterior
      strcpy(horaIzquierda,aux->hora);
      *sentidoSendero = izquierda;
      posI++;

   }

   //Ha "fallecido" terminado el proceso
   *estadoProcesoHermano = OFF;

   printf("HE SALIDO DE BISLEFT \n");

}


void elegirCamino(Bicicleta *bici, int direccion){

   int repetir = OFF;

   down(mutex);

   if(direccion == izquierda)
      strcpy(bici->hora,horaIzquierda);
   else
      strcpy(bici->hora,horaDerecha);


   if(*estadoProcesoHermano == ON){
      
      int respuesta = strcmp(horaIzquierda,horaDerecha);

      //Hora Izquierda es MENOR a Hora Derecha
      if( (respuesta < 0)&&(direccion == derecha) )
         repetir = ON;
      else
         //Hora Izquierda es MAYOR a Hora Derecha
         if( (respuesta > 0)&&(direccion == izquierda) )
            repetir = ON;
         else
            //Hora Izquierda es IGUAL a Hora Derecha
            if(respuesta == 0){

               //La segunda parte de la condición solo funciona cuando NO ha cruzado ninguna bicicleta
               if( (bici->sentido == *sentidoSendero)||(*sentidoSendero == -1) ){

                  //Aumenta la hora de la bicicleta en UNA unidad
                  char *horaAumentada = aumentarHora(bici->hora,1);

                  //Actualiza la hora (Derecha e Izquierda)
                  if(direccion == izquierda)
                     strcpy(horaDerecha,horaAumentada);
                  else
                     strcpy(horaIzquierda,horaAumentada);

               }
               //Caso de que el otro sentido tiene MAYOR prioridad
               else
                  repetir = ON;

            }

   }//Fin de Proceso Hermano "vivo"

   //Mi hora es menor o igual a la última hora en el sendero
   if( (repetir == OFF)&&(strcmp(bici->hora,horaSendero) <= 0) ){
      bici->hora = aumentarHora(bici->hora,1);

      if(direccion == derecha)
         strcpy(horaDerecha,bici->hora);
      else
         strcpy(horaIzquierda,bici->hora);

      repetir = ON;
   }

   up(mutex);

   //En caso de que el proceso hermano tenga prioridad
   if(repetir == ON)
      elegirCamino(bici, direccion); 

}


void inicializarVariablesGlobales(){

   *estadoProcesoHermano = ON;

   *sentidoSendero = -1;
   strcpy(horaSendero,"");

   strcpy(horaDerecha,"");
   strcpy(horaIzquierda,"");

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
      printf("ESPERANDO QUE SALGAN LAS 10 BICICLETAS!!! \n");
      //Se aumenta la hora del sendero en 2 segundos, para que todas las bicicletas esperen
      strcpy( horaSendero,aumentarHora(horaSendero,2) );
      *cantidadMaximaBicicletas = 10;
   }

}



void guardarBicicleta(Bicicleta *aux){

   FILE *archivo;
   archivo = fopen (ubicacion2, "a");

   if (archivo == NULL){
      printf("ERROR en la apertura del archivo \n\n");
      exit(EXIT_FAILURE);
   }
   else{

      const char *horaCompleta;

      if(aux->sentido == izquierda)
         horaCompleta = strcat(aux->hora," L\n");
      else
         horaCompleta = strcat(aux->hora," R\n");

      fputs(horaCompleta,archivo);

      fclose(archivo);

   }

}







