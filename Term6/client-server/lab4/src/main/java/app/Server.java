package app;

import static spark.Spark.*;

import java.io.*;
import java.util.*;

import com.google.gson.Gson;
import org.apache.commons.io.FileUtils;

import javax.servlet.MultipartConfigElement;

public class Server {
    private static final String STORAGE_DIR = "storage/server_storage";
    private static final Gson gson = new Gson();

    public static void main(String[] args) {
        port(4567);
        new File(STORAGE_DIR).mkdirs();

        post("/upload", (req, res) -> {
            req.attribute("org.eclipse.jetty.multipartConfig", new MultipartConfigElement("/tmp"));

            try (InputStream is = req.raw().getPart("file").getInputStream()) {
                String fileName = req.raw().getPart("file").getSubmittedFileName();
                File targetFile = new File(STORAGE_DIR, fileName);
                FileUtils.copyInputStreamToFile(is, targetFile);
                res.status(200);
                return "Файл сохранён: " + fileName;
            } catch (Exception e) {
                res.status(500);
                return "Ошибка: " + e.getMessage();
            }
        });

        get("/files", (req, res) -> {
            File dir = new File(STORAGE_DIR);
            String[] files = dir.list();
            List<String> fileList = files != null ? Arrays.asList(files) : List.of();
            res.type("application/json");
            return gson.toJson(fileList);
        });
    }
}
