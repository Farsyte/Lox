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
	throw new NotImplementedException();
    }

    @Override
    public Object visitLiteralExpr(Expr.Literal expr) {
	throw new NotImplementedException();
    }

    @Override
    public Object visitUnaryExpr(Expr.Unary expr) {
	throw new NotImplementedException();
    }
}
