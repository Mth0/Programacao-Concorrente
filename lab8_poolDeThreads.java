/*
--------------------IMPORTANTE--------------------

Reaproveitei bastante coisa do código do lab 8. Mudei a classe
que continha a função Hello, a main e comentei os métodos para entender melhor
o que estava acontecendo.

Esse programa exige dois argumentos:
   -O primeiro é referente ao número de threads que se deseja.
   -O segundo é referente até que número ocorrerá o teste de primalidade.

Testes: Criei um hashSet no calculoDePrimalidade para que cada primo
encontrado fosse adicionado nele. Esse hashSet é "populado" pela versão sequencial,
que executa anteriormente à concorrente. Após isso, a concorrente inicia e as threads
basicamente checam se os primos que encontram estão no hashSet contendo os primos.
Caso algum primo não presente no hashSet, uma mensagem é disparada indicando qual primo é
esse e o código se encerra.

Testei para 10 threads e 100000000 (100 milhões) como valor do segundo argumento e o teste de corretude
indicou que estava correto.

--------------------------------------------------
*/

import java.util.LinkedList;
import java.lang.Math;
import java.util.Set;
import java.util.HashSet;

//-------------------------------------------------------------------------------
//Classe que define um pool de threads 
//!!! Documentar essa classe !!!

   /*
   Matheus:
   Ministra as threads no pool. Contém:
   numero de threads do pool;
   as próprias threads do pool;
   lista de funções ou tarefas pendentes;
   um booleano para indicar que acabou todos os trabalhos
   */
class FilaTarefas {
    private final int nThreads;
    private final MyPoolThreads[] threads;
    private final LinkedList<Runnable> queue;
    private boolean shutdown;

    //inicializa o objeto da classe
    public FilaTarefas(int nThreads) {
        this.shutdown = false;
        this.nThreads = nThreads;
        queue = new LinkedList<Runnable>();
        threads = new MyPoolThreads[nThreads];
        for (int i=0; i<nThreads; i++) {
            threads[i] = new MyPoolThreads();
            threads[i].start();
        } 
    }

    /*
    Matheus:
    Adiciona a função/tarefa na lista de tarefas
    pendentes um objeto runnable.
    Para isso, primeiro checa se o shutdown já está ativo
    e, caso não esteja, faz a adição e notifica as threads do pool

    detalhe importante para o synchronized: ele é feito na lista de tarefas
    de modo que, na hora da adição de uma nova tarefa, nenhuma thread possa
    mexer nesta lista. Isso se repete em outros métodos dessa classe.
    */
    public void execute(Runnable r) {
        synchronized(queue) {
            if (this.shutdown) return;
            queue.addLast(r);
            queue.notify();
        }
    }
    
    /*
    Matheus:
    Função que ativa o shutdown. Isso faz com que novas tarefas
    não possam ser adicionadas na lista de tarefas pendentes (queue).

    Após isso, aguarda todas as threads terminarem suas tarefas e esvaziarem
    a lista de tarefas. Isso se ilustra melhor na classe de MyPoolThreads.
    */
    public void shutdown() {
        synchronized(queue) {
            this.shutdown=true;
            queue.notifyAll();
        }
        for (int i=0; i<nThreads; i++) {
          try { threads[i].join(); } catch (InterruptedException e) { return; }
        }
    }

    private class MyPoolThreads extends Thread {
        /*
        Matheus:
        Por herdar Thread, é necessário que um método
        run seja implementado.

        Se inicia um loop "infinito" que só termina quando
        a lista queue estiver vazia e o shutdown já tiver
        sido acionado. Caso contrário, aguarda (é bloqueada até)
        o momento em que ninguém estava mexendo na queue para retirar
        uma tarefa da lista, caso haja alguma.
        */
       public void run() {
         Runnable r;
         while (true) {
           synchronized(queue) {
             while (queue.isEmpty() && (!shutdown)) {
               try { queue.wait(); }
               catch (InterruptedException ignored){}
             }
             if (queue.isEmpty() && shutdown) return;  
             r = (Runnable) queue.removeFirst();
           }
           try { r.run(); }
           catch (RuntimeException e) {}
         } 
       } 
    } 
}
//-------------------------------------------------------------------------------

