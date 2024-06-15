package farsyte.lox;

class Interpreter implements Expr.Visitor<Object> {

    @Override
    public Object visitLiteralExpr(Expr.Literal expr) {
        throw new NotImplementedException(", unreachable at end.");
    }

    @Override
    public Object visitGroupingExpr(Expr.Grouping expr) {
        throw new NotImplementedException(", unreachable at end.");
    }

    @Override
    public Object visitUnaryExpr(Expr.Unary expr) {
        throw new NotImplementedException(", unreachable at end.");
    }

    @Override
    public Object visitBinaryExpr(Expr.Binary expr) {
        throw new NotImplementedException(", unreachable at end.");
    }

}
