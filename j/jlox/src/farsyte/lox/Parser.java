package farsyte.lox;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static farsyte.lox.TokenType.*;

class Parser {
    private static class ParseError extends RuntimeException {}

    private final List<Token> tokens;
    private int current = 0;
    private boolean canBreak = false;

    Parser(List<Token> tokens) {
	this.tokens = tokens;
    }

    List<Stmt> parse() {
	// program → declaration* EOF ;
	List<Stmt> statements = new ArrayList<>();
	while (!isAtEnd()) {
	    statements.add(declaration());
	}
	return statements;
    }

    private Stmt declaration() {
	// declaration → varDecl | statement ;
	try {
	    if (match(FUN)) return function("function");
	    if (match(VAR)) return varDeclaration();
	    return statement();
	} catch (ParseError error) {
	    synchronize();
	    return null;
	}
    }

    private Stmt varDeclaration() {
	// varDecl → "var" IDENTIFIER ( "=" expression )? ";" ;
	Token name = consume(IDENTIFIER, "Expect variable name.");

	Expr initializer = null;
	if (match(EQUAL)) {
	    initializer = expression();
	}
	consume(SEMICOLON, "Expect ';' after variable declaration.");
	return new Stmt.Var(name, initializer);
    }

    private Stmt function(String kind) {
	// funDecl → "fun" function ;
	// function → IDENTIFIER "(" parameters? ")" block ;
	// parameters → IDENTIFIER ( "," IDENTIFIER )* ;

	Token name = consume(IDENTIFIER, "Expect " + kind + " name.");

	consume(LEFT_PAREN, "Expect '(' after " + kind + " name.");
	List<Token> parameters = new ArrayList<>();
	if (!check(RIGHT_PAREN)) {
	    do {
		if (parameters.size() > 255) {
		    error(peek(), "Can't have more than 255 parameters.");
		}

		parameters.add(
		    consume(IDENTIFIER, "Expect parameter name."));
	    } while (match(COMMA));
	}
	consume(RIGHT_PAREN, "Expect ')' after parameters.");

	consume(LEFT_BRACE, "Expect '{' before " + kind + " body.");
	List<Stmt> body = block();
	return new Stmt.Function(name, parameters, body);
    }

    private Stmt statement() {
	// statement → exprStmt | printStmt | ifStmt | block;
	if (match(IF)) return ifStatement();
	if (match(BREAK)) return breakStatement();
	if (match(FOR)) return forStatement();
	if (match(WHILE)) return whileStatement();
	if (match(PRINT)) return printStatement();
	if (match(LEFT_BRACE)) return new Stmt.Block(block());
	return expressionStatement();
    }

    private Stmt breakStatement() {
	if (!canBreak) {
	    throw error(previous(), "Break must be inside a 'for' or 'while' loop body.");
	}

	consume(SEMICOLON, "Expect ';' after variable declaration.");
	return new Stmt.Break();
    }

    private Stmt whileStatement() {
	consume(LEFT_PAREN, "Expect '(' after 'while'.");
	Expr condition = expression();
	consume(RIGHT_PAREN, "Expect ')' after condition.");
	boolean couldBreak = canBreak;
	canBreak = true;
	Stmt body = statement();
	canBreak = couldBreak;
	return new Stmt.While(condition, body);
    }

    private Stmt ifStatement() {
	// ifStmt → "if" "(" expression ")" statement ( "else" statement )? ;
	consume(LEFT_PAREN, "Expect '(' after 'íf'.");
	Expr condition = expression();
	consume(RIGHT_PAREN, "Expect ')' after if condition.");

	Stmt thenBranch = statement();
	Stmt elseBranch = null;
	if (match(ELSE)) {
	    elseBranch = statement();
	}

	return new Stmt.If(condition, thenBranch, elseBranch);
    }

    private Stmt forStatement() {
	consume(LEFT_PAREN, "expect '(' after 'for'.");

	Stmt initializer;
	if (match(SEMICOLON)) {
	    initializer = null;
	} else if (match(VAR)) {
	    initializer = varDeclaration();
	} else {
	    initializer = expressionStatement();
	}

	Expr condition = null;
	if (!check(SEMICOLON)) {
	    condition = expression();
	}
	consume(SEMICOLON, "Expect ';' after loop condition.");

	Expr increment = null;
	if (!check(RIGHT_PAREN)) {
	    increment = expression();
	}
	consume(RIGHT_PAREN, "Expect ')' after for clauses.");

	boolean couldBreak = canBreak;
	canBreak = true;

	Stmt body = statement();

	if (increment != null) {
	    body = new Stmt.Block(
		Arrays.asList(
		    body,
		    new Stmt.Expression(increment)));
	}

	if (condition == null) {
	    condition = new Expr.Literal(true);
	}
	body = new Stmt.While(condition, body);

	canBreak = couldBreak;

	if (initializer != null) {
	    body = new Stmt.Block(
		Arrays.asList(
		    initializer,
		    body));
	}

	return body;
    }

    private List<Stmt> block() {
	// block → "{" declaration* "}" ;
	List<Stmt> statements = new ArrayList<>();

	while (!check(RIGHT_BRACE) && !isAtEnd()) {
	    statements.add(declaration());
	}

	consume(RIGHT_BRACE, "Expect '}' after block.");

	// From Crafting Interpreters, and worth including here:
	//
	// having block() return the raw list of statements and leaving
	// it to statement() to wrap the list in a Stmt.Block looks a
	// bit odd. I did it that way because we'll reuse block() later
	// for parsing function bodies and we don't want that body
	// wrapped in a Stmt.Block. [§8.5.2 of Crafting Interpreters]

	return statements;
    }