//--PASSO 1: cria uma classe que implementa a interface Runnable 
class CalculaPrimalidade implements Runnable {
   private long n;
   private static Set<Long> primosEncontrados;

   public CalculaPrimalidade(long n) {
      this.n = n;
      if(primosEncontrados == null){
         primosEncontrados = new HashSet<Long>();
      }
   }

   //--metodo executado pela thread
   /*
   Matheus:
   Se n for primo segundo uma thread, é verificado se ele está
   no hashSet de primos.
   Caso não esteja, uma mensagem é disparada e o programa encerrado.
   Isso indica que o resultado concorrente não está correto.
   */
   public void run() {
      if(ehPrimo(this.n)){
         if(!checaSet(this.n)){
            System.out.println(this.n + " determinado como primo, mas não é!");
            System.exit(0);
         }
      }
   }

   /*
   Matheus:
   Criei essa função aqui ao invés de executar diretamente
   no run para ficar mais fácil de checar a corretude posteriormente.
   */
   public static Boolean ehPrimo(long n) {
      if (n <= 1) return false;
      if (n == 2) return true;
      if (n % 2 == 0) return false;

      long i;

      for (i = 3; i < Math.sqrt((double)n) + 1; i += 2){
         if(n % i == 0) return false;
      }
      return true;
   }

   /*
   Matheus:
   Na implementação desse código em específico,
   O syncronized deste método é descartável, já que só a parte da
   execução sequencial de fato adiciona elementos ao hashSet e executa antes
   do pool de threads ser inicializado. Mas achei que seria uma "boa prática"
   deixá-lo aqui.
   */
   public static Boolean achou(long n){
      synchronized(primosEncontrados){
         return primosEncontrados.add(n);
      }
   }

   /*
   Matheus:
   Argumento semelhante à função "achou". Como não há adições no decorrer
   da execução concorrente, a leitura não sofrerá problemas. Porém decidi
   manter o synchronized por "boas práticas".
   */
   public static Boolean checaSet(long n){
      synchronized(primosEncontrados){
         return primosEncontrados.contains(n);
      }
   }

   /*
   Matheus:
   Esse método existe somente devido ao modo que faço o teste de corretude.
   */
   public static void initSet(){
      if(primosEncontrados == null){
         primosEncontrados = new HashSet<Long>();
      }
   }

}

//Classe da aplicação (método main)
class lab8_poolDeThreads {
    private static int NTHREADS;
    private static long N;

    public static void main (String[] args) {

      //Matheus: trata os argumentos vindos da entrada padrão
      if(args.length != 2){
         System.out.println("Número de argumentos inválido! Além do arquivo Lab8.java passe <número de threads no pool> <tamanho do for para testar primalidade>");
         System.exit(0);
      }
      else{
         NTHREADS = Integer.parseInt(args[0]);
         N = Long.parseLong(args[1]);
      }



      //Matheus: teste sequencial.
      CalculaPrimalidade.initSet();
      for (long i = 0; i < N; i++) {
         /*
         Matheus:
         Cada primo encontrado é adicionado ao HashSet primosEncontrados.
         */
         if(CalculaPrimalidade.ehPrimo(i)){
            CalculaPrimalidade.achou(i);
         }
      }


      //--PASSO 2: cria o pool de threads
      FilaTarefas pool = new FilaTarefas(NTHREADS); 
      
      //--PASSO 3: dispara a execução dos objetos runnable usando o pool de threads
      for (long i = 0; i < N; i++) {
      /*
      Matheus:
      O teste de corretude é feito durante a execução do concorrente. Caso algum primo
      seja encontrado por uma thread e *não* esteja no hashSet "populado" pela versão
      sequencial, então uma mensagem é disparada e o código encerrado.
      */
        Runnable r = new CalculaPrimalidade(i);
        pool.execute(r);
      }

      //--PASSO 4: esperar pelo termino das threads
      pool.shutdown();
      System.out.println("Terminou");


      //Matheus: Só imprime essa mensagem se o teste bate para todos os primos.
      System.out.println("\nOs resultados da versão concorrente e sequencial batem!");
   }
}
