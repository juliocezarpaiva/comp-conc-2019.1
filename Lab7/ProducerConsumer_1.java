// Java program to implement solution of producer 
// consumer problem. 
import java.util.LinkedList;

import static java.lang.Math.sqrt;

public class ProducerConsumer
{
    public static void main(String[] args)
            throws InterruptedException
    {
        // Object of a class that has both produce()
        // and consume() methods
        final PC pc = new PC();

        // Create producer thread
        Thread t1 = new Thread(new Runnable()
        {
            @Override
            public void run()
            {
                try
                {
                    pc.produce();
                }
                catch(InterruptedException e)
                {
                    e.printStackTrace();
                }
            }
        });

        // Create consumer thread
        Thread t2 = new Thread(new Runnable()
        {
            @Override
            public void run()
            {
                try
                {
                    pc.consume();
                }
                catch(InterruptedException e)
                {
                    e.printStackTrace();
                }
            }
        });

        // Start both threads
        t1.start();
        t2.start();

        // t1 finishes before t2
        t1.join();
        t2.join();
    }

    // This class has a list, producer (adds items to list
    // and consumber (removes items).
    public static class PC
    {
        // Create a list shared by producer and consumer
        // Size of list is 2.
        LinkedList<Integer> list = new LinkedList<>();
        int capacity = 5;

        // Function called by producer thread
        public void produce() throws InterruptedException
        {
            int fibovector[] = {1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377, 610, 987, 1597, 2584, 4181, 6765, 10946, 17711, 28657, 46368, 75025};
            int value = 0, index = 0;
            while (true)
            {
                synchronized (this)
                {
                    // producer thread waits while list
                    // is full
                    while (list.size()==capacity)
                        wait();

                    System.out.println("Producer produced-"
                            + fibovector[index]);

                    // to insert the jobs in the list
                    list.add(fibovector[index]); index++;

                    // notifies the consumer thread that
                    // now it can start consuming
                    notifyAll();
                }
            }
        }

        // Function called by consumer thread
        public void consume() throws InterruptedException
        {
            while (true)
            {
                synchronized (this)
                {
                    // consumer thread waits while list
                    // is empty
                    while (list.size()==0)
                        wait();

                    //to retrive the ifrst job in the list
                    int val = list.removeFirst();
                    if ( ehPrimo(val) ) System.out.println("-------------------------" + val + " eh primo.");

                    System.out.println("-------------------------Consumer consumed-"
                            + val);

                    // Wake up producer thread
                    notifyAll();
                }
            }
        }

        public boolean ehPrimo(int n)
        {
            int i;
            if ( n <= 1 ) return false;
            if ( n == 2 ) return true;
            if ( n%2 == 0) return false;
            for ( i=3; i< sqrt(n)+1; i+=2 )
            {
                if ( n%i == 0 ) return false;
            }
            return true;
        }
    }

} 
