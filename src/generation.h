#pragma once
#include <utility>

#include "parser.h"

class Generator {
public:
    explicit Generator(NodeProgram program): m_program(std::move(program)) {};

    [[nodiscard]] std::string generate_program() const;
    [[nodiscard]] std::string generate_statement(const NodeStatement& statement) const;
    [[nodiscard]] std::string generate_expression(NodeExpression expression) const;
private:
    const NodeProgram m_program;
};
