import monitor.*;

class Barberia extends AbstractMonitor{
  private Condition pelando = makeCondition();
  private Condition sala_espera = makeCondition();
  private Condition barbero = makeCondition();

  // invcado por los clientes para cortarse el pelo
  public void cortarPelo (){
    enter();
    if(!pelando.isEmpty())
      sala_espera.await();
    barbero.signal();
    pelando.await();
    leave();
  }

  // invocado por el barbero para esperar (si procede) a un nuevo cliente y sentarlo para el corte
  public void siguienteCliente (){
    enter();
    if(sala_espera.isEmpty())
      barbero.await();
    sala_espera.signal();
    leave();
  }

  // invocado por el barbero para indicar que ha terminado de cortar el pelo
  public void finCliente (){
    enter();
    pelando.signal();
    leave();
  }
}

class Cliente implements Runnable{
  public Thread thr ;
  private int numero;
  private Barberia barberia;

  public Cliente(Barberia barb, int numero){
    barberia = barb;
    this.numero = numero;
    thr = new Thread(this,"Cliente"+numero);
  }
  public void run (){
    while (true) {
      barberia.cortarPelo (); // el cliente espera (si procede) y se corta el pelo
      System.out.println("\t\t\t\t\tSALE CLIENTE  "+numero);
      aux.dormir_max( 2000 ); // el cliente está fuera de la barberia un tiempo
    }
  }
}

class Barbero implements Runnable{
  public Thread thr ;
  private Barberia barberia;

  public Barbero(Barberia barb){
    barberia = barb;
    thr = new Thread(this);
  }
  public void run (){
    while (true) {
      barberia.siguienteCliente ();
      System.out.println("COMIENZA A PELAR");
      aux.dormir_max( 2500 ); // el barbero está cortando el pelo
      System.out.println("TERMINA DE PELAR");
      barberia.finCliente ();
    }
  }
}

class MainBarbero{
  public static void main(String[] args){
    int          n_clientes  = 5;
    Cliente[]   clientes    = new Cliente[n_clientes];
    Barberia     barberia    = new Barberia();
    Barbero      barbero     = new Barbero(barberia);

    // crear hebras de los clientes
    for(int i = 0; i < n_clientes; i++)
      clientes[i] = new Cliente(barberia, i) ;

    // poner en marcha las hebras
    for(int i = 0; i < n_clientes; i++)
      clientes[i].thr.start();
    barbero.thr.start();
  }
}
