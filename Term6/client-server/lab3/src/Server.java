import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;

public class Server {
    private static final int PORT = 12345;
    private static final String STORAGE_DIR = "storage/server_storage";

    public static void main(String[] args) throws IOException {
        new File(STORAGE_DIR).mkdirs();

        try (DatagramSocket socket = new DatagramSocket(PORT)) {
            System.out.println("UDP-сервер запущен на порту " + PORT);

            byte[] buffer = new byte[65507];
            DatagramPacket packet = new DatagramPacket(buffer, buffer.length);

            while (true) {
                socket.receive(packet);

                try {
                    ByteArrayInputStream bais = new ByteArrayInputStream(packet.getData(), 0, packet.getLength());
                    DataInputStream dis = new DataInputStream(bais);

                    int nameLength = dis.readInt();
                    byte[] nameBytes = new byte[nameLength];
                    dis.readFully(nameBytes);
                    String fileName = new String(nameBytes, StandardCharsets.UTF_8);
		    dis.close();

                    byte[] fileData = bais.readAllBytes();
                    File file = new File(STORAGE_DIR, fileName);
                    try (FileOutputStream fos = new FileOutputStream(file)) {
                        fos.write(fileData);
                    }

                    System.out.println("Файл принят: " + fileName + " от " + packet.getAddress() + ":" + packet.getPort());

                    // Ответ клиенту
                    String response = "Файл \"" + fileName + "\" успешно получен.";
                    byte[] responseBytes = response.getBytes(StandardCharsets.UTF_8);
                    DatagramPacket responsePacket = new DatagramPacket(
                            responseBytes, responseBytes.length,
                            packet.getAddress(), packet.getPort()
                    );
                    socket.send(responsePacket);
                } catch (IOException e) {
                    System.err.println("Ошибка при обработке пакета: " + e.getMessage());
                }
            }
        }
    }
}
