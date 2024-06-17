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

	while (match(BANG_EQUAL, EQUAL_EQUAL)) {
	    Token operator = previous();
	    Expr right = comparison();
	    expr = new Expr.Binary(expr, operator, right);
	}

	return expr;
    }

    private Expr comparison() {
	// comparison → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
	throw new NotImplementedException();
    }

    private Expr term() {
	// term → factor ( ( "-" | "+" ) factor )* ;
	throw new NotImplementedException();
    }

    private Expr factor() {
	// factor → unary ( ( "/" | "*" ) unary )* ;
	throw new NotImplementedException();
    }

    private Expr unary() {
	// unary → ( "!" | "-" ) unary | primary ;
	throw new NotImplementedException();
    }

    private Expr primary() {
	// primary → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
	throw new NotImplementedException();
    }

    private boolean match(TokenType... types) {
	throw new NotImplementedException();
    }

    private Token previous() {
	throw new NotImplementedException();
    }
}
