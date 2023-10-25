/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Codigo: Leitores e escritores usando  Java */
/* -------------------------------------------------------------------*/

/*
--------------------MUDANÇAS--------------------

Matheus:

Adicionei uma variável para contabilizar quantos escritores
estão esperando para escrever. Caso esse valor seja diferente
de 0, então nenhum leitor pode iniciar uma leitura mesmo que não
haja escritores escrevendo no momento.

Essa condição foi incorporada no "lab6_verificaLE.py" também.
Os incrementos e decrementos dessa variável são feitos dentro
da função "EntraEscrita" e a condição para o leitor verificá-la
está em "EntraLeitor".

Adicional a isso criei uma variável "linha" só para marcar a linha em
que a próxima adição de texto do log será posta. Essa variável é
incrementada sempre que uma mensagem é escrita no log.

------------------------------------------------
*/

// Monitor
class LE {
  private int leit, escr;
  private int escrEsperando, linha;
  
  // Construtor
  LE() { 
     this.leit = 0;
     this.escr = 0;
     //Matheus:
     //Adicionei essas duas novas variáveis
     //Uma checa se um escritor está esperando, outra guarda a linha
     //que se está no log
     this.escrEsperando = 0;
     this.linha = 3;
  } 
  
  // Entrada para leitores
  public synchronized void EntraLeitor (int id) {
    try { 
      //Matheus:
      //leitores só leram caso não haja escritores escrevendo nem
      //nenhum escritor esperando para escrever
      while (this.escr > 0 || this.escrEsperando > 0) {
      //if (this.escr > 0) {
         System.out.println ("le.leitorBloqueado("+id+", "+this.linha+")");
         this.linha++;
         wait();  //bloqueia pela condicao logica da aplicacao 
      }
      this.leit++; 
      System.out.println ("le.leitorLendo("+id+", "+this.linha+")");
      this.linha++;
    } catch (InterruptedException e) { }
  }
  
  // Saida para leitores
  public synchronized void SaiLeitor (int id) {
     this.leit--;
     if (this.leit == 0) 
           this.notify(); //libera escritor (caso exista escritor bloqueado)
     System.out.println ("le.leitorSaindo("+id+", "+this.linha+")");
     this.linha++;
  }
  
  // Entrada para escritores
  public synchronized void EntraEscritor (int id) {
    try {
      //Matheus:
      //Caso um escritor se bloqueie, soma-se em 1
      //a variável escrEsperando para que se tenha sinalizado
      //que há (pelo menos) um escritor esperando para escrever
      while ((this.leit > 0) || (this.escr > 0)) {
         this.escrEsperando++;
         System.out.println ("le.escritorBloqueado("+id+", "+this.linha+")");
         this.linha++;
         wait();  //bloqueia pela condicao logica da aplicacao 
      }
      this.escr++;
      //Após ser desbloqueado, sinaliza que não está mais esperando
      this.escrEsperando--;
      System.out.println ("le.escritorEscrevendo("+id+", "+this.linha+")");
      this.linha++;
    } catch (InterruptedException e) { }
  }
  
  // Saida para escritores
  public synchronized void SaiEscritor (int id) {
     this.escr--;
     notifyAll(); //libera leitores e escritores (caso existam leitores ou escritores bloqueados)
     System.out.println ("le.escritorSaindo("+id+", "+this.linha+")");
     this.linha++;
  }
}

//--------------------------------------------------------
// Leitor
class Leitor extends Thread {
  int id; //identificador da thread
  LE monitor;//objeto monitor para coordenar a lógica de execução das threads

  // Construtor
  Leitor (int id, LE m) {
    this.id = id;
    this.monitor = m;
  }

  // Método executado pela thread
  public void run () {
      double j=777777777.7, i;
      for (;;) {
        this.monitor.EntraLeitor(this.id);
        for (i=0; i<100000000; i++) {j=j/2;} //...loop bobo para simbolizar o tempo de leitura
        this.monitor.SaiLeitor(this.id);
      }
  }
}

//--------------------------------------------------------
// Escritor
class Escritor extends Thread {
  int id; //identificador da thread
  LE monitor; //objeto monitor para coordenar a lógica de execução das threads

  // Construtor
  Escritor (int id, LE m) {
    this.id = id;
    this.monitor = m;
  }

  // Método executado pela thread
  public void run () {
      double j=777777777.7, i;
      for (;;) {
        this.monitor.EntraEscritor(this.id); 
        for (i=0; i<100000000; i++) {j=j/2;} //...loop bobo para simbolizar o tempo de escrita
        this.monitor.SaiEscritor(this.id); 
      }
  }
}

//--------------------------------------------------------
// Classe principal
class LeitorEscritor {
  static final int L = 4;
  static final int E = 3;

  public static void main (String[] args) {
    int i;
    LE monitor = new LE();            // Monitor (objeto compartilhado entre leitores e escritores)
    Leitor[] l = new Leitor[L];       // Threads leitores
    Escritor[] e = new Escritor[E];   // Threads escritores

    //inicia o log de saida
    System.out.println ("import lab6_verificaLE2");
    System.out.println ("le = lab6_verificaLE2.LE()");
    
    for (i=0; i<L; i++) {
       l[i] = new Leitor(i+1, monitor);
       l[i].start(); 
    }
    for (i=0; i<E; i++) {
       e[i] = new Escritor(i+1, monitor);
       e[i].start(); 
    }
  }
}
