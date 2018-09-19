// Con que uno cambie el orden ya no habrá interbloqueo
#include <iostream>
#include <time.h>      // incluye "time"
#include <unistd.h>    // incluye "usleep"
#include <stdlib.h>    // incluye "rand" y "srand"
#include <mpi.h>

using namespace std;

void Filosofo( int id, int nprocesos);
void Tenedor ( int id, int nprocesos);

// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
   int rank, size;

   srand(time(0));
   MPI_Init( &argc, &argv );
   MPI_Comm_rank( MPI_COMM_WORLD, &rank );
   MPI_Comm_size( MPI_COMM_WORLD, &size );

   if( size!=10)
   {
      if( rank == 0)
         cout << "El numero de procesos debe ser 10" << endl << flush ;
      MPI_Finalize( );
      return 0;
   }

   if ((rank%2) == 0)
      Filosofo(rank,size); // Los pares son Filosofos
   else
      Tenedor(rank,size);  // Los impares son Tenedores

   MPI_Finalize( );
   return 0;
}
// ---------------------------------------------------------------------

void Filosofo( int id, int nprocesos )
{
   int izq = (id+1) % nprocesos;
   int der = ((id+nprocesos)-1) % nprocesos;
   MPI_Status status;

   while(1)
   {
      if(id == 0){           // Uno de los Filosofos invierte el orden para así evitar la situación de interbloqueo
        // Solicita tenedor derecho
        cout <<"Filosofo "<< id << " coge tenedor der ..." << der << endl << flush;
        MPI_Ssend( &der, 1, MPI_INT, der, 0, MPI_COMM_WORLD );

        // Solicita tenedor izquierdo
        cout << "Filosofo "<< id << " solicita tenedor izq ..." << izq << endl << flush;
        MPI_Ssend( &izq, 1, MPI_INT, izq, 0, MPI_COMM_WORLD );
      }
      else{
        // Solicita tenedor izquierdo
        cout << "Filosofo "<< id << " coge tenedor izq ..." << izq << endl << flush;
        MPI_Ssend( &izq, 1, MPI_INT, izq, 0, MPI_COMM_WORLD );

        // Solicita tenedor derecho
        cout <<"\t\tFilosofo "<< id << " coge tenedor der ..." << der << endl << flush;
        MPI_Ssend( &der, 1, MPI_INT, der, 0, MPI_COMM_WORLD );
      }

      cout<<"\t\t\tFilosofo "<< id << " COMIENDO"<<endl<<flush;
      sleep((rand() % 3)+1);  //comiendo

      if(id == 0){
        // Suelta el tenedor derecho
        cout <<"Filosofo "<< id << " suelta tenedor der ..." << der << endl << flush;
        MPI_Ssend( &izq, 1, MPI_INT, der, 0, MPI_COMM_WORLD );

        // Suelta el tenedor izquierdo
        cout <<"\t\tFilosofo "<< id << " suelta tenedor izq ..." << izq << endl << flush;
        MPI_Ssend( &der, 1, MPI_INT, izq, 0, MPI_COMM_WORLD );
      }
      else{
        // Suelta el tenedor izquierdo
        cout <<"Filosofo "<< id << " suelta tenedor izq ..." << izq << endl << flush;
        MPI_Ssend( &izq, 1, MPI_INT, izq, 0, MPI_COMM_WORLD );

        // Suelta el tenedor derecho
        cout <<"\t\tFilosofo "<< id << " suelta tenedor der ..." << der << endl << flush;
        MPI_Ssend( &der, 1, MPI_INT, der, 0, MPI_COMM_WORLD );
      }

      // Piensa (espera bloqueada aleatorio del proceso)
      cout << "\t\t\tFilosofo " << id << " PENSANDO" << endl << flush;

      // espera bloqueado durante un intervalo de tiempo aleatorio
      // (entre una décima de segundo y un segundo)
      usleep( 1000U * (100U+(rand()%900U)) );
 }
}
// ---------------------------------------------------------------------

void Tenedor(int id, int nprocesos)
{
  int buf;
  MPI_Status status;
  int Filo;

  while( true )
  {
    // Espera un peticion desde cualquier filosofo vecino ...
    MPI_Probe( MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status );

    // Recibe la peticion del filosofo ...
    MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    Filo = status.MPI_SOURCE;

    cout << "Ten. " << id << " recibe petic. de " << Filo << endl << flush;

    // Espera a que el filosofo suelte el tenedor...
    MPI_Recv(&buf, 1, MPI_INT, Filo, 0, MPI_COMM_WORLD, &status);

    cout << "Ten. " << id << " recibe liberac. de " << Filo << endl << flush;
  }
}
// ---------------------------------------------------------------------
