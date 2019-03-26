//
// NonDeteminism.java
//
public class NonDeterminism {
    public static void main(String[] args) throws InterruptedException {
        class Container {
            public String value = "Empty";
        }
        final Container container = new Container();

        class FastThread extends Thread {
            public void run() {
                container.value = "Fast";
            }
        }

        class SlowThread extends Thread {
            public void run() {
                while(!container.value.equals("Fast")) {
                    try {
                        Thread.sleep(50);
                    }
                    catch(Exception e) {}
                }
                container.value = "Slow";
            }
        }
        
        FastThread fast = new FastThread();
        SlowThread slow = new SlowThread();
        fast.start(); slow.start();
        fast.join(); slow.join();
        System.out.println(container.value);
    }
}
