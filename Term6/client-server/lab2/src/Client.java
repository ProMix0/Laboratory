import java.io.*;
import java.net.Socket;
import java.util.Scanner;
import java.nio.charset.StandardCharsets;

public class Client {
    private static final String HOST = "localhost";
    private static final int PORT = 12345;

    public static void main(String[] args) {
        try (
                Socket socket = new Socket(HOST, PORT);
                DataInputStream in = new DataInputStream(socket.getInputStream());
                DataOutputStream out = new DataOutputStream(socket.getOutputStream());
                Scanner scanner = new Scanner(System.in)
        ) {
            System.out.println("Подключено к серверу: " + socket.getRemoteSocketAddress());

            while (true) {
                System.out.print("Команда: ");
                String line = scanner.nextLine();

                if (line.equalsIgnoreCase("exit")) {
                    System.out.println("Завершение работы клиента.");
                    break;
                } else if (line.startsWith("send ")) {
                    String filePath = line.substring(5).trim();
                    sendFile(filePath, out);

                    // Чтение ответа от сервера
                    int responseLength = in.readInt();
                    byte[] responseBytes = new byte[responseLength];
                    in.readFully(responseBytes);
                    String response = new String(responseBytes, StandardCharsets.UTF_8);
                    System.out.println("Ответ сервера: " + response);
                } else {
                    System.out.println("Неизвестная команда.");
                }
            }
        } catch (IOException e) {
            System.out.println("Ошибка: " + e.getMessage());
        }
    }

    private static void sendFile(String filePath, DataOutputStream out) {
        File file = new File(filePath);
        if (!file.exists() || !file.isFile()) {
            System.out.println("Файл не найден: " + filePath);
            return;
        }

        try (FileInputStream fileIn = new FileInputStream(file)) {
            byte[] nameBytes = file.getName().getBytes();
            out.writeInt(nameBytes.length);
            out.write(nameBytes);
            out.writeLong(file.length());

            byte[] buffer = new byte[4096];
            int count;
            while ((count = fileIn.read(buffer)) > 0) {
                out.write(buffer, 0, count);
            }

            System.out.println("Файл отправлен: " + file.getName());
        } catch (IOException e) {
            System.out.println("Ошибка отправки файла: " + e.getMessage());
        }
    }
}
