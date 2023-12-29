#include <sstream>
#include <iostream>
#include "generation.h"

 std::string Generator::generate_expression(NodeExpression expression) const {

    struct ExpressionVisitor {
        std::stringstream output_stream;
        void operator()(const NodeExpressionIntegerLiteral& integerLiteral) {
            output_stream << "    mov x1, #" << integerLiteral.integer_literal.value.value() << "\n";
            output_stream << "    sub sp, sp, #16\n";
            output_stream << "    str x1, [sp, #0]\n";
        }
        void operator()(const NodeExpressionIdentifier identifier) {

        }
    };

    ExpressionVisitor visitor;
    std::visit(visitor, expression.variant);
    return visitor.output_stream.str();
}

std::string Generator::generate_statement(const NodeStatement& statement) const {
    struct StatementVisitor {
        std::stringstream output_stream;
        const Generator& generator;
        explicit StatementVisitor(const Generator& gen) : generator(gen) {}

        void operator()(const NodeStatementExit& statement_exit) {
            output_stream << generator.generate_expression(statement_exit.expr);
            output_stream << "    ldr x2, [sp, #0]\n";
            output_stream << "    add sp, sp, #16\n";
            output_stream << "    mov x0, x2\n";
            output_stream << "    mov x16, #1\n";
            output_stream << "    svc 0\n";
        }
        void operator()(const NodeStatementLet statement_let) {

        }
    };

    StatementVisitor visitor(*this);
    std::visit(visitor, statement.var);
    return visitor.output_stream.str();
}

std::string Generator::generate_program() const {
    std::stringstream output_stream;
    output_stream << ".global _main\n.align 2\n_main:\n";

    for (const NodeStatement& statement : m_program.statement) {
        output_stream << generate_statement(statement);
    }

    output_stream << "    mov x0, #0\n";
    output_stream << "    mov x16, #1\n";
    output_stream << "    svc 0";

    return output_stream.str();
}
