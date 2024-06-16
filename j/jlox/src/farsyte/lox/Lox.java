package farsyte.lox;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.List;
import java.util.ArrayList;

public class Lox {
    public static void main(String[] args) throws IOException {
        try {
            if (args.length > 1) {
                System.out.println("Usage: jlox [script]");
                System.exit(64);
            } else if (args.length == 1) {
                runFile(args[0]);
            } else {
                runPrompt();
            }

        } catch (NotImplementedException e) {
            System.err.println(e.toString());
        }
    }

    private static void runFile(String path) throws IOException {
        byte[] bytes = Files.readAllBytes(Paths.get(path));
        run(new String(bytes, Charset.defaultCharset()));
    }

    private static void runPrompt() throws IOException {
        throw new NotImplementedException();
    }

    private static void run(String source) {
        throw new NotImplementedException();
    }
}