    private Stmt expressionStatement() {
	// exprStmt → expression ";" ;
	Expr expr = expression();
	consume(SEMICOLON, "Expect ';' after value.");
	return new Stmt.Expression(expr);
    }

    private Stmt printStatement() {
	// printStmt → "print" expression ";" ;
	Expr value = expression();
	consume(SEMICOLON, "Expect ';' after value.");
	return new Stmt.Print(value);
    }

    private Expr expression() {
	// expression → assignment
	return assignment();
    }

    private Expr assignment() {
	// assignment → IDENTIFIER = assignment | logical_or ;
	Expr expr = or();

	if (match(EQUAL)) {
	    Token equals = previous();
	    Expr value = assignment();

	    if (expr instanceof Expr.Variable) {
		Token name = ((Expr.Variable)expr).name;
		return new Expr.Assign(name, value);
	    }

	    throw error(equals, "Invalid assignment target.");
	}

	return expr;
    }

    private Expr or() {
	// logic_or → logic_and ( "or" logic_and )* ;
	Expr expr = and();

	while (match(OR)) {
	    Token operator = previous();
	    Expr right = and();
	    expr = new Expr.Logical(expr, operator, right);
	}

	return expr;
    }

    private Expr and() {
	// logic_and → equality ( "and" equality )* ;
	Expr expr = equality();

	while (match(AND)) {
	    Token operator = previous();
	    Expr right = equality();
	    expr = new Expr.Logical(expr, operator, right);
	}

	return expr;
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
	Expr expr = term();

	while (match(GREATER, GREATER_EQUAL, LESS, LESS_EQUAL)) {
	    Token operator = previous();
	    Expr right = term();
	    expr = new Expr.Binary(expr, operator, right);
	}

	return expr;
    }

    private Expr term() {
	// term → factor ( ( "-" | "+" ) factor )* ;
	Expr expr = factor();

	while (match(MINUS, PLUS)) {
	    Token operator = previous();
	    Expr right = factor();
	    expr = new Expr.Binary(expr, operator, right);
	}

	return expr;
    }

    private Expr factor() {
	// factor → unary ( ( "/" | "*" ) unary )* ;
	Expr expr = unary();

	while (match(SLASH, STAR)) {
	    Token operator = previous();
	    Expr right = unary();
	    expr = new Expr.Binary(expr, operator, right);
	}

	return expr;
    }

    private Expr unary() {
	// unary → ( "!" | "-" ) unary | call ;
	if (match(BANG, MINUS)) {
	    Token operator = previous();
	    Expr right = unary();
	    return new Expr.Unary(operator, right);
	}

	return call();
    }

    private Expr call() {
	// call → primary ( "(" arguments? ")" )* ;
	Expr expr = primary();

	while (true) {
	    if (match(LEFT_PAREN)) {
		expr = finishCall(expr);
	    } else {
		break;
	    }
	}

	return expr;
	// throw new NotImplementedException();
    }

    private Expr finishCall(Expr callee) {
	List<Expr> arguments = new ArrayList<>();
	if (!check(RIGHT_PAREN)) {
	    do {
		if (arguments.size() >= 255) {
		    // Report the problem but DO NOT throw it.
		    // This is how we do warnings.
		    error(peek(), "Can't have more than 255 arguments.");
		}
		arguments.add(expression());
	    } while (match(COMMA));
	}

	Token paren = consume(
	    RIGHT_PAREN, "Expect ')' after arguments.");

	return new Expr.Call(callee, paren, arguments);
    }

    private Expr primary() {
	// primary → NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" ;
	if (match(FALSE)) return new Expr.Literal(false);
	if (match(TRUE)) return new Expr.Literal(true);
	if (match(NIL)) return new Expr.Literal(null);

	if (match(IDENTIFIER)) {
	    return new Expr.Variable(previous());
	}

	if (match(NUMBER, STRING)) {
	    return new Expr.Literal(previous().literal);
	}

	if (match(LEFT_PAREN)) {
	    Expr expr = expression();
	    consume(RIGHT_PAREN, "Expect ')' after expression.");
	    return new Expr.Grouping(expr);
	}

	throw error(peek(), "Expect expression.");
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

    private Token consume(TokenType type, String message) {
	if (check(type)) return advance();

	throw error(peek(), message);
    }

    private boolean check(TokenType type) {
	if (isAtEnd()) return false;
	return peek().type == type;
    }

    private Token advance() {
	if (!isAtEnd()) current++;
	return previous();
    }

    private Token peek() {
	return tokens.get(current);
    }

    private Token previous() {
	return tokens.get(current - 1);
    }

    private boolean isAtEnd() {
	return peek().type == EOF;
    }

    private ParseError error(Token token, String message) {
	Lox.error(token, message);
	return new ParseError();
    }

    private void synchronize() {
	advance();

	while (!isAtEnd()) {
	    if (previous().type == SEMICOLON) return;
	    switch (peek().type) {
	    case CLASS:
	    case FUN:
	    case VAR:
	    case FOR:
	    case IF:
	    case WHILE:
	    case PRINT:
	    case RETURN:
		return;
	    }
	    advance();
	}
    }
}
