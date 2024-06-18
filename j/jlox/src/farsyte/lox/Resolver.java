package farsyte.lox;

import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Stack;

class Resolver implements Expr.Visitor<Void>, Stmt.Visitor<Void> {
    private final Interpreter interpreter;
    private final Stack<Map<String, Boolean>> scopes = new Stack<>();

    Resolver (Interpreter interpreter) {
	this.interpreter = interpreter;
    }
    
    void resolve(List<Stmt> statements) {
	for (Stmt statement : statements) {
	    resolve(statement);
	}
    }

    private void resolve(Stmt stmt) {
	stmt.accept(this);
    }

    private void resolve(Expr expr) {
	expr.accept(this);
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
    public Void visitBlockStmt(Stmt.Block stmt) {
	beginScope();
	resolve(stmt.statements);
	endScope();
	return null;
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

    private void beginScope() {
	scopes.push(new HashMap<String, Boolean>());
    }

    private void endScope() {
	scopes.pop();
    }
}
