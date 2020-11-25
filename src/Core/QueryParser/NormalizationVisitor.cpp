#include "Core/QueryParser/NormalizationVisitor.hpp"
#include "Core/QueryParser/AST.hpp"

namespace VerifyTAPN {
namespace AST {

    void NormalizationVisitor::visit(NotExpression &expr, Result &context) {
        auto &tuple = static_cast<Tuple &>(context);
        Tuple any = Tuple(!tuple.negate, nullptr);
        expr.getChild().accept(*this, any);
        tuple.returnExpr = static_cast<Tuple &>(any).returnExpr;
    }

    void NormalizationVisitor::visit(OrExpression &expr, Result &context) {
        auto &tuple = static_cast<Tuple &>(context);
        Tuple left = Tuple(tuple.negate, nullptr), right = Tuple(tuple.negate, nullptr);
        if (tuple.negate) {
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);
            tuple.returnExpr = new AndExpression(static_cast<Tuple &>(left).returnExpr,
                                                 static_cast<Tuple &>(right).returnExpr);
        } else {
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);
            tuple.returnExpr = new OrExpression(static_cast<Tuple &>(left).returnExpr,
                                                static_cast<Tuple &>(right).returnExpr);
        }
    }

    void NormalizationVisitor::visit(AndExpression &expr, Result &context) {
        auto &tuple = static_cast<Tuple &>(context);
        Tuple left = Tuple(tuple.negate, nullptr), right = Tuple(tuple.negate, nullptr);
        if (tuple.negate) {
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);
            tuple.returnExpr = new OrExpression(static_cast<Tuple &>(left).returnExpr,
                                                static_cast<Tuple &>(right).returnExpr);
        } else {
            expr.getLeft().accept(*this, left);
            expr.getRight().accept(*this, right);
            tuple.returnExpr = new AndExpression(static_cast<Tuple &>(left).returnExpr,
                                                 static_cast<Tuple &>(right).returnExpr);
        }
    }

    void NormalizationVisitor::visit(AtomicProposition &expr, Result &context) {
        auto &tuple = static_cast<Tuple &>(context);
        if(!tuple.negate) 
        {
            tuple.returnExpr = new AtomicProposition(&expr.getLeft(), expr.getOperator(),
                                                     &expr.getRight());
        }
        else
        {
            AtomicProposition::op_e op;
            switch(expr.getOperator()) {
                case AtomicProposition::EQ: op = AtomicProposition::NE; break;
                case AtomicProposition::NE: op = AtomicProposition::EQ; break;
                case AtomicProposition::LE: op = AtomicProposition::LT; break;
                case AtomicProposition::LT: op = AtomicProposition::LE; break;
            }
            // flip of arguments is intentional
            tuple.returnExpr = new AtomicProposition(&expr.getRight(), op,
                                                     &expr.getLeft());
        }
    }

    void NormalizationVisitor::visit(NumberExpression &expr, Result &context) {
    }

    void NormalizationVisitor::visit(IdentifierExpression &expr, Result &context) {
    }

    void NormalizationVisitor::visit(MultiplyExpression &expr, Result &context) {
    }

    void NormalizationVisitor::visit(MinusExpression &expr, Result &context) {
    }

    void NormalizationVisitor::visit(SubtractExpression &expr, Result &context) {
    }

    void NormalizationVisitor::visit(PlusExpression &expr, Result &context) {
    }

    void NormalizationVisitor::visit(DeadlockExpression &expr, Result &context) {
        auto &tuple = static_cast<Tuple &>(context);
        tuple.returnExpr = new DeadlockExpression();
    }

    void NormalizationVisitor::visit(BoolExpression &expr, Result &context) {
        auto &tuple = static_cast<Tuple &>(context);
        tuple.returnExpr = new BoolExpression(tuple.negate == !expr.getValue());
    }

    void NormalizationVisitor::visit(Query &query, Result &context) {
        Tuple any = Tuple(false, nullptr);
        query.getChild()->accept(*this, any);

        normalizedQuery = new AST::Query(query.getQuantifier(), static_cast<Tuple &>(any).returnExpr);
    }
}
}