package app.retrofit;

import okhttp3.MultipartBody;
import okhttp3.ResponseBody;
import retrofit2.Call;
import retrofit2.http.*;

import java.util.List;

public interface FileService {
    @Multipart
    @POST("upload")
    Call<ResponseBody> uploadFile(@Part MultipartBody.Part file);

    @GET("files")
    Call<List<String>> listFiles();
}
