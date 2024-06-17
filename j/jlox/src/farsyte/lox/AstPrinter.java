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
	if (expr.value == null) return "nil";
	return expr.value.toString();
    }

    @Override
    public String visitUnaryExpr(Expr.Unary expr) {
	return format(expr.operator.lexeme,
		      expr.right);
    }

    protected String format(String name, Expr... exprs) {
	StringBuilder builder = new StringBuilder();

	builder.append("(").append(name);
	for (Expr expr : exprs) {
	    builder.append(" ");
	    builder.append(expr.accept(this));
	}
	builder.append(")");
	return builder.toString();
    }
}
