import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

public class Server {
  private static final int PORT = 12345;
  private static final String STORAGE_DIR = "storage/server_storage";

  public static void main(String[] args) throws IOException {
    new File(STORAGE_DIR).mkdirs();

    System.out.println("Server listening on port " + PORT);

    while (true) {
      TcpOverUdp client = TcpOverUdp.accept(PORT);
      System.out.println("Client connected");

      handleClient(client);
    }
  }

  private static void handleClient(TcpOverUdp client) {
    try (client) {
      while (true) {
        int nameLength = client.readInt();
        byte[] nameBytes = new byte[nameLength];
        client.receive(nameBytes, nameBytes.length);

        String fileName = new String(nameBytes);

        long fileSize = client.readLong();
        File file = new File(STORAGE_DIR, fileName);

        try (FileOutputStream fileOut = new FileOutputStream(file)) {
          byte[] buffer = new byte[4096];
          long remaining = fileSize;
          while (remaining > 0) {
            int read = (int)Math.min(buffer.length, remaining);
            client.receive(buffer, read);
            fileOut.write(buffer, 0, read);
            remaining -= read;
          }
        }

        System.out.println("Received file from client: " + fileName);
      }
    } catch (IOException e) { System.out.println("Client disconnected"); }
  }
}
