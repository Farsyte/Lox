package farsyte.lox;

import java.util.List;

import static farsyte.lox.TokenType.*;

class Parser {
    private final List<Token> tokens;
    private int current;

    Parser(List<Token> tokens) {
        this.tokens = tokens;
    }
}
