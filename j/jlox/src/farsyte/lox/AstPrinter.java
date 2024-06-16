package farsyte.lox;

class AstPrinter implements Expr.Visitor<String>, Stmt.Visitor<String> {
    String print(Expr expr) {
        return expr.accept(this);
    }

    @Override
    public String visitWhileStmt(Stmt.While stmt) {
        throw new NotImplementedException();
    }

    @Override
    public String visitBlockStmt(Stmt.Block stmt) {
        throw new NotImplementedException();
    }

    @Override
    public String visitExpressionStmt(Stmt.Expression stmt) {
        throw new NotImplementedException();
    }

    @Override
    public String visitIfStmt(Stmt.If stmt) {
        throw new NotImplementedException();
    }

    @Override
    public String visitPrintStmt(Stmt.Print stmt) {
        throw new NotImplementedException();
    }

    @Override
    public String visitVarStmt(Stmt.Var stmt) {
        throw new NotImplementedException();
    }

    @Override
    public String visitLogicalExpr(Expr.Logical expr) {
        throw new NotImplementedException();
    }

    @Override
    public String visitVariableExpr(Expr.Variable expr) {
        throw new NotImplementedException();
    }

    @Override
    public String visitAssignExpr(Expr.Assign expr) {
        throw new NotImplementedException();
    }

    @Override
    public String visitBinaryExpr(Expr.Binary expr) {
        return parenthesize(expr.operator.lexeme,
                            expr.left, expr.right);
    }

    @Override
    public String visitGroupingExpr(Expr.Grouping expr) {
        return parenthesize("group", expr.expression);
    }

    @Override
    public String visitLiteralExpr(Expr.Literal expr) {
        if (expr.value == null) return "nil";
        return expr.value.toString();
    }

    @Override
    public String visitUnaryExpr(Expr.Unary expr) {
        return parenthesize(expr.operator.lexeme,
                            expr.right);
    }

    private String parenthesize(String name, Expr... exprs) {
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
