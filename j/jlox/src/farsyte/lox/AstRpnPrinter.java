package farsyte.lox;

class AstRpnPrinter extends AstPrinter {

    @Override
    protected String format(String name, Expr... exprs) {
	StringBuilder builder = new StringBuilder();

	for (Expr expr : exprs) {
	    builder.append(expr.accept(this));
	    builder.append(" ");
	}
	builder.append(name);
	return builder.toString();
    }

    @Override
    protected String format(String name, Stmt... stmts) {
	StringBuilder builder = new StringBuilder();

	builder.append("{\n");
	for (Stmt stmt : stmts) {
	    builder.append(stmt.accept(this));
	    builder.append("\n");
	}
	builder.append("} ");
	builder.append(name);
	return builder.toString();
    }
}
