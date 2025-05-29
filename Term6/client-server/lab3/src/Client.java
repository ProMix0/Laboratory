import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;

public class Client {
    private static final String SERVER_HOST = "localhost";
    private static final int SERVER_PORT = 12345;
    private static final int MAX_PACKET_SIZE = 65507;

    public static void main(String[] args) {
        try (DatagramSocket socket = new DatagramSocket()) {
            BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
            InetAddress serverAddress = InetAddress.getByName(SERVER_HOST);

            while (true) {
                System.out.print("Введите команду (send <файл> или exit): ");
                String line = reader.readLine();
                if (line == null || line.equalsIgnoreCase("exit")) {
                    break;
                } else if (line.startsWith("send ")) {
                    String path = line.substring(5).trim();
                    File file = new File(path);
                    if (!file.exists()) {
                        System.out.println("Файл не найден.");
                        continue;
                    }

                    byte[] fileContent = Files.readAllBytes(file.toPath());
                    byte[] fileNameBytes = file.getName().getBytes(StandardCharsets.UTF_8);
                    ByteArrayOutputStream baos = new ByteArrayOutputStream();

                    // Формат: [длина имени][имя файла][байты файла]
                    DataOutputStream dos = new DataOutputStream(baos);
                    dos.writeInt(fileNameBytes.length);
                    dos.write(fileNameBytes);
                    dos.write(fileContent);
                    dos.flush();

                    byte[] packetData = baos.toByteArray();
                    dos.close();

                    if (packetData.length > MAX_PACKET_SIZE) {
                        System.out.println("Файл слишком большой для одного UDP-пакета.");
                        continue;
                    }

                    DatagramPacket packet = new DatagramPacket(packetData, packetData.length, serverAddress, SERVER_PORT);
                    socket.send(packet);
                    System.out.println("Файл отправлен: " + file.getName());

                    // Ожидание ответа
                    byte[] buffer = new byte[1024];
                    DatagramPacket responsePacket = new DatagramPacket(buffer, buffer.length);
                    socket.setSoTimeout(3000); // 3 сек

                    try {
                        socket.receive(responsePacket);
                        String response = new String(responsePacket.getData(), 0, responsePacket.getLength(), StandardCharsets.UTF_8);
                        System.out.println("Ответ сервера: " + response);
                    } catch (SocketTimeoutException e) {
                        System.out.println("Сервер не ответил.");
                    }
                }
            }
        } catch (IOException e) {
            System.err.println("Ошибка клиента: " + e.getMessage());
        }
    }
}
