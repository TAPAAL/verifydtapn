#ifndef AST_HPP_
#define AST_HPP_

#include "Visitor.hpp"

#include <PQL/PQL.h>

#include <string>
#include <iostream>

namespace VerifyTAPN {
    namespace TAPN {
        class TimedArcPetriNet;
    }

    namespace AST {

        class Visitable {
        public:
            virtual void accept(Visitor &visitor, Result &context) = 0;

            int32_t eval = 0;
        };

        class Expression : public Visitable {
        public:

            virtual ~Expression() = default;


            virtual Expression *clone() const = 0;
        };

        class NotExpression : public Expression {
        public:

            explicit NotExpression(Expression *expr) : expr(expr) {
            };

            NotExpression(const NotExpression &other) : expr(other.expr->clone()) {
            };

            NotExpression &operator=(const NotExpression &other) {
                if (&other != this) {
                    delete expr;
                    expr = other.expr->clone();
                }

                return *this;
            }

            ~NotExpression() override {
                delete expr;
            };

            NotExpression *clone() const override;

            void accept(Visitor &visitor, Result &context) override;

            Expression &getChild() const {
                return *expr;
            }

        private:
            Expression *expr;
        };

        class DeadlockExpression : public Expression {
        public:

            explicit DeadlockExpression() = default;


            ~DeadlockExpression() override = default;


            DeadlockExpression *clone() const override;

            void accept(Visitor &visitor, Result &context) override;
        };

        class BoolExpression : public Expression {
        public:

            explicit BoolExpression(bool value) : value(value) {
            };

            ~BoolExpression() override = default;


            BoolExpression *clone() const override;

            void accept(Visitor &visitor, Result &context) override;

            bool getValue() const {
                return value;
            };
        private:
            bool value;
        };

        class AtomicProposition : public Expression {
        public:

            enum op_e {
                LT, LE, EQ, NE
            };

            AtomicProposition(ArithmeticExpression *left, std::string *op, ArithmeticExpression *right);

            AtomicProposition(ArithmeticExpression *left, op_e op, ArithmeticExpression *right)
            : left(left), right(right), op(op) {
            };

            AtomicProposition &operator=(const AtomicProposition &other) {
                if (&other != this) {
                    left = other.left;
                    op = other.op;
                    right = other.right;
                }
                return *this;
            }

            ~AtomicProposition() override = default;

            ArithmeticExpression &getLeft() const {
                return *left;
            };

            ArithmeticExpression &getRight() const {
                return *right;
            };

            op_e getOperator() const {
                return op;
            };

            AtomicProposition *clone() const override;

            void accept(Visitor &visitor, Result &context) override;

        private:
            ArithmeticExpression *left;
            ArithmeticExpression *right;
            op_e op;
        };

        class AndExpression : public Expression {
        public:

            AndExpression(Expression *left, Expression *right) : left(left), right(right) {
            };

            AndExpression(const AndExpression &other) : left(other.left->clone()), right(other.right->clone()) {
            };

            AndExpression &operator=(const AndExpression &other) {
                if (&other != this) {
                    delete left;
                    delete right;

                    left = other.left->clone();
                    right = other.right->clone();
                }
                return *this;
            }

            ~AndExpression() override {
                delete left;
                delete right;
            }

            AndExpression *clone() const override;

            void accept(Visitor &visitor, Result &context) override;

            Expression &getLeft() const {
                return *left;
            }

            Expression &getRight() const {
                return *right;
            }

        private:
            Expression *left;
            Expression *right;
        };

        class OrExpression : public Expression {
        public:

            OrExpression(Expression *left, Expression *right) : left(left), right(right) {
            };

            OrExpression(const OrExpression &other) : left(other.left->clone()), right(other.right->clone()) {
            };

            OrExpression &operator=(const OrExpression &other) {
                if (&other != this) {
                    delete left;
                    delete right;

                    left = other.left->clone();
                    right = other.right->clone();
                }
                return *this;
            }

            ~OrExpression() override {
                delete left;
                delete right;
            };


            OrExpression *clone() const override;

            void accept(Visitor &visitor, Result &context) override;

            Expression &getLeft() const {
                return *left;
            }

            Expression &getRight() const {
                return *right;
            }

        private:
            Expression *left;
            Expression *right;
        };

        class ArithmeticExpression : public Visitable {
        public:

            virtual ~ArithmeticExpression() = default;


            virtual ArithmeticExpression *clone() const = 0;
        };

        class OperationExpression : public ArithmeticExpression {
        protected:

            OperationExpression(ArithmeticExpression *left, ArithmeticExpression *right) : left(left), right(right) {
            };

            OperationExpression(const OperationExpression &other) : left(other.left), right(other.right) {
            };

            OperationExpression &operator=(const OperationExpression &other) {
                if (&other != this) {
                    delete left;
                    left = other.left;
                    delete right;
                    right = other.right;
                }
                return *this;
            }

            ~OperationExpression() override = default;


        public:

            ArithmeticExpression &getLeft() {
                return *left;
            };

            ArithmeticExpression &getRight() {
                return *right;
            };

        protected:
            ArithmeticExpression *left;
            ArithmeticExpression *right;
        };

        class PlusExpression : public OperationExpression {
        public:

            PlusExpression(ArithmeticExpression *left, ArithmeticExpression *right)
            : OperationExpression(left, right) {
            };

