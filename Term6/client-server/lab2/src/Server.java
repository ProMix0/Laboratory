import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.charset.StandardCharsets;

public class Server {
    private static final int PORT = 12345;
    private static final String STORAGE_DIR = "storage/server_storage";

    public static void main(String[] args) throws IOException {
        new File(STORAGE_DIR).mkdirs();

        try (ServerSocket serverSocket = new ServerSocket(PORT)) {
            System.out.println("Сервер запущен на порту " + PORT);

            while (true) {
                Socket clientSocket = serverSocket.accept();
                System.out.println("Клиент подключен: " + clientSocket.getRemoteSocketAddress());

                new Thread(() -> handleClient(clientSocket)).start();
            }
        }
    }

    private static void handleClient(Socket socket) {
        try (
                DataInputStream in = new DataInputStream(socket.getInputStream());
                DataOutputStream out = new DataOutputStream(socket.getOutputStream())
        ) {
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

                System.out.println("Получен файл от клиента: " + fileName);

                // Отправка ответа клиенту
                String response = "Файл \"" + fileName + "\" получен сервером.";
                byte[] responseBytes = response.getBytes(StandardCharsets.UTF_8);
                out.writeInt(responseBytes.length);
                out.write(responseBytes);
            }
        } catch (IOException e) {
            System.out.println("Клиент отключился: " + socket.getRemoteSocketAddress());
        }
    }
}
