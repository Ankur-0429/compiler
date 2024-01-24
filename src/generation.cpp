#include <sstream>
#include <iostream>
#include <cassert>
#include "../include/generation.h"

void Generator::push_scope() {
    llvm::BasicBlock* scopeBlock = llvm::BasicBlock::Create(m_context, "scope" + std::to_string(m_scopeStack.size()), m_builder.GetInsertBlock()->getParent());
    m_scopeStack.push_back(scopeBlock);
    m_builder.CreateBr(scopeBlock);
    m_builder.SetInsertPoint(scopeBlock);
}

void Generator::pop_scope() {
    if (!m_scopeStack.empty()) {
        llvm::BasicBlock* currentScope = m_scopeStack.back();
        m_scopeStack.pop_back();

        std::string continuationName = "continuation" + std::to_string(m_scopeStack.size());
        llvm::BasicBlock* continuationBlock = llvm::BasicBlock::Create(m_context, continuationName, currentScope->getParent());

        m_builder.CreateBr(continuationBlock);
        m_builder.SetInsertPoint(continuationBlock);
    } else {
        std::cerr << "Error: Attempted to pop an empty scope stack." << std::endl;
        exit(EXIT_FAILURE);
    }
}

llvm::Value* Generator::generate_term(const NodeTerm* term) {
    struct TermVisitor {
        Generator& generator;
        mutable llvm::Value* result{};
        explicit TermVisitor(const Generator& gen) : generator(const_cast<Generator &>(gen)) {}
        void operator()(const NodeTermIntegerLiteral* integer_literal) const {
            result = generator.m_builder.getInt32(std::stoi(integer_literal->integer_literal.value.value()));
        }
        void operator()(const NodeTermIdentifier* identifier) const {
            for (llvm::BasicBlock* scopeBlock : generator.m_scopeStack) {
                auto it = std::find_if(generator.m_vars.cbegin(), generator.m_vars.cend(), [&](const Var& var) {
                    return (var.block == scopeBlock) && (var.name == identifier->identifier.value.value());
                });

                if (it != generator.m_vars.cend()) {
                    result = it->value;
                    return;
                }
            }

            std::cerr << "Undeclared Identifier: " << identifier->identifier.value.value() << std::endl;
            exit(EXIT_FAILURE);
        }
        void operator()(const NodeTermParenthesis* parenthesis) const {
            result = generator.generate_expression(parenthesis->expr);
        }
    };
    TermVisitor visitor(*this);;
    std::visit(visitor, term->var);
    return visitor.result;
}

llvm::Value* Generator::generate_binary_expression(const NodeBinaryExpression* binary_expression) {
    struct BinaryExpressionVisitor {
        Generator& generator;
        llvm::Value* result{};
        explicit BinaryExpressionVisitor(const Generator& gen) : generator(const_cast<Generator &>(gen)) {}

        void operator()(const NodeBinaryExpressionAdd* add_expr) {
            llvm::Value* lhsValue = generator.generate_expression(add_expr->lhs);
            llvm::Value* rhsValue = generator.generate_expression(add_expr->rhs);

            llvm::Value* resultValue = generator.m_builder.CreateAdd(lhsValue, rhsValue);
            result = resultValue;
        }

        void operator()(const NodeBinaryExpressionMultiplication* mult_expr) {
            llvm::Value* lhsValue = generator.generate_expression(mult_expr->lhs);
            llvm::Value* rhsValue = generator.generate_expression(mult_expr->rhs);

            llvm::Value* resultValue = generator.m_builder.CreateMul(lhsValue, rhsValue);
            result = resultValue;
        }

        void operator()(const NodeBinaryExpressionSubtraction* mult_expr) {
            llvm::Value* lhsValue = generator.generate_expression(mult_expr->lhs);
            llvm::Value* rhsValue = generator.generate_expression(mult_expr->rhs);

            llvm::Value* resultValue = generator.m_builder.CreateSub(lhsValue, rhsValue);
            result = resultValue;
        }

        void operator()(const NodeBinaryExpressionDivision* mult_expr) {
            llvm::Value* lhsValue = generator.generate_expression(mult_expr->lhs);
            llvm::Value* rhsValue = generator.generate_expression(mult_expr->rhs);

            llvm::Value* resultValue = generator.m_builder.CreateSDiv(lhsValue, rhsValue);
            result = resultValue;
        }

        void operator()(const NodeBinaryExpressionGreaterThan* greater_than) {
            llvm::Value* lhsValue = generator.generate_expression(greater_than->lhs);
            llvm::Value* rhsValue = generator.generate_expression(greater_than->rhs);

            llvm::Value* resultValue = generator.m_builder.CreateICmpSGT(lhsValue, rhsValue);
            result = resultValue;
        }

        void operator()(const NodeBinaryExpressionGreaterThanOrEqualTo* greater_than_or_equal_to) {
            llvm::Value* lhsValue = generator.generate_expression(greater_than_or_equal_to->lhs);
            llvm::Value* rhsValue = generator.generate_expression(greater_than_or_equal_to->rhs);

            llvm::Value* resultValue = generator.m_builder.CreateICmpSGE(lhsValue, rhsValue);
            result = resultValue;
        }

        void operator()(const NodeBinaryExpressionLessThan* less_than) {
            llvm::Value* lhsValue = generator.generate_expression(less_than->lhs);
            llvm::Value* rhsValue = generator.generate_expression(less_than->rhs);

            llvm::Value* resultValue = generator.m_builder.CreateICmpSLT(lhsValue, rhsValue);
            result = resultValue;
        }

        void operator()(const NodeBinaryExpressionLessThanOrEqualTo* less_than_or_equal_to) {
            llvm::Value* lhsValue = generator.generate_expression(less_than_or_equal_to->lhs);
            llvm::Value* rhsValue = generator.generate_expression(less_than_or_equal_to->rhs);

            llvm::Value* resultValue = generator.m_builder.CreateICmpSLE(lhsValue, rhsValue);
            result = resultValue;
        }

        void operator()(const NodeBinaryExpressionEquals* equals) {
            llvm::Value* lhsValue = generator.generate_expression(equals->lhs);
            llvm::Value* rhsValue = generator.generate_expression(equals->rhs);

            llvm::Value* resultValue = generator.m_builder.CreateICmpEQ(lhsValue, rhsValue);
            result = resultValue;
        }

        void operator()(const NodeBinaryExpressionNotEquals* not_equals) {
            llvm::Value* lhsValue = generator.generate_expression(not_equals->lhs);
            llvm::Value* rhsValue = generator.generate_expression(not_equals->rhs);

            llvm::Value* resultValue = generator.m_builder.CreateICmpNE(lhsValue, rhsValue);
            result = resultValue;
        }
    };

    BinaryExpressionVisitor visitor(*this);
    std::visit(visitor, binary_expression->var);
    return visitor.result;
}

