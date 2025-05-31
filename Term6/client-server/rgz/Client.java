import java.io.*;
import java.net.*;
import java.util.Scanner;
import java.nio.charset.StandardCharsets;

public class Client {
    private static final String HOST = "localhost";
    private static final int PORT = 12345;

    public static void main(String[] args) {
        try (
	TcpOverUdp client = new  TcpOverUdp();
                Scanner scanner = new Scanner(System.in)
        ) {
	client.connect(InetAddress.getByName(HOST), PORT);
            System.out.println("Connected to server");

            while (true) {
                System.out.print("Enter command: ");
                String line = scanner.nextLine();

                if (line.equalsIgnoreCase("exit")) {
                    System.out.println("Exiting");
                    break;
                } else if (line.startsWith("send ")) {
                    String filePath = line.substring(5).trim();
                    sendFile(filePath, client);
                } else {
                    System.out.println("Unknown command");
                }
            }
        } catch (IOException e) {
            System.out.println("Error: " + e.getMessage());
        }
    }

    private static void sendFile(String filePath, TcpOverUdp client) throws IOException {
        File file = new File(filePath);
        if (!file.exists() || !file.isFile()) {
            System.out.println("File not found: " + filePath);
            return;
        }

        try (FileInputStream fileIn = new FileInputStream(file)) {
            byte[] nameBytes = file.getName().getBytes();
            client.writeInt(nameBytes.length);
            client.send(nameBytes, nameBytes.length);
            client.writeLong(file.length());

            byte[] buffer = new byte[4096];
            int count;
            while ((count = fileIn.read(buffer)) > 0) {
                client.send(buffer, count);
            }

            System.out.println("File sent: " + file.getName());
        } catch (IOException e) {
            System.out.println("File sending error: " + e.getMessage());
	    throw e;
        }
    }
}
