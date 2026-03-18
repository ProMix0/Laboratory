public class PooledThread {
  private Thread thread;
  private PooledThreadLoop loopObject;
  public boolean isRunning() { return loopObject.isRunning; }

  public PooledThread(ThreadPool pool) {
    loopObject = new PooledThreadLoop(pool);
    thread = new Thread(loopObject);
    thread.start();
  }

  public void setTask(Runnable task) {
    loopObject.task = task;
    loopObject.isRunning = true;
    //      System.err.println("Interrupting");
    thread.interrupt();
  }

  public void join() {
    loopObject.stop = true;
    if (!isRunning()) {
      thread.interrupt();
    }
    try {
      thread.join();
    } catch (InterruptedException e) {
      System.err.println("Join Interrupted");
    }
  }

  private class PooledThreadLoop implements Runnable {
    private ThreadPool pool;
    private Runnable task;
    public boolean isRunning;
    public boolean stop;

    public PooledThreadLoop(ThreadPool pool) {
      this.pool = pool;
      task = null;
      stop = false;
      isRunning = false;
    }

    public void run() {
      while (!stop) {
        try {
          Thread.sleep(10000);
        } catch (InterruptedException e) {
          //          System.err.println("Thread Interrupted");
          if (isRunning) {
            task.run();
            task = null;
            isRunning = false;
          }
        }
      }
    }
  }
}
