package farsyte.lox;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import static farsyte.lox.TokenType.*;

class Scanner {
    private final String source;
    private final List<Token> tokens = new ArrayList<>();

    Scanner(String source) {
        this.source = source;
    }

    List<Token> scanTokens() {
        System.out.println("STUB: Scanner.scanTokens() over \"" + source + "\"");
        return new ArrayList<Token>();
    }
}
