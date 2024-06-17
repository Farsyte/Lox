package farsyte.lox;

import java.util.List;

// throw new NotImplementedException();
class Interpreter implements Expr.Visitor<Object> {

    @Override
    public Object visitBinaryExpr(Expr.Binary expr) {
	throw new NotImplementedException();
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
	throw new NotImplementedException();
    }

    private Object evaluate(Expr expr) {
	return expr.accept(this);
    }
}
