public class ThreadPool {
  private PooledThread threads[];

  private int poolSize;

  public ThreadPool(int size) {
  poolSize = size;
    threads = new PooledThread[poolSize];
    for (int i = 0; i < poolSize; i++) {
      threads[i] = new PooledThread(this);
    }
  }

  public void setTask(Runnable task) {
    while (true) {
      for (int i = 0; i < poolSize; i++) {
        if (!threads[i].isRunning()) {
          threads[i].setTask(task);
          return;
        }
      }
      // System.err.println("No available runner, looping");
      try {
        Thread.sleep(10);
      } catch (InterruptedException e) {
      }
    }
  }

  public void join() {
    while (true) {
      int i;
      for (i = 0; i < poolSize; i++) {
        threads[i].join();
      }
      if (i == poolSize) {
        return;
      }
    }
  }
}
