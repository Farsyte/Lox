package farsyte.lox;

class AstRpnPrinter extends AstPrinter {

    @Override
    protected String parenthesize(String name, Expr... exprs) {
        StringBuilder builder = new StringBuilder();

	// The output for "-" will be ambiguous as to whether it is
	// UNARY NEGATE or BINARY SUBTRACT.

        for (Expr expr : exprs) {
            builder.append(expr.accept(this));
            builder.append(" ");
        }
        builder.append(name);
        return builder.toString();
    }
}
