#include "TaintedStmtAnalysis.h"

//Tainted_Attr *StmtTa = new Tainted_Attr();   //each  statement only has one common StmtTa 

bool checkblock(CFGBlock* cfgb,CTmap &out)
{
	CFGBlock::iterator v = cfgb->begin();
	for (; v != cfgb->end(); v++)
	{
		if (v->getKind() == CFGElement::Kind::Statement)
		{
			CFGStmt s = v->castAs<CFGStmt>();
			Stmt_analysis(s.getStmt(),out);
			out.output();
		}
	}
	return false;
}

int Stmt_analysis(const Stmt* stmt, CTmap &out)
{
	const DeclStmt* ds; 
	const Decl* decl;
	const VarDecl* vd;
	const Expr* expr;
	DeclGroupRef::const_iterator di;
	switch (stmt->getStmtClass()) {
	case Stmt::CompoundStmtClass:break;
	case Stmt::BinaryOperatorClass:
		expr = dyn_cast<Expr>(stmt);
		Expr_analysis(expr,out);
		break;
	case Stmt::DeclStmtClass:
		ds = dyn_cast<DeclStmt>(stmt);
		if (ds->isSingleDecl())
		{
			decl = ds->getSingleDecl();
			vd = dyn_cast<VarDecl>(decl);
			expr = vd->getInit();
			if (expr != NULL)
				out.var_attr_set(vd, Expr_analysis(expr, out));
		}
		else
		{
			di = ds->getDeclGroup().begin();
			for (; di != ds->getDeclGroup().end(); di++)
			{
				vd = dyn_cast<VarDecl>(*di);
				expr = vd->getInit();
				if (expr != NULL)
					out.var_attr_set(vd, Expr_analysis(expr, out));
			}
			
		}
		break;
	default:break;
	}
	return 0;
}

Tainted_Attr* Expr_analysis(const Expr* expr, CTmap &out)
{
	const VarDecl *vd;
	const ImplicitCastExpr* icexpr;
	const DeclRefExpr* drexpr;
	Tainted_Attr* res = new Tainted_Attr();
	res->setType(TYPE_VARIABLE);
	switch (expr->getStmtClass()) {
	case Stmt::BinaryOperatorClass:
		return BinaryOperator_Expr_analysis(expr,out);	
	case Stmt::BlockExprClass:break;
	case Stmt::CallExprClass:break;
	case Stmt::ImplicitCastExprClass:
		icexpr = dyn_cast<ImplicitCastExpr>(expr);
		return Expr_analysis(icexpr->getSubExpr(),out);
	case Stmt::DeclRefExprClass:
		drexpr = dyn_cast<DeclRefExpr>(expr);
		vd = dyn_cast<VarDecl>(drexpr->getDecl());
		res->copy(out.getAttr(vd));
		return res;
	default:break;
	}
	return res;
}

Tainted_Attr* BinaryOperator_Expr_analysis(const Expr* expr, CTmap &out)
{
	Tainted_Attr* res = new Tainted_Attr();
	res->setType(TYPE_VARIABLE);
	const BinaryOperator* bo = dyn_cast<BinaryOperator>(expr);               
	Expr* lexpr = bo->getLHS();
	Expr* rexpr = bo->getRHS(); 
	const VarDecl *vd;
	const DeclRefExpr* drexpr;
	switch (bo->getOpcode()) {
	case BinaryOperatorKind::BO_Add:
		res->unionAttr(*Expr_analysis(lexpr, out), *Expr_analysis(rexpr, out));
		return res;
	case BinaryOperatorKind::BO_AddAssign:break;
	case BinaryOperatorKind::BO_And:break;
	case BinaryOperatorKind::BO_AndAssign:break;
	case BinaryOperatorKind::BO_Assign:
		drexpr = dyn_cast<DeclRefExpr>(lexpr);
		vd = dyn_cast<VarDecl>(drexpr->getDecl());
		res = Expr_analysis(rexpr,out);	
		out.var_attr_set(vd,res);
		return res;
	case BinaryOperatorKind::BO_Comma:break;
	case BinaryOperatorKind::BO_Div:break;
	case BinaryOperatorKind::BO_DivAssign:break;
	case BinaryOperatorKind::BO_EQ:break;
	case BinaryOperatorKind::BO_GE:break;
	case BinaryOperatorKind::BO_GT:break;
	case BinaryOperatorKind::BO_LAnd:break;
	case BinaryOperatorKind::BO_LE:break;
	case BinaryOperatorKind::BO_LOr:break;
	case BinaryOperatorKind::BO_LT:break;
	case BinaryOperatorKind::BO_Mul:break;
	case BinaryOperatorKind::BO_MulAssign:break;
	case BinaryOperatorKind::BO_NE:break;
	case BinaryOperatorKind::BO_Or:break;
	case BinaryOperatorKind::BO_OrAssign:break;
	case BinaryOperatorKind::BO_PtrMemD:break;
	case BinaryOperatorKind::BO_PtrMemI:break;
	case BinaryOperatorKind::BO_Rem:break;
	case BinaryOperatorKind::BO_RemAssign:break;
	case BinaryOperatorKind::BO_Shl:break;
	case BinaryOperatorKind::BO_ShlAssign:break;
	case BinaryOperatorKind::BO_Shr:break;
	case BinaryOperatorKind::BO_ShrAssign:break;
	case BinaryOperatorKind::BO_Sub:break;
	case BinaryOperatorKind::BO_SubAssign:break;
	case BinaryOperatorKind::BO_Xor:break;
	case BinaryOperatorKind::BO_XorAssign:break;
	default:break;
	}
	return 0;
}
