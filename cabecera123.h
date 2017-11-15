#include <unistd.h>



struct intervalo{ //struct intervalo de los numeros a leer por los procesos
  int desde;
  int hasta;
  int numero;
};

int *numeros_archivo; //vector de ambito global para los numeros del archivo
int numero_primos; // para el problema 2




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
    exit (-1);
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
    exit(-1);
  }
  else{


    for (int i = 0; i<lineas; i++){
 	fscanf(archivo,"%d",&numeros[i]);
    }

	    fclose(archivo);
  }
}




void funpro (struct intervalo inter){ // funcion que ejecutan los procesos

   printf("SOY EL PROCESO %i de id %i ", inter.numero ,getpid());
   printf("CREO CON: %i %i\n", inter.desde,inter.hasta);

   FILE *archivo;

     char subi = (char)(inter.numero)+'0';

     char stor[5] = ".txt"; 
     char str1[1];
     str1[0]= subi;

     if (inter.numero == 10){
     archivo = fopen ("10.txt", "w+");}
     else{
     archivo = fopen (str1, "w+");
     }

 
     for (int i=(inter.desde)-1; i<(inter.hasta); i++){
           if (esprimo(numeros_archivo[i])){
            fprintf(archivo, "%i %i\n", numeros_archivo[i], 1);
          }else{
            fprintf(archivo, "%i %i\n", numeros_archivo[i], 0);
          }   
     }
}


void catch_signal_ctrlC(int s) //señal
{
  printf("\nI'm sorry dave I'm afraid I can't do that\n");
  exit(0);
}


void mostrarerror(){
    printf ("\n ERROR. La ejecucion debe tener el siguiente formato:\n\n");
    printf (" ./primos entrada.txt [-p | -t] [-n N] donde:\n");
    printf ("\n * entrada.txt son los numeros a procesar (debe existir)\n");
    printf ("\n * p o t indica si los numeros van a ser procesados por hilos y procesos\n");
    printf ("\n * n es la cantidad de hilos/procesos a crear\n");
    printf ("\n * n debe estar entre 1 y 10\n\n\n");
}


void mostrarerror2(){
    printf ("\n ERROR. La ejecucion debe tener el siguiente formato:\n\n");
    printf (" ./primosn entrada.txt [-n N] donde:\n");
    printf ("\n * entrada.txt son los numeros a procesar (debe existir)\n");
    printf ("\n * n es la cantidad de hilos a crear\n");
    printf ("\n * n debe estar entre 1 y 10\n\n\n");
}


void mostrarerror3(){
    printf ("\n ERROR. La ejecucion debe tener el siguiente formato:\n\n");
    printf (" ./primoscatch entrada.txt [-n N] donde:\n");
    printf ("\n * entrada.txt son los numeros a procesar (debe existir)\n");
    printf ("\n * n es la cantidad de procesos a crear\n");
    printf ("\n * n debe estar entre 1 y 10\n\n\n");
}


void imprimir (int *vector, int tamano){
 for (int i=0; i<tamano; i++){
  printf ("%i",vector[i]);
 }
}