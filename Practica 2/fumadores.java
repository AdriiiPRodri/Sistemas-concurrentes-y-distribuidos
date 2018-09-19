import monitor.*;

class Estanco extends AbstractMonitor{
  private int ingrediente        = -1;
  private Condition[] fuma       = new Condition[]{makeCondition(), makeCondition(), makeCondition()};
  private Condition fabrica      = makeCondition();

  // invocado por cada fumador, indicando su ingrediente o numero
  public void obtenerIngrediente( int miIngrediente ){
    enter();
    if(miIngrediente != ingrediente){
      System.out.printf("ESPERA número %d\n", miIngrediente);
      fuma[miIngrediente].await();
    }
    ingrediente = -1;
    fabrica.signal();
    leave();
  }
  // invocado por el estanquero, indicando el ingrediente que pone
  public void ponerIngrediente( int ingrediente ){
    enter();
    this.ingrediente = ingrediente;
    fuma[ingrediente].signal();
    leave();
  }
  // invocado por el estanquero
  public void esperarRecogidaIngrediente(){
    enter();
    if(ingrediente != -1)
      fabrica.await();
    leave();
  }
}

class Estanquero implements Runnable{
  private Estanco estanco;
  public Thread thr ;

  public Estanquero(Estanco est){
    estanco = est;
    thr = new Thread(this,"Estanquero");
  }

  public void run(){
    int ingrediente ;
    while (true){
      ingrediente = (int) (Math.random () * 3.0); // 0,1 o 2
      estanco.ponerIngrediente( ingrediente );
      estanco.esperarRecogidaIngrediente() ;
    }
  }
}

class Fumador implements Runnable{
  private int miIngrediente;
  private Estanco estanco;
  public Thread thr ;

  public Fumador( int p_miIngrediente, Estanco est){
    estanco = est;
    miIngrediente = p_miIngrediente;
    thr = new Thread(this,"Fumador"+miIngrediente);
  }

  public void run(){
    while ( true ){
      estanco.obtenerIngrediente( miIngrediente );
      System.out.printf("Comienza a fumar %d\n", miIngrediente);
      aux.dormir_max( 2000 );
      System.out.printf("Termina de fumar %d\n", miIngrediente);
    }
  }
}

class MainEstanco{
  public static void main(String[] args){
    Fumador[]  fumador       = new Fumador[3];
    Estanco      estanco     = new Estanco();
    Estanquero   estanquero  = new Estanquero(estanco);

    // crear hebras de los fumadores
    for(int i = 0; i < 3; i++)
      fumador[i] = new Fumador(i, estanco) ;

    // poner en marcha las hebras
    for(int i = 0; i < 3; i++)
      fumador[i].thr.start();
    estanquero.thr.start();
  }
}