            PlusExpression(const PlusExpression &other) = default;

            PlusExpression &operator=(const PlusExpression &other) {
                if (&other != this) {
                    left = other.left;
                    right = other.right;
                }
                return *this;
            }

            ~PlusExpression() override = default;


            PlusExpression *clone() const override;

            void accept(Visitor &visitor, Result &context) override;

        };

        class SubtractExpression : public OperationExpression {
        public:

            SubtractExpression(ArithmeticExpression *left, ArithmeticExpression *right)
            : OperationExpression(left, right) {
            };

            SubtractExpression(const SubtractExpression &other) = default;

            SubtractExpression &operator=(const SubtractExpression &other) {
                if (&other != this) {
                    left = other.left;
                    right = other.right;
                }
                return *this;
            }

            ~SubtractExpression() override = default;


            SubtractExpression *clone() const override;

            void accept(Visitor &visitor, Result &context) override;
        };

        class MinusExpression : public ArithmeticExpression {
        public:

            explicit MinusExpression(ArithmeticExpression *value) : value(value) {
            };

            MinusExpression(const MinusExpression &other)
            : value(other.value) {
            };

            MinusExpression &operator=(const MinusExpression &other) {
                if (&other != this) {
                    value = other.value;
                }
                return *this;
            }

            ArithmeticExpression &getValue() const {
                return *value;
            };

            ~MinusExpression() override = default;


            MinusExpression *clone() const override;

            void accept(Visitor &visitor, Result &context) override;

        private:
            ArithmeticExpression *value;
        };

        class MultiplyExpression : public OperationExpression {
        public:

            MultiplyExpression(ArithmeticExpression *left, ArithmeticExpression *right)
            : OperationExpression(left, right) {
            };

            MultiplyExpression(const MultiplyExpression &other)
            = default;

            MultiplyExpression &operator=(const MultiplyExpression &other) {
                if (&other != this) {
                    left = other.left;
                    right = other.right;
                }
                return *this;
            }

            ~MultiplyExpression() override = default;


            MultiplyExpression *clone() const override;

            void accept(Visitor &visitor, Result &context) override;
        };

        class NumberExpression : public ArithmeticExpression {
        public:

            explicit NumberExpression(int i) : value(i) {
            }

            NumberExpression(const NumberExpression &other) : value(other.value) {
            };

            NumberExpression &operator=(const NumberExpression &other) {
                value = other.value;
                return *this;
            };

            int getValue() const {
                return value;
            };

            ~NumberExpression() override = default;


            NumberExpression *clone() const override;

            void accept(Visitor &visitor, Result &context) override;

        private:
            int value;
        };

        class IdentifierExpression : public ArithmeticExpression {
        public:

            explicit IdentifierExpression(int placeIndex) : place(placeIndex) {
            }

            IdentifierExpression(const IdentifierExpression &other) : place(other.place) {
            };

            IdentifierExpression &operator=(const IdentifierExpression &other) {
                place = other.place;
                return *this;
            };

            int getPlace() const {
                return place;
            };

            ~IdentifierExpression() override = default;


            IdentifierExpression *clone() const override;

            void accept(Visitor &visitor, Result &context) override;

        private:
            int place;
        };

        enum BoundType { None, TimeBound, StepsBound };
        class RunBound  {
        public:
            RunBound() : _type(None) , _bound(0) {}
            RunBound(const BoundType type, const int bound) :_type(type), _bound(bound) {}
            BoundType getType() const {
                return _type;
            }
            void setValue(const int new_bound) {
                _bound = new_bound;
            }
            int getValue() const {
                return _bound;
            }
        private:
            BoundType _type;
            int _bound;
        };

        // EF : Reachability
        // AG : Safety
        // EG : Preservability
        // AF : Liveness
        // CF : Control liveness
        // CG : Control Safety
        // PF : Probability Finally
        // PG : Probability Globally
        enum Quantifier {
            EF, AG, EG, AF, CF, CG, PF, PG
        };

        class Query : public Visitable {
        public:

            Query(Quantifier quantifier, Expression *expr) : quantifier(quantifier), expr(expr) {
            };

            Query(const Query &other) : quantifier(other.quantifier), expr(other.expr->clone()) {
            };

            Query &operator=(const Query &other) {
                if (&other != this) {
                    delete expr;
                    expr = other.expr->clone();
                }
                return *this;
            }

            virtual ~Query() {
                delete expr;
            }

            virtual Query *clone() const;

            void accept(Visitor &visitor, Result &context) override;

            Quantifier getQuantifier() const {
                return quantifier;
            }

            const Expression &getConstChild() const {
                return *expr;
            }

            Expression *getChild() {
                return expr;
            }

            void setChild(Expression *expr) {
                this->expr = expr;
            }

            void setQuantifier(Quantifier q) {
                quantifier = q;
            }

            void setRunBound(RunBound bound) {
                runBound = bound;
            }

            RunBound& getRunBound() {
                return runBound;
            }

        private:
            Quantifier quantifier;
            Expression *expr;
            RunBound runBound;
        };

        std::unique_ptr<Query> toAST(const unfoldtacpn::PQL::Condition_ptr& ptr, const TAPN::TimedArcPetriNet& tapn);

    }
}

#endif /* AST_HPP_ */
