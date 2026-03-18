import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.concurrent.ThreadLocalRandom;

public class Program {

  public static void main(String args[]) {
    ThreadPool pool = new ThreadPool(4);
    for (int i = 0; i < 20; i++) {
      Runnable task = new TestTask(i);
      pool.setTask(task);
    }
    pool.join();
  }
}
class TestTask implements Runnable {
  private static DateFormat dateFormat = new SimpleDateFormat("HH:mm:ss");
  private static String getTime() {
    return dateFormat.format(Calendar.getInstance().getTime());
  }

  private int id;

  public TestTask(int id) { this.id = id; }

  public void run() {
    int sec = ThreadLocalRandom.current().nextInt(5, 10);
    System.out.printf("%s [%d]: Task executing for %d seconds\n", getTime(), id,
                      sec);
    try {
      Thread.sleep(sec * 1000);
    } catch (InterruptedException e) {
      System.out.println("TASK INTERRUPTED!!!");
    }
    System.out.printf("%s [%d]: Task done\n", getTime(), id);
  }
}
