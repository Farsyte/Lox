package farsyte.lox;

import java.util.List;

import static farsyte.lox.TokenType.*;

class Parser {
    private final List<Token> tokens;
    private int current;

    Parser(List<Token> tokens) {
        this.tokens = tokens;
    }

    private Expr expression() {
        // expression → equality ;
        return equality();
    }

    private Expr equality() {
        System.out.println("STUB: equality()");
        return null;
    }

}
