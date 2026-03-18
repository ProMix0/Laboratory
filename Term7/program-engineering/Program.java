import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class Program {

  public static void main(String args[]) {
    Runnable task = new TestTask();
    ThreadPool pool = new ThreadPool();
    for (int i = 0; i < 30; i++) {
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

  public void run() {
    System.out.println("Hi from task " + getTime());
    try {
      Thread.sleep(5000);
    } catch (InterruptedException e) {
      System.out.println("TASK INTERRUPTED!!!");
    }
    System.out.println("Task done " + getTime());
  }
}
