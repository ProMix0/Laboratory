package app.retrofit;

import okhttp3.MediaType;
import okhttp3.MultipartBody;
import okhttp3.RequestBody;

import java.io.File;

public class MultipartUtils {
    public static MultipartBody.Part prepareFilePart(String partName, File file) {
        RequestBody requestFile = RequestBody.create(file, MediaType.parse("application/octet-stream"));
        return MultipartBody.Part.createFormData(partName, file.getName(), requestFile);
    }
}
