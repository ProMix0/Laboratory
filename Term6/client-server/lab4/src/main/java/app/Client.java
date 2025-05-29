package app;

import java.io.*;
import java.net.*;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.util.Scanner;

public class Client {
    private static final String SERVER_URL = "http://localhost:4567";

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        System.out.println("Введите команду: send <путь к файлу>, list, exit");

        while (true) {
            System.out.print("> ");
            String line = scanner.nextLine();

            if (line.equalsIgnoreCase("exit")) {
                System.out.println("Выход.");
                break;
            } else if (line.startsWith("send ")) {
                String filePath = line.substring(5).trim();
                File file = new File(filePath);
                if (!file.exists()) {
                    System.out.println("Файл не найден.");
                    continue;
                }

                try {
                    String response = uploadFile(file);
                    System.out.println("Ответ сервера: " + response);
                } catch (IOException e) {
                    System.out.println("Ошибка загрузки: " + e.getMessage());
                }

            } else if (line.equalsIgnoreCase("list")) {
                try {
                    String response = getFileList();
                    System.out.println("Список файлов: " + response);
                } catch (IOException e) {
                    System.out.println("Ошибка получения списка файлов: " + e.getMessage());
                }
            } else {
                System.out.println("Неизвестная команда.");
            }
        }
    }

    private static String uploadFile(File file) throws IOException {
        String boundary = "===" + System.currentTimeMillis() + "===";
        URL url = new URL(SERVER_URL + "/upload");
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();

        conn.setDoOutput(true);
        conn.setRequestMethod("POST");
        conn.setRequestProperty("Content-Type", "multipart/form-data; boundary=" + boundary);

        try (OutputStream out = conn.getOutputStream();
             PrintWriter writer = new PrintWriter(new OutputStreamWriter(out, StandardCharsets.UTF_8), true)) {

            // Заголовок файла
            writer.append("--").append(boundary).append("\r\n");
            writer.append("Content-Disposition: form-data; name=\"file\"; filename=\"")
                    .append(file.getName()).append("\"\r\n");
            writer.append("Content-Type: application/octet-stream\r\n\r\n");
            writer.flush();

            // Данные файла
            Files.copy(file.toPath(), out);
            out.flush();

            // Закрытие формы
            writer.append("\r\n").flush();
            writer.append("--").append(boundary).append("--").append("\r\n");
            writer.flush();
        }

        int responseCode = conn.getResponseCode();
        InputStream responseStream = responseCode == 200
                ? conn.getInputStream()
                : conn.getErrorStream();

        try (BufferedReader in = new BufferedReader(new InputStreamReader(responseStream))) {
            StringBuilder response = new StringBuilder();
            String line;
            while ((line = in.readLine()) != null) response.append(line);
            return response.toString();
        }
    }

    private static String getFileList() throws IOException {
        URL url = new URL(SERVER_URL + "/files");
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setRequestMethod("GET");

        try (BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream(), StandardCharsets.UTF_8))) {
            StringBuilder response = new StringBuilder();
            String line;
            while ((line = in.readLine()) != null) response.append(line);
            return response.toString();
        }
    }
}
