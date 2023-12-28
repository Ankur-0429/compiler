#pragma once
#include <vector>
#include "tokenization.h"

struct NodeExpr {
    Token integer_literal;
};
struct NodeExit {
    NodeExpr expr;
};

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens): m_tokens(tokens) {}
    std::optional<NodeExit> parse();
    std::optional<NodeExpr> parse_expr();
private:
    const std::vector<Token> m_tokens;
    size_t m_current_index{};

    [[nodiscard]] std::optional<Token> peak() const;
    Token consume();
};
