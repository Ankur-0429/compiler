#include "parser.h"
#include <iostream>

std::optional<Token> Parser::peek(int offset) const {
    if (m_current_index + offset >= m_tokens.size()) {
        return {};
    }
    return m_tokens.at(m_current_index + offset);
}

Token Parser::consume() {
    return m_tokens.at(m_current_index++);
}

std::optional<NodeExpression*> Parser::parse_expr() {
    if (peek().has_value() && peek().value().type == TokenType::integer_literal) {
        auto node_expr_integer_literal = m_allocator.allocate<NodeExpressionIntegerLiteral>();
        node_expr_integer_literal->integer_literal = consume();
        auto node_expression = m_allocator.allocate<NodeExpression>();
        node_expression->variant = node_expr_integer_literal;
        return node_expression;
    } else if (peek().has_value() && peek().value().type == TokenType::identifier) {
        auto expression_identifier = m_allocator.allocate<NodeExpressionIdentifier>();
        expression_identifier->identifier = consume();
        auto expr = m_allocator.allocate<NodeExpression>();
        expr->variant = expression_identifier;
        return expr;
    }
    return {};
}

std::optional<NodeStatement*> Parser::parse_statement() {
    if (peek().value().type == TokenType::exit && peek(1).has_value() && peek(1).value().type == TokenType::open_parenthesis) {
        consume();
        consume();

        auto node_statement_exit = m_allocator.allocate<NodeStatementExit>();
        if (auto node_expr = parse_expr()) {
            node_statement_exit->expr = node_expr.value();
        } else {
            std::cerr << "Invalid expression parse" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (peek().has_value() && peek().value().type == TokenType::closed_parenthesis) {
            consume();
        } else {
            std::cerr << "Expected closed parenthesis in exit parsing" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (peek().has_value() && peek().value().type == TokenType::semi_colon) {
            consume();
        }
        else {
            std::cerr << "Invalid expression, semi colon not found or expected" << std::endl;
            exit(EXIT_FAILURE);
        }

        auto node_statement = m_allocator.allocate<NodeStatement>();
        node_statement->var = node_statement_exit;
        return node_statement;
    } else if (peek().has_value() && peek().value().type == TokenType::let) {
        if (peek(1).has_value() && peek(1).value().type == TokenType::identifier) {
            if (peek(2).has_value() && peek(2).value().type == TokenType::equals) {
                consume();

                auto statement_let = m_allocator.allocate<NodeStatementLet>();
                statement_let->Identifier = consume();

                consume();

                if (auto expr = parse_expr()) {
                    statement_let->expr = expr.value();
                } else {
                    std::cerr << "Invalid expression parsing" << std::endl;
                    exit(EXIT_FAILURE);
                }

                if (peek().has_value() && peek().value().type == TokenType::semi_colon) {
                    consume();
                } else {
                    std::cerr << "Expected semi-colon" << std::endl;
                    exit(EXIT_FAILURE);
                }

                auto node_statement = m_allocator.allocate<NodeStatement>();
                node_statement->var = statement_let;
                return node_statement;
            }
        }
    }
    return {};
}

std::optional<NodeProgram> Parser::parse_program() {
    NodeProgram program;
    while (peek().has_value()) {
        if (auto statement = parse_statement()) {
            program.statement.push_back(statement.value());
        } else {
            std::cerr << "parsing statement failed in program" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    return program;
}
