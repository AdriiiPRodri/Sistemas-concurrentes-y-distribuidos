// *****************************************************************************
//
// Prácticas de SCD. Práctica 1.
// Plantilla de código para el ejercicio del productor-consumidor con
// buffer intermedio.
//
// *****************************************************************************

#include <iostream>
#include <cassert>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h> // necesario para {\ttbf usleep()}
#include <stdlib.h> // necesario para {\ttbf random()}, {\ttbf srandom()}
#include <time.h>   // necesario para {\ttbf time()}

using namespace std ;

// ---------------------------------------------------------------------
// constantes configurables:

sem_t
  puede_escribir,
  puede_leer;

const unsigned
  num_items  = 40 ,    // numero total de items que se producen o consumen
  tam_vector = 10 ;    // tamaño del vector, debe ser menor que el número de items

int buffer[tam_vector];
int primera_libre = 0;
int primera_ocupada = 0;

// ---------------------------------------------------------------------
// introduce un retraso aleatorio de duración comprendida entre
// 'smin' y 'smax' (dados en segundos)

void retraso_aleatorio( const float smin, const float smax )
{
  static bool primera = true ;
  if ( primera )        // si es la primera vez:
  {  srand(time(NULL)); //   inicializar la semilla del generador
     primera = false ;  //   no repetir la inicialización
  }
  // calcular un número de segundos aleatorio, entre {\ttbf smin} y {\ttbf smax}
  const float tsec = smin+(smax-smin)*((float)random()/(float)RAND_MAX);
  // dormir la hebra (los segundos se pasan a microsegundos, multiplicándos por 1 millón)
  usleep( (useconds_t) (tsec*1000000.0)  );
}

// ---------------------------------------------------------------------
// función que simula la producción de un dato

unsigned producir_dato()
{
  static int contador = 0 ;
  contador = contador + 1 ;
  retraso_aleatorio( 0.2, 0.5 );
  cout << "Productor : dato producido: " << contador << endl << flush ;
  return contador ;
}
// ---------------------------------------------------------------------
// función que simula la consumición de un dato

void consumir_dato( int dato )
{
   retraso_aleatorio( 0.2, 0.5 );
   cout << "Consumidor:                              dato consumido: " << dato << endl << flush ;
}
// ---------------------------------------------------------------------
// función que ejecuta la hebra del productor

void * funcion_productor( void * )
{
  for( unsigned i = 0 ; i < num_items ; i++ )
  {
    sem_wait(&puede_escribir);        // En un principio este semáforo tiene valor 10 ya que este puede llenar el buffer
    int dato = producir_dato() ;
    buffer[primera_libre] = dato;
    primera_libre = (primera_libre + 1) % tam_vector;
    sem_post(&puede_leer);          // Esto va aumentando en 1 el semáforo del consumidor para asi poder consumir todos los valores del buffer

    cout << "Productor : dato insertado: " << dato << endl << flush ;
  }
  return NULL ;
}
// ---------------------------------------------------------------------
// función que ejecuta la hebra del consumidor

void * funcion_consumidor( void * )
{
  for( unsigned i = 0 ; i < num_items ; i++ )
  {
    // falta aquí: leer "dato" desde el vector intermedio

    sem_wait(&puede_leer);
    int dato = buffer[primera_ocupada];
    primera_ocupada = (primera_ocupada + 1) % tam_vector;
    cout << "Consumidor:                              dato extraído : " << dato << endl << flush ;
    consumir_dato( dato ) ;
    sem_post(&puede_escribir);
  }
  return NULL ;
}
//----------------------------------------------------------------------

int main()
{

  // falta: crear y poner en marcha las hebras, esperar que terminen
  pthread_t hebra_productora, hebra_consumidora ;

   sem_init( &puede_escribir, 0, 10 ); // inicialmente se puede escribir
   sem_init( &puede_leer,     0, 0 ); // inicialmente no se puede leer

   pthread_create( &hebra_productora, NULL, funcion_productor, NULL );
   pthread_create( &hebra_consumidora, NULL, funcion_consumidor, NULL );

   pthread_join( hebra_productora, NULL ) ;
   pthread_join( hebra_consumidora,   NULL ) ;

   sem_destroy( &puede_escribir );
   sem_destroy( &puede_leer );

   return 0 ;
}
