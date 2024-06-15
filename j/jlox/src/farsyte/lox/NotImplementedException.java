package farsyte.lox;

class NotImplementedException extends RuntimeException {
    private String msg;

    NotImplementedException(String message) {
        super(message);
        this.msg = message;
    }

    NotImplementedException() {
        this(" Unimplemented.");
    }

    public String toString() {
        StackTraceElement[] stktrace = this.getStackTrace();
        StackTraceElement e = stktrace[0];
        String filename = e.getFileName();
        int linenumber = e.getLineNumber();
        String classn = e.getClassName();
        String method = e.getMethodName();

        String[] classna = classn.split("[.]");

        String relp  = String.join("/", classna);

        return "src/" + relp + ".java:" + linenumber + ": "
            + "STUB: " + classna[classna.length-1] + "." + method + msg;
    }
}
