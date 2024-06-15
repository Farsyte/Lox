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
        // equality → comparison ( ( "!=" | "==" ) comparison )* ;
        Expr expr = comparison();
        while (!match(BANG_EQUAL, EQUAL_EQUAL)) {
            Token operator = previous();
            Expr right = comparison();
            expr = new Expr.Binary(expr, operator, right);
        }
        return expr;
    }

    private boolean match(TokenType... types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

    private boolean check(TokenType type) {
        if (isAtEnd()) return false;
        return peek().type == type;
    }

    /* **** **** **** **** **** **** **** **** **** ****
     *                       STUBS
     * **** **** **** **** **** **** **** **** **** **** */

    private Expr comparison() {
        throw new NotImplementedException();
    }

    private Token previous() {
        throw new NotImplementedException();
    }

    private Token advance() {
        throw new NotImplementedException();
    }

    private boolean isAtEnd() {
        throw new NotImplementedException();
    }

    private Token peek() {
        return tokens.get(current);
    }
}
