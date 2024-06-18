package farsyte.lox;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

class Resolver implements Expr.Visitor<Void>, Stmt.Visitor<Void> {
    private final Interpreter interpreter;

    Resolver (Interpreter interpreter) {
	this.interpreter = interpreter;
    }
    
    @Override
    public Void visitExpressionStmt(Stmt.Expression stmt) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitPrintStmt(Stmt.Print stmt) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitVarStmt(Stmt.Var stmt) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitBlockStmt(Stmt.Block blk) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitIfStmt(Stmt.If stmt) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitWhileStmt(Stmt.While stmt) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitBreakStmt(Stmt.Break stmt) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitFunctionStmt(Stmt.Function stmt) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitReturnStmt(Stmt.Return stmt) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitBinaryExpr(Expr.Binary expr) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitGroupingExpr(Expr.Grouping expr) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitLiteralExpr(Expr.Literal expr) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitUnaryExpr(Expr.Unary expr) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitVariableExpr(Expr.Variable expr) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitAssignExpr(Expr.Assign expr) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitLogicalExpr(Expr.Logical expr) {
	throw new NotImplementedException();
    }

    @Override
    public Void visitCallExpr(Expr.Call expr) {
	throw new NotImplementedException();
}
