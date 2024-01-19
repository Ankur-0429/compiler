#pragma once
#include <vector>
#include "tokenization.h"
#include "arenaAllocator.h"
#include <variant>

struct NodeTermIntegerLiteral {
    Token integer_literal;
};

struct NodeTermIdentifier {
    Token identifier;
};

struct NodeExpression;

struct NodeTermParenthesis {
    NodeExpression* expr;
};

struct NodeBinaryExpressionAdd {
    NodeExpression* lhs;
    NodeExpression* rhs;
};

struct NodeBinaryExpressionMultiplication {
    NodeExpression* lhs;
    NodeExpression* rhs;
};

struct NodeBinaryExpressionSubtraction {
    NodeExpression* lhs;
    NodeExpression* rhs;
};

struct NodeBinaryExpressionDivision {
    NodeExpression* lhs;
    NodeExpression* rhs;
};

struct NodeBinaryExpression {
    std::variant<NodeBinaryExpressionAdd*, NodeBinaryExpressionMultiplication*, NodeBinaryExpressionDivision*, NodeBinaryExpressionSubtraction*> var;
};

struct NodeTerm {
    std::variant<NodeTermIntegerLiteral*, NodeTermIdentifier*, NodeTermParenthesis*> var;
};

struct NodeExpression {
    std::variant<NodeTerm*, NodeBinaryExpression*> var;
};

struct NodeStatementExit {
    NodeExpression* expr;
};

struct NodeStatementUInt32 {
    Token Identifier;
    NodeExpression* expr{};
};

struct NodeStatement {
    std::variant<NodeStatementExit*, NodeStatementUInt32*> var;
};

struct NodeProgram {
    std::vector<NodeStatement*> statement;
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens):
    m_tokens(tokens),
    m_allocator(1024*1024*4){}
    std::optional<NodeExpression*> parse_expr(int min_prec = 0);
    std::optional<NodeTerm*> parse_term();
    std::optional<NodeStatement*> parse_statement();
    std::optional<NodeProgram> parse_program();
private:
    const std::vector<Token> m_tokens;
    size_t m_current_index{};
    ArenaAllocator m_allocator;

    [[nodiscard]] std::optional<Token> peek(int offset = 0) const;
    Token consume();
};
