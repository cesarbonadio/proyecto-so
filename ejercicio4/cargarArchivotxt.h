// ------- Librerías --------

#include <string.h> 


// ------ Variables globales --------

struct Bicicleta{
   char *hora;
   int sentido;
   Bicicleta *prox;
};

Bicicleta *b = NULL;

const int maximoCaracteres = 20;
const int derecha = 0;
const int izquierda = 1;


// ------- Cabecera de funciones --------

Bicicleta* cargarArchivotxt(const char *ubicacion);
Bicicleta* agregarBicicleta(Bicicleta *b, char *bici);

void mostrarLista(Bicicleta *b);
char* subString(char *cadenaFuente, int desde, int hasta);
int obtenerSentido(char *cadenaFuente);


// ------- Implementación --------

Bicicleta* cargarArchivotxt(const char *ubicacion){

   char *bici = new char;
   /*Se tiene que inicializar debido a que fgets debe recibir un puntero char inicializado,
de lo contrario, sale la alerta "Violación de segmento (`core' generado)" */

   FILE *archivo;
   archivo = fopen (ubicacion, "r");

   if (archivo == NULL){
      printf("ERROR en la apertura del archivo \n\n");
      exit(EXIT_FAILURE);
   }
   else{

      while(!feof(archivo)){

         fgets(bici,maximoCaracteres,archivo);
         b = agregarBicicleta(b,bici);

      }

      fclose(archivo);

      printf("\n  SALIDA");
      mostrarLista(b);
   }

   return b;

}



//NOTA: Se debe enviar el puntero b como NULL
Bicicleta* agregarBicicleta(Bicicleta *b, char *bici){

   if(b != NULL){

      //Se agrega una Nueva Bicicleta
      if(b->prox == NULL){

         Bicicleta *aux = new Bicicleta;
         aux->hora = subString(bici,0,4);
         aux->sentido = obtenerSentido(bici);

         b->prox = aux;

      }
      else
         agregarBicicleta(b->prox, bici);
   }
   //Primera Bicicleta
   else{

      b = new Bicicleta;
      b->hora = subString(bici,0,4);
      b->sentido = obtenerSentido(bici);
   }

   return b;


}



void mostrarLista(Bicicleta *b){

   if(b != NULL){
      printf("\n %s  %i",b->hora,b->sentido);
      mostrarLista(b->prox);
   }

}


char* subString(char *cadenaFuente, int desde, int hasta){

   int pos = 0;
   char *cadenaDestino = new char;


   for(int i = desde; i <= hasta; i++){

      cadenaDestino[pos] = cadenaFuente[i];
      pos++;

   }

   cadenaDestino[pos] = '\0';

   return cadenaDestino;

}



int obtenerSentido(char *cadenaFuente){

   char *sentidoChar;
   sentidoChar = subString(cadenaFuente,6,6);

   int sentidoInt;

   if(strcmp(sentidoChar,"L") == 0)
      sentidoInt = izquierda;
   else
      sentidoInt = derecha;

   return sentidoInt;

}





