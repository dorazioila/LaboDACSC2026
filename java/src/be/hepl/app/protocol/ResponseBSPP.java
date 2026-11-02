package be.hepl.app.protocol;

import java.io.Serializable;

public class ResponseBSPP implements Serializable {
    private static final long serialVersionUID = 1L;

    private boolean success;
    private String message;
    private Object data;

    public ResponseBSPP(boolean success, String message, Object data) {
        this.success = success;
        this.message = message;
        this.data = data;
    }

    public ResponseBSPP(boolean success, String message) {
        this(success, message, null);
    }

    public ResponseBSPP(boolean success, Object data) {
        this(success, null, data);
    }

    public boolean isSuccess() { return success; }
    public String getMessage() { return message; }
    public Object getData() { return data; }
}