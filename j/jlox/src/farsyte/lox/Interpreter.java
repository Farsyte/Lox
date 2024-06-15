package farsyte.lox;

class Interpreter implements Expr.Visitor<Object> {

    private Object evaluate(Expr expr) {
        return expr.accept(this);
    }

    @Override
    public Object visitLiteralExpr(Expr.Literal expr) {
        return expr.value;
    }

    @Override
    public Object visitGroupingExpr(Expr.Grouping expr) {
        return evaluate(expr.expression);
    }

    @Override
    public Object visitUnaryExpr(Expr.Unary expr) {
        Object right = evaluate(expr.right);

        switch (expr.operator.type) {
        case MINUS:
            return -(double)right;
        }

        // Unreachable.
        throw new NotImplementedException(", unreachable at end.");
    }

    @Override
    public Object visitBinaryExpr(Expr.Binary expr) {
        throw new NotImplementedException();
    }

}