llvm::Value* Generator::generate_expression(NodeExpression* expression) {
    struct ExpressionVisitor {
        Generator& generator;
        llvm::Value* result{};
        explicit ExpressionVisitor(const Generator& gen) : generator(const_cast<Generator &>(gen)) {}

        void operator()(const NodeTerm* term) {
            result = generator.generate_term(term);
        }

        void operator()(const NodeBinaryExpression* binary_expr) {
            result = generator.generate_binary_expression(binary_expr);
        }
    };

    ExpressionVisitor visitor(*this);
    std::visit(visitor, expression->var);
    return visitor.result;
}

void Generator::generate_statement(const NodeStatement* statement) {
    struct StatementVisitor {
        Generator& generator;
        explicit StatementVisitor(const Generator& gen) : generator(const_cast<Generator &>(gen)) {}

        void operator()(const NodeStatementExit* statement_exit) {
            llvm::Value* exitValue = generator.generate_expression(statement_exit->expr);
            llvm::FunctionType* exitFuncType = llvm::FunctionType::get(generator.m_builder.getInt32Ty(), {generator.m_builder.getInt32Ty()}, false);
            llvm::FunctionCallee exitFunc = generator.m_module.getOrInsertFunction("exit", exitFuncType);
            generator.m_builder.CreateCall(exitFunc, {exitValue});
        }
        void operator()(const NodeStatementUInt32* statement_uint32) {
            auto it = std::find_if(generator.m_vars.cbegin(), generator.m_vars.cend(), [&](const Var& var) {
                return var.name == statement_uint32->Identifier.value.value();
            });
            if (it != generator.m_vars.cend()) {
                std::cerr << "Identifier already used: " << statement_uint32->Identifier.value.value() << "\n";
                exit(EXIT_FAILURE);
            }

            llvm::Value* letValue = generator.generate_expression(statement_uint32->expr);

            if (!letValue->getType()->isIntegerTy(32)) {
                std::cerr << "Error: Variable type must be an unsigned 32-bit integer.\n";
                exit(EXIT_FAILURE);
            }

            generator.m_vars.push_back({statement_uint32->Identifier.value.value(), letValue, generator.m_builder.GetInsertBlock()});
        }
        void operator()(const NodeScope* statement_scope) {
            generator.push_scope();
            for (const NodeStatement *statement : statement_scope->statements) {
                generator.generate_statement(statement);
            }
            generator.pop_scope();
        }
        void operator()(const NodeStatementIf* statementIf) {
            llvm::Function* currentFunction = generator.m_builder.GetInsertBlock()->getParent();
            llvm::BasicBlock* ifBlock = llvm::BasicBlock::Create(generator.m_context, "ifblock", currentFunction);
            llvm::BasicBlock* endBlock = llvm::BasicBlock::Create(generator.m_context, "endblock", currentFunction);

            llvm::Value* conditionValue = generator.generate_expression(statementIf->expr);

            if (!conditionValue->getType()->isIntegerTy(1)) {
                std::cerr << "Error: Condition must evaluate to a boolean value." << std::endl;
                exit(EXIT_FAILURE);
            }

            generator.m_builder.CreateCondBr(conditionValue, ifBlock, endBlock);

            generator.m_builder.SetInsertPoint(ifBlock);

            for (auto statement : statementIf->scope->statements) {
                generator.generate_statement(statement);
            }

            generator.m_builder.CreateBr(endBlock);
            generator.m_builder.SetInsertPoint(endBlock);
        }
    };

    StatementVisitor visitor(*this);
    std::visit(visitor, statement->var);
}

std::string Generator::generate_program() {
    llvm::FunctionType* mainFuncType = llvm::FunctionType::get(m_builder.getInt32Ty(), false);
    llvm::Function* mainFunc = llvm::Function::Create(mainFuncType, llvm::Function::ExternalLinkage, "main", &m_module);
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(m_context, "entry", mainFunc);
    m_scopeStack.push_back(entryBlock);
    m_builder.SetInsertPoint(entryBlock);

    for (const NodeStatement* statement : m_program.statement) {
        generate_statement(statement);
    }

    m_builder.CreateRet(m_builder.getInt32(0));

    std::string irCode;
    llvm::raw_string_ostream irStream(irCode);
    m_module.print(irStream, nullptr);

    return irCode;
}
