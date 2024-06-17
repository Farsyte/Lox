package farsyte.lox;

import java.util.List;

class Interpreter implements Expr.Visitor<Object>, Stmt.Visitor<Void> {
    private Environment environment = new Environment();

    // No harm in keeping around the ability to
    // interpret just an expression. Might come
    // in handy in the future (for example, if we
    // allow the REPL to accept expressions).

    void interpret(Expr expression) {
	try {
	    Object value = evaluate(expression);
	    System.out.println(stringify(value));
	} catch (RuntimeError error) {
	    Lox.runtimeError(error);
	}
    }

    void interpret(List<Stmt> statements) {
	try {
	    for (Stmt statement : statements) {
		System.out.println("LISPish AST: " + new AstPrinter().print(statement));
		System.out.println("RPNish AST:  " + new AstRpnPrinter().print(statement));
		execute(statement);
	    }
	} catch (RuntimeError error) {
	    Lox.runtimeError(error);
	}
    }

    private void execute(Stmt stmt) {
	stmt.accept(this);
    }

    @Override
    public Void visitExpressionStmt(Stmt.Expression stmt) {
	evaluate(stmt.expression);
	return null;
    }

    @Override
    public Void visitPrintStmt(Stmt.Print stmt) {
	Object value = evaluate(stmt.expression);
	System.out.println(stringify(value));
	return null;
    }

    @Override
    public Void visitVarStmt(Stmt.Var stmt) {
	Object value = null;

	if (stmt.initializer != null) {
	    value = evaluate(stmt.initializer);
	}

	environment.define(stmt.name.lexeme, value);
	return null;
    }

    @Override
    public Void visitBlockStmt(Stmt.Block blk) {
	executeBlock(blk.statements, new Environment(environment));
	return null;
    }

    @Override
    public Void visitIfStmt(Stmt.If stmt) {
	throw new NotImplementedException();
    }

    @Override
    public Object visitBinaryExpr(Expr.Binary expr) {
	Object left = evaluate(expr.left);
	Object right = evaluate(expr.right);

	switch (expr.operator.type) {

	case PLUS:
	    if (left instanceof Double && right instanceof Double) {
		return (double)left + (double)right;
	    }
	    if (left instanceof String && right instanceof String) {
		return (String)left + (String)right;
	    }
	    if (left instanceof String) {
		return (String)left + stringify(right);
	    }
	    throw new RuntimeError(expr.operator, "Operands must be two Numbers, or two Strings.");

	case MINUS:
	    checkNumberOperands(expr.operator, left, right);
	    return (double)left - (double)right;

	case SLASH:
	    checkNumberOperands(expr.operator, left, right);
	    if (0.0 == (double)right)
		throw new RuntimeError(expr.operator, "Division by zero.");
	    double result = (double)left / (double)right;
	    if (Double.isNaN(result))
		throw new RuntimeError(expr.operator, "Division produced a NaN.");
	    if (Double.isInfinite(result))
		throw new RuntimeError(expr.operator, "Division produced Infinity.");
	    return result;

	case STAR:
	    checkNumberOperands(expr.operator, left, right);
	    return (double)left * (double)right;

	case GREATER:
	    if (left instanceof Double && right instanceof Double) {
		return (double)left > (double)right;
	    }
	    if (left instanceof String && right instanceof String) {
		return ((String)left).compareTo((String)right) > 0;
	    }
	    throw new RuntimeError(expr.operator, "Operands must be two Numbers, or two Strings.");
	case GREATER_EQUAL:
	    if (left instanceof Double && right instanceof Double) {
		return (double)left >= (double)right;
	    }
	    if (left instanceof String && right instanceof String) {
		return ((String)left).compareTo((String)right) >= 0;
	    }
	    throw new RuntimeError(expr.operator, "Operands must be two Numbers, or two Strings.");
	case LESS:
	    if (left instanceof Double && right instanceof Double) {
		return (double)left < (double)right;
	    }
	    if (left instanceof String && right instanceof String) {
		return ((String)left).compareTo((String)right) < 0;
	    }
	    throw new RuntimeError(expr.operator, "Operands must be two Numbers, or two Strings.");
	case LESS_EQUAL:
	    if (left instanceof Double && right instanceof Double) {
		return (double)left <= (double)right;
	    }
	    if (left instanceof String && right instanceof String) {
		return ((String)left).compareTo((String)right) <= 0;
	    }
	    throw new RuntimeError(expr.operator, "Operands must be two Numbers, or two Strings.");

	case BANG_EQUAL:
	    return !isEqual(left, right);
	case EQUAL_EQUAL:
	    return isEqual(left, right);

	}

	// Unreachable.
	throw new NotImplementedException(", unreachable at end.");
    }

    @Override
    public Object visitGroupingExpr(Expr.Grouping expr) {
	return evaluate(expr.expression);
    }

    @Override
    public Object visitLiteralExpr(Expr.Literal expr) {
	return expr.value;
    }

    @Override
    public Object visitUnaryExpr(Expr.Unary expr) {
	Object right = evaluate(expr.right);

	switch (expr.operator.type) {
	case BANG:
	    return !isTruthy(right);
	case MINUS:
	    checkNumberOperand(expr.operator, right);
	    return -(double)right;
	}

	// Unreachable.
	throw new NotImplementedException(", unreachable at end.");
    }

    @Override
    public Object visitVariableExpr(Expr.Variable expr) {
	return environment.get(expr.name);
    }

    @Override
    public Object visitAssignExpr(Expr.Assign expr) {
	Object value = evaluate(expr.value);
	environment.assign(expr.name, value);
	return value;
    }

    private void executeBlock(
	List<Stmt> statements, Environment environment) {

	Environment previous = this.environment;
	try {
	    this.environment = environment;

	    for (Stmt statement : statements) {
		execute(statement);
	    }
	} finally {
	    this.environment = previous;
	}
    }

    private Object evaluate(Expr expr) {
	return expr.accept(this);
    }

    private void checkNumberOperand(Token operator, Object operand) {
	if (operand instanceof Double) return;
	throw new RuntimeError(operator, "Operand must be a Number.");
    }

    private void checkNumberOperands(Token operator, Object left, Object right) {
	if (left instanceof Double && right instanceof Double) return;
	throw new RuntimeError(operator, "Operands must be Numbers.");
    }

    private boolean isTruthy(Object object) {
	if (object == null) return false;
	if (object instanceof Boolean) return (boolean)object;
	return true;
    }

    private boolean isEqual(Object a, Object b) {
	if (a == null && b == null) return true;
	if (a == null) return false;
	return a.equals(b);
    }

    private String stringify(Object object) {
	if (object == null) return "nil";

	if (object instanceof Double) {
	    String text = object.toString();
	    if (text.endsWith(".0")) {
		text = text.substring(0, text.length() - 2);
	    }
	    return text;
	}

	return object.toString();
    }
}
