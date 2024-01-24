#include "../include/tokenization.h"
#include <iostream>

Tokenizer::Tokenizer(std::string &src) : m_src(src) {}

std::vector<Token> Tokenizer::tokenize() {
    std::string buf;
    std::vector<Token> tokens;
    while (peek().has_value()) {
        if (isalpha(peek().value())) {
            buf.push_back(consume());
            while (peek().has_value() && isalnum(peek().value())) {
                buf.push_back(consume());
            }
            if (buf == "exit") {
                tokens.push_back({.type = TokenType::exit});
                buf.clear();
            } else if (buf == "uint32") {
                tokens.push_back({.type = TokenType::uint32, .value = buf});
                buf.clear();
            } else if (buf == "if") {
                tokens.push_back({.type = TokenType::if_});
                buf.clear();
            } else {
                tokens.push_back({.type = TokenType::identifier, .value = buf});
                buf.clear();
            }
        } else if (std::isdigit(peek().value())) {
            buf.push_back(consume());
            while (peek().has_value() && std::isdigit(peek().value())) {
                buf.push_back(consume());
            }
            tokens.push_back({.type = TokenType::integer_literal, .value = buf});
            buf.clear();
        } else if (peek().value() == '(') {
            consume();
            tokens.push_back({.type = TokenType::open_parenthesis});
        } else if (peek().value() == ')') {
            consume();
            tokens.push_back({.type = TokenType::closed_parenthesis});
        } else if (peek().value() == ';') {
            consume();
            tokens.push_back({.type = TokenType::semi_colon});
        } else if (peek().value() == '=') {
            consume();
            tokens.push_back({.type = TokenType::equals});
        } else if (peek().value() == '+') {
            consume();
            tokens.push_back({.type = TokenType::plus});
        } else if (std::isspace(peek().value())) {
            consume();
        } else if (peek().value() == '*') {
            consume();
            tokens.push_back({.type = TokenType::star});
        } else if (peek().value() == '-') {
            consume();
            tokens.push_back({.type = TokenType::sub});
        } else if (peek().value() == '/') {
            consume();
            tokens.push_back({.type = TokenType::div});
        } else if (peek().value() == '{') {
            consume();
            tokens.push_back({.type = TokenType::open_curly});
        } else if (peek().value() == '}') {
            consume();
            tokens.push_back({.type = TokenType::closed_curly});
        } else if (peek().value() == '>' && peek(1).value() == '=') {
            consume();
            consume();
            tokens.push_back({.type = TokenType::greater_than_or_equal_to});
        } else if (peek().value() == '>') {
            consume();
            tokens.push_back({.type = TokenType::greater_than});
        } else {
            std::cerr << "tokenization failed" << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    m_current_index = 0;

    return tokens;
}

std::optional<char> Tokenizer::peek(int offset) const {
    if (m_current_index + offset >= m_src.length()) {
        return {};
    }
    return m_src.at(m_current_index + offset);
}

char Tokenizer::consume() {
    return m_src.at(m_current_index++);
}