#include "../include/parser.h"
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

std::optional<NodeBinaryExpression*> Parser::parse_binary_expr() {
    if (auto lhs = parse_expr()) {
        auto bin_expr = m_allocator.allocate<NodeBinaryExpression>();
        if (peek().has_value() && peek().value().type == TokenType::plus) {
            auto bin_expr_add = m_allocator.allocate<NodeBinaryExpressionAdd>();
            bin_expr_add->lhs = lhs.value();
            consume();

            if (auto rhs = parse_expr()) {
                bin_expr_add->rhs = rhs.value();
                bin_expr->var = bin_expr_add;
                return bin_expr;
            } else {
                std::cerr << "Expected rhs expression" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else {
            std::cerr << "Unsupported binary operator" << std::endl;
            exit(EXIT_FAILURE);
        }

    }

    return {};
}

std::optional<NodeTerm*> Parser::parse_term() {
    if (peek().has_value() && peek().value().type == TokenType::integer_literal) {
        auto node_term_integer_literal = m_allocator.allocate<NodeTermIntegerLiteral>();
        node_term_integer_literal->integer_literal = consume();
        auto node_term = m_allocator.allocate<NodeTerm>();
        node_term->var = node_term_integer_literal;
        return node_term;
    } else if (peek().has_value() && peek().value().type == TokenType::identifier) {
        auto node_term_identifier = m_allocator.allocate<NodeTermIdentifier>();
        node_term_identifier->identifier = consume();
        auto node_term = m_allocator.allocate<NodeTerm>();
        node_term->var = node_term_identifier;
        return node_term;
    }
    return {};
}

std::optional<NodeExpression*> Parser::parse_expr() {
    if (auto term = parse_term()) {
        if (peek().has_value() && peek().value().type == TokenType::plus) {
            auto bin_expr = m_allocator.allocate<NodeBinaryExpression>();
            auto bin_expr_add = m_allocator.allocate<NodeBinaryExpressionAdd>();

            auto lsh_expr = m_allocator.allocate<NodeExpression>();
            lsh_expr->variant = term.value();
            bin_expr_add->lhs = lsh_expr;

            consume();

            if (auto rhs = parse_expr()) {
                bin_expr_add->rhs = rhs.value();
                bin_expr->var = bin_expr_add;
                auto expr = m_allocator.allocate<NodeExpression>();
                expr->variant = bin_expr;
                return expr;
            } else {
                std::cerr << "Expected rhs expression" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        auto expr = m_allocator.allocate<NodeExpression>();
        expr->variant = term.value();
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
