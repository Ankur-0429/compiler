#include <sstream>
#include <iostream>
#include <cassert>
#include "generation.h"

void Generator::generate_term(const NodeTerm* term) {
    struct TermVisitor {
        Generator& generator;
        explicit TermVisitor(const Generator& gen) : generator(const_cast<Generator &>(gen)) {}
        void operator()(const NodeTermIntegerLiteral* integer_literal) const {
            generator.memory_stack_push(integer_literal->integer_literal.value.value());
        }
        void operator()(const NodeTermIdentifier* identifier) const {
            if (generator.m_vars.find(identifier->identifier.value.value()) == generator.m_vars.end()) {
                std::cerr << "Undeclared Identifier: " << identifier->identifier.value.value() << std::endl;
                exit(EXIT_FAILURE);
            }
            const auto& var = generator.m_vars.at(identifier->identifier.value.value());
            size_t offset = generator.m_stack_size - var.stack_loc;

            generator.m_output_stream << "    ldr x1, [sp, #" << 16*(offset-1) << "]\n";
            generator.memory_stack_push(1);
        }
    };
    TermVisitor visitor(*this);;
    std::visit(visitor, term->var);
}

void Generator::generate_expression(NodeExpression* expression) {

    struct ExpressionVisitor {
        Generator& generator;
        explicit ExpressionVisitor(const Generator& gen) : generator(const_cast<Generator &>(gen)) {}
        void operator()(const NodeTerm* term) {
            generator.generate_term(term);
        }
        void operator()(const NodeBinaryExpression* binary_expr) {
            if (std::holds_alternative<NodeBinaryExpressionAdd*>(binary_expr->var)) {
                NodeBinaryExpressionAdd* add_expr = std::get<NodeBinaryExpressionAdd*>(binary_expr->var);
                generator.generate_expression(add_expr->lhs);
                generator.generate_expression(add_expr->rhs);
                generator.memory_stack_pop(3);
                generator.memory_stack_pop(4);

                generator.m_output_stream << "    add x1, x3, x4\n";
                generator.memory_stack_push(1);
            }
        }
    };

    ExpressionVisitor visitor(*this);;
    std::visit(visitor, expression->variant);
}

void Generator::generate_statement(const NodeStatement* statement) {
    struct StatementVisitor {
        Generator& generator;
        explicit StatementVisitor(const Generator& gen) : generator(const_cast<Generator &>(gen)) {}

        void operator()(const NodeStatementExit* statement_exit) {
            generator.generate_expression(statement_exit->expr);
            generator.memory_stack_pop();
            generator.m_output_stream << "    mov x0, x2\n";
            generator.m_output_stream << "    mov x16, #1\n";
            generator.m_output_stream << "    svc 0\n";
        }
        void operator()(const NodeStatementLet* statement_let) {
            if (generator.m_vars.find(statement_let->Identifier.value.value()) != generator.m_vars.end()) {
                std::cerr << "Identifier already used: " << statement_let->Identifier.value.value() << "\n";
                exit(EXIT_FAILURE);
            }

            generator.m_vars.insert({statement_let->Identifier.value.value(), Var {.stack_loc = generator.m_stack_size}});
            generator.generate_expression(statement_let->expr);
        }
    };

    StatementVisitor visitor(*this);
    std::visit(visitor, statement->var);
}

std::string Generator::generate_program() {
    m_output_stream << ".global _main\n.align 2\n_main:\n";

    for (const NodeStatement* statement : m_program.statement) {
        generate_statement(statement);
    }

    m_output_stream << "    mov x0, #0\n";
    m_output_stream << "    mov x16, #1\n";
    m_output_stream << "    svc 0";

    return m_output_stream.str();
}
