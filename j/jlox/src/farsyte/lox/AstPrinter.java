package farsyte.lox;

class AstPrinter implements Expr.Visitor<String>, Stmt.Visitor<String> {
    String print(Expr expr) {
	return expr.accept(this);
    }

    String print(Stmt stmt) {
	return stmt.accept(this);
    }

    @Override
    public String visitExpressionStmt(Stmt.Expression stmt) {
	return format("discard",
		      stmt.expression);
    }

    @Override
    public String visitPrintStmt(Stmt.Print stmt) {
	return format("print",
		      stmt.expression);
    }

    @Override
    public String visitVarStmt(Stmt.Var stmt) {
	return format("var " + stmt.name.lexeme,
		      stmt.initializer);
    }

    @Override
    public String visitBinaryExpr(Expr.Binary expr) {
	return format(expr.operator.lexeme,
		      expr.left, expr.right);
    }

    @Override
    public String visitGroupingExpr(Expr.Grouping expr) {
	return format("group", expr.expression);
    }

    @Override
    public String visitLiteralExpr(Expr.Literal expr) {
	return stringify(expr.value);
    }

    @Override
    public String visitUnaryExpr(Expr.Unary expr) {
	return format(expr.operator.lexeme,
		      expr.right);
    }

    @Override
    public String visitVariableExpr(Expr.Variable expr) {
	return expr.name.lexeme;
    }

    protected String format(String name, Expr... exprs) {
	StringBuilder builder = new StringBuilder();

	builder.append("(").append(name);
	for (Expr expr : exprs) {
	    builder.append(" ");
	    if (expr == null) {
		builder.append("nil");
	    } else {
		builder.append(expr.accept(this));
	    }
	}
	builder.append(")");
	return builder.toString();
    }

    private String stringify(Object object) {
	if (object == null) return "nil";

	if (object instanceof String) {
	    StringBuilder builder = new StringBuilder();
	    builder.append("\"");
	    for (char ch : ((String)object).toCharArray()) {
		switch (ch) {
		case '\b': builder.append("\\b"); break;
		case '\f': builder.append("\\f"); break;
		case '\n': builder.append("\\n"); break;
		case '\r': builder.append("\\r"); break;
		case '\t': builder.append("\\t"); break;
		case '"': builder.append("\\\""); break;
		case '\\': builder.append("\\\\"); break;
		default: builder.append(ch); break;
		}
	    }
	    builder.append("\"");
	    return builder.toString();
	}

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
