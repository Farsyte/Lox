package farsyte.lox;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

class Interpreter implements Expr.Visitor<Object>, Stmt.Visitor<Void> {
    private static class LoxBreakException extends RuntimeException {}

    final Environment globals = new Environment();
    private Environment environment = globals;
    private final Map<Expr, Integer> locals = new HashMap<>();

    Interpreter() {

	globals.define("clock", new LoxCallable() {
		@Override
		public int arity() { return 0; }
		@Override
		public Object call(Interpreter interpreter, List<Object> arguments) {
		    return (double) System.currentTimeMillis() / 1000.0; }
		@Override
		public String toString() { return "<native fn>"; } });
    }

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
		execute(statement);
	    }
	} catch (RuntimeError error) {
	    Lox.runtimeError(error);
	}
    }

    void executeBlock(
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

    private void execute(Stmt stmt) {
	stmt.accept(this);
    }

    ////////////////////////////////////////////////////////////////////////

    @Override
    public Void visitBlockStmt(Stmt.Block blk) {
	executeBlock(blk.statements, new Environment(environment));
	return null;
    }

    @Override
    public Void visitBreakStmt(Stmt.Break stmt) {
	throw new LoxBreakException();
    }

    @Override
    public Void visitClassStmt(Stmt.Class stmt) {
	environment.define(stmt.name.lexeme, null);
	Map<String, LoxFunction> methods = new HashMap<>();
	for (Stmt.Function method : stmt.methods) {
	    LoxFunction function = new LoxFunction(method, environment);
	    methods.put(method.name.lexeme, function);
	}
	LoxClass klass = new LoxClass(stmt.name.lexeme, methods);
	environment.assign(stmt.name, klass);
	return null;
    }

    @Override
    public Void visitExpressionStmt(Stmt.Expression stmt) {
	evaluate(stmt.expression);
	return null;
    }

    @Override
    public Void visitFunctionStmt(Stmt.Function stmt) {
	LoxFunction function = new LoxFunction(stmt, environment);
	environment.define(stmt.name.lexeme, function);
	return null;
    }

    @Override
    public Void visitIfStmt(Stmt.If stmt) {
	if (isTruthy(evaluate(stmt.condition))) {
	    execute(stmt.thenBranch);
	} else if (stmt.elseBranch != null) {
	    execute(stmt.elseBranch);
	}
	return null;
    }

    @Override
    public Void visitPrintStmt(Stmt.Print stmt) {
	Object value = evaluate(stmt.expression);
	System.out.println(stringify(value));
	return null;
    }

    @Override
    public Void visitReturnStmt(Stmt.Return stmt) {
	Object value = null;
	if (stmt.value != null) {
	    value = evaluate(stmt.value);
	}
	throw new Return(value);
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
    public Void visitWhileStmt(Stmt.While stmt) {
	try {
	    while (isTruthy(evaluate(stmt.condition))) {
		execute(stmt.body);
	    }
	} catch (LoxBreakException error) {
	    ;
	}
	return null;
    }

    @Override
    public Object visitAssignExpr(Expr.Assign expr) {
	Object value = evaluate(expr.value);
	Integer distance = locals.get(expr);
	if (distance != null) {
	    environment.assignAt(distance, expr.name, value);
	} else {
	    globals.assign(expr.name, value);
	}

	return value;
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
    public Object visitCallExpr(Expr.Call expr) {
	Object callee = evaluate(expr.callee);

	List<Object> arguments = new ArrayList<>();
	for (Expr argument : expr.arguments) {
	    arguments.add(evaluate(argument));
	}

	if (!(callee instanceof LoxCallable)) {
	    throw new RuntimeError(
		expr.paren, "Can only call functions and classes.");
	}

	LoxCallable function = (LoxCallable)callee;
	if (arguments.size() != function.arity()) {
	    throw new RuntimeError(
		expr.paren,
		"Expected " + function.arity() + " arguments," +
		" but got " + arguments.size() + ".");
	}
	return function.call(this, arguments);
    }

    @Override
    public Object visitGetExpr(Expr.Get expr) {
	Object object = evaluate(expr.object);
	if (object instanceof LoxInstance) {
	    return ((LoxInstance) object).get(expr.name);
	}

	throw new RuntimeError(
	    expr.name, "Only instances have properties.");
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
    public Object visitLogicalExpr(Expr.Logical expr) {
	Object left = evaluate(expr.left);

	if (expr.operator.type == TokenType.OR) {
	    if (isTruthy(left)) return left;
	} else {
	    if (!isTruthy(left)) return left;
	}

	return evaluate(expr.right);
    }

    @Override
    public Object visitSetExpr(Expr.Set expr) {
	Object object = evaluate(expr.object);

	if (!(object instanceof LoxInstance)) {
	    throw new RuntimeError(
		expr.name, "Only instances have fields.");
	}

	Object value = evaluate(expr.value);
	((LoxInstance)object).set(expr.name, value);
	return value;
    }

    @Override
    public Object visitThisExpr(Expr.This expr) {
	throw new NotImplementedException();
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
	return lookUpVariable(expr.name, expr);
    }

    ////////////////////////////////////////////////////////////////////////

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

    public void resolve(Expr expr, int depth) {
	locals.put(expr, depth);
    }

    public Object lookUpVariable(Token name, Expr expr) {
	Integer distance = locals.get(expr);
	if (distance != null) {
	    return environment.getAt(distance, name.lexeme);
	} else {
	    return globals.get(name);
	}
    }
}
