#pragma once
#include <vector>
#include "tokenization.h"
#include <variant>

struct NodeExpressionIntegerLiteral {
    Token integer_literal;
};

struct NodeExpressionIdentifier {
    Token identifier;
};

struct NodeExpression {
    std::variant<NodeExpressionIntegerLiteral, NodeExpressionIdentifier> variant;
};

struct NodeStatementExit {
    NodeExpression expr;
};

struct NodeStatementLet {
    Token Identifier;
    NodeExpression expr;
};

struct NodeStatement {
    std::variant<NodeStatementExit, NodeStatementLet> var;
};

struct NodeProgram {
    std::vector<NodeStatement> statement;
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens): m_tokens(tokens) {}
    std::optional<NodeExpression> parse_expr();
    std::optional<NodeStatement> parse_statement();
    std::optional<NodeProgram> parse_program();
private:
    const std::vector<Token> m_tokens;
    size_t m_current_index{};

    [[nodiscard]] std::optional<Token> peek(int offset = 0) const;
    Token consume();
};
