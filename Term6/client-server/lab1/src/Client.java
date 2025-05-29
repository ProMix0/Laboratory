import java.io.*;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    private static final String HOST = "localhost";
    private static final int PORT = 12345;
    private static final String STORAGE_DIR = "storage/client_storage";

    public static void main(String[] args) throws IOException {
        new File(STORAGE_DIR).mkdirs();

        try (Socket socket = new Socket(HOST, PORT)) {
            System.out.println("Подключено к серверу: " + socket.getRemoteSocketAddress());

            DataInputStream in = new DataInputStream(socket.getInputStream());
            DataOutputStream out = new DataOutputStream(socket.getOutputStream());

            // Поток приёма файлов
            Thread receiveThread = new Thread(() -> receiveFiles(in));
            receiveThread.start();

            // Основной поток: ввод команд
            Scanner scanner = new Scanner(System.in);
            while (true) {
                System.out.print("Команда: ");
                String line = scanner.nextLine();
                if (line.equalsIgnoreCase("exit")) {
                    socket.close();
                    System.out.println("Соединение закрыто.");
                    break;
                } else if (line.startsWith("send ")) {
                    String path = line.substring(5).trim();
                    sendFile(path, out);
                } else {
                    System.out.println("Неизвестная команда.");
                }
            }
        }
    }

    private static void receiveFiles(DataInputStream in) {
        try {
            while (true) {
                int nameLength = in.readInt();
                byte[] nameBytes = new byte[nameLength];
                in.readFully(nameBytes);
                String fileName = new String(nameBytes);

                long fileSize = in.readLong();
                File file = new File(STORAGE_DIR, fileName);

                try (FileOutputStream fileOut = new FileOutputStream(file)) {
                    byte[] buffer = new byte[4096];
                    long remaining = fileSize;
                    while (remaining > 0) {
                        int read = in.read(buffer, 0, (int)Math.min(buffer.length, remaining));
                        if (read == -1) break;
                        fileOut.write(buffer, 0, read);
                        remaining -= read;
                    }
                }

                System.out.println("Получен файл: " + fileName);
            }
        } catch (IOException e) {
            System.out.println("Соединение закрыто (входящий поток).");
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
