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
    } else if (peek().has_value() && peek().value().type == TokenType::open_parenthesis) {
        consume();

        auto expr = parse_expr();
        if (!expr.has_value()) {
            std::cerr << "expected expression" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (!peek().has_value() || peek().value().type != TokenType::closed_parenthesis) {
            std::cerr << "expected closed parenthesis" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        auto term_parenthesis = m_allocator.allocate<NodeTermParenthesis>();
        term_parenthesis->expr = expr.value();
        auto term = m_allocator.allocate<NodeTerm>();
        term->var = term_parenthesis;
        return term;
    }
    return {};
}

std::optional<NodeExpression*> Parser::parse_expr(int min_prec) {
    std::optional<NodeTerm*> lhs = parse_term();
    if (!lhs.has_value()) {
        return {};
    }
    auto* expr_lhs = m_allocator.allocate<NodeExpression>();
    expr_lhs->var = lhs.value();

    while (true) {
        std::optional<Token> current_token = peek();
        std::optional<int> prec{};
        if (!current_token.has_value()) {
            break;
        }

        if (current_token->type == TokenType::plus || current_token->type == TokenType::sub) {
            prec = 0;
        } else if (current_token->type == TokenType::star || current_token->type == TokenType::div) {
            prec = 1;
        }
        if (!prec.has_value() || prec < min_prec) {
            break;
        }

        Token op = consume();
        int next_minimum_prec = prec.value() + 1;

        std::optional<NodeExpression*> expr_rhs = parse_expr(next_minimum_prec);
        if (!expr_rhs.has_value()) {
            std::cerr << "right expression should have value" << std::endl;
            exit(EXIT_FAILURE);
        }

        auto* node_binary_expression = m_allocator.allocate<NodeBinaryExpression>();
        auto* node_expression = m_allocator.allocate<NodeExpression>();

        if (op.type == TokenType::plus) {
            auto* add = m_allocator.allocate<NodeBinaryExpressionAdd>();
            node_expression->var = expr_lhs->var;
            add->lhs = node_expression;
            add->rhs = expr_rhs.value();
            node_binary_expression->var = add;
        } else if (op.type == TokenType::star) {
            auto* mult = m_allocator.allocate<NodeBinaryExpressionMultiplication>();
            node_expression->var = expr_lhs->var;
            mult->lhs = node_expression;
            mult->rhs = expr_rhs.value();
            node_binary_expression->var = mult;
        } else if (op.type == TokenType::div) {
            auto* div = m_allocator.allocate<NodeBinaryExpressionDivision>();
            node_expression->var = expr_lhs->var;
            div->lhs = node_expression;
            div->rhs = expr_rhs.value();
            node_binary_expression->var = div;
        } else if (op.type == TokenType::sub) {
            auto* sub = m_allocator.allocate<NodeBinaryExpressionSubtraction>();
            node_expression->var = expr_lhs->var;
            sub->lhs = node_expression;
            sub->rhs = expr_rhs.value();
            node_binary_expression->var = sub;
        }

        expr_lhs->var = node_binary_expression;
    }
    return expr_lhs;
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
    } else if (peek().has_value() && peek().value().type == TokenType::uint32) {
        if (peek(1).has_value() && peek(1).value().type == TokenType::identifier) {
            if (peek(2).has_value() && peek(2).value().type == TokenType::equals) {
                consume();

                auto statement_let = m_allocator.allocate<NodeStatementUInt32>();
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
