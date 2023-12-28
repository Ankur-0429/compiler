#include "parser.h"
#include <iostream>

std::optional<Token> Parser::peek() const {
    if (m_current_index + 1 > m_tokens.size()) {
        return {};
    }
    return m_tokens.at(m_current_index);
}

Token Parser::consume() {
    return m_tokens.at(m_current_index++);
}

std::optional<NodeExpr> Parser::parse_expr() {
    if (peek().has_value() && peek().value().type == TokenType::integer_literal) {
        return NodeExpr{.integer_literal = consume()};
    }
    return {};
}

std::optional<NodeExit> Parser::parse() {

    std::optional<NodeExit> exit_node;
    while (peek().has_value()) {
        if (peek().value().type == TokenType::exit) {
            consume();
            if (auto node_expr = parse_expr()) {
                exit_node = NodeExit {.expr = node_expr.value()};
            } else {
                std::cerr << "Invalid expression parse" << std::endl;
                exit(EXIT_FAILURE);
            }

            if (peek().has_value() && peek().value().type == TokenType::semi_colon) {
                consume();
            }
            else {
                std::cerr << "Invalid expression, semi colon not found or expected" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
    }
    m_current_index = 0;
    return exit_node;
}
