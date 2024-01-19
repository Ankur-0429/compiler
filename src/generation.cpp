#include <sstream>
#include <iostream>
#include <cassert>
#include "../include/generation.h"

llvm::Value* Generator::generate_term(const NodeTerm* term) {
    struct TermVisitor {
        Generator& generator;
        mutable llvm::Value* result{};
        explicit TermVisitor(const Generator& gen) : generator(const_cast<Generator &>(gen)) {}
        void operator()(const NodeTermIntegerLiteral* integer_literal) const {
            result = generator.m_builder.getInt32(std::stoi(integer_literal->integer_literal.value.value()));
        }
        void operator()(const NodeTermIdentifier* identifier) const {
            if (generator.m_vars.find(identifier->identifier.value.value()) == generator.m_vars.end()) {
                std::cerr << "Undeclared Identifier: " << identifier->identifier.value.value() << std::endl;
                exit(EXIT_FAILURE);
            }
            result = generator.m_vars.at(identifier->identifier.value.value());
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
            generator.m_builder.CreateStore(exitValue, generator.m_builder.CreateAlloca(exitValue->getType()));
            llvm::FunctionType* exitFuncType = llvm::FunctionType::get(generator.m_builder.getInt32Ty(), false);
            llvm::FunctionCallee exitFunc = generator.m_module.getOrInsertFunction("exit", exitFuncType);
            generator.m_builder.CreateCall(exitFunc, {exitValue});
        }
        void operator()(const NodeStatementLet* statement_let) {
            if (generator.m_vars.find(statement_let->Identifier.value.value()) != generator.m_vars.end()) {
                std::cerr << "Identifier already used: " << statement_let->Identifier.value.value() << "\n";
                exit(EXIT_FAILURE);
            }

            llvm::Value* letValue = generator.generate_expression(statement_let->expr);
            generator.m_vars.insert({statement_let->Identifier.value.value(), letValue});
        }
    };

    StatementVisitor visitor(*this);
    std::visit(visitor, statement->var);
}

std::string Generator::generate_program() {
    llvm::FunctionType* mainFuncType = llvm::FunctionType::get(m_builder.getInt32Ty(), false);
    llvm::Function* mainFunc = llvm::Function::Create(mainFuncType, llvm::Function::ExternalLinkage, "main", &m_module);
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(m_context, "entry", mainFunc);
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
