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

std::optional<NodeScope*> Parser::parse_scope() {
    if (peek().has_value() && peek().value().type != TokenType::open_curly) {
        return {};
    }
    consume();

    auto scope = m_allocator.allocate<NodeScope>();
    while (auto statement = parse_statement()) {
        scope->statements.push_back(statement.value());
    }

    if (peek().has_value() && peek().value().type != TokenType::closed_curly) {
        return {};
    }
    consume();

    return scope;
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

        switch (current_token->type) {
            case TokenType::greater_than:
            case TokenType::greater_than_or_equal_to:
            case TokenType::less_than:
            case TokenType::less_than_or_equal_to:
                prec = 0;
                break;
            case TokenType::plus:
            case TokenType::sub:
                prec = 1;
                break;
            case TokenType::star:
            case TokenType::div:
                prec = 2;
                break;
            default:
                break;
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

        auto createBinaryNode = [&](auto* binaryNode) {
            node_expression->var = expr_lhs->var;
            binaryNode->lhs = node_expression;
            binaryNode->rhs = expr_rhs.value();
            node_binary_expression->var = binaryNode;
        };

        switch (op.type) {
            case TokenType::plus:
                createBinaryNode(m_allocator.allocate<NodeBinaryExpressionAdd>());
                break;
            case TokenType::star:
                createBinaryNode(m_allocator.allocate<NodeBinaryExpressionMultiplication>());
                break;
            case TokenType::div:
                createBinaryNode(m_allocator.allocate<NodeBinaryExpressionDivision>());
                break;
            case TokenType::sub:
                createBinaryNode(m_allocator.allocate<NodeBinaryExpressionSubtraction>());
                break;
            case TokenType::greater_than:
                createBinaryNode(m_allocator.allocate<NodeBinaryExpressionGreaterThan>());
                break;
            case TokenType::greater_than_or_equal_to:
                createBinaryNode(m_allocator.allocate<NodeBinaryExpressionGreaterThanOrEqualTo>());
                break;
            case TokenType::less_than:
                createBinaryNode(m_allocator.allocate<NodeBinaryExpressionLessThan>());
                break;
            case TokenType::less_than_or_equal_to:
                createBinaryNode(m_allocator.allocate<NodeBinaryExpressionLessThanOrEqualTo>());
                break;
            default:
                std::cerr << "Binary expression parsing failed" << std::endl;
                exit(EXIT_FAILURE);
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
    } else if (peek().has_value() && peek().value().type == TokenType::open_curly) {
        if (auto scope = parse_scope()) {
            auto statement = m_allocator.allocate<NodeStatement>();
            statement->var = scope.value();
            return statement;
        } else {
            std::cerr << "Invalid expression" << std::endl;
            exit(EXIT_FAILURE);
        }
    } else if (peek().has_value() && peek().value().type == TokenType::if_) {
        consume();

        if (peek().has_value() && peek().value().type != TokenType::open_parenthesis) {
            std::cerr << "Expected (" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        auto statementIf = m_allocator.allocate<NodeStatementIf>();

        if (auto expr = parse_expr()) {
            statementIf->expr = expr.value();
        } else {
            std::cerr << "Invalid expression" << std::endl;
            exit(EXIT_FAILURE);
        }

        if (peek().has_value() && peek().value().type != TokenType::closed_parenthesis) {
            std::cerr << "Expected )" << std::endl;
            exit(EXIT_FAILURE);
        }
        consume();

        if (auto scope = parse_scope()) {
            statementIf->scope = scope.value();
        } else {
            std::cerr << "Invalid scope" << std::endl;
            exit(EXIT_FAILURE);
        }

        auto statement = m_allocator.allocate<NodeStatement>();
        statement->var = statementIf;
        return statement;
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
