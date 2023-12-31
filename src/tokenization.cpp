#include "tokenization.h"
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
                continue;
            } else if (buf == "let") {
                tokens.push_back({.type = TokenType::let, .value = buf});
                buf.clear();
                continue;
            } else {
                tokens.push_back({.type = TokenType::identifier, .value = buf});
                buf.clear();
                continue;
            }
        } else if (std::isdigit(peek().value())) {
            buf.push_back(consume());
            while (peek().has_value() && std::isdigit(peek().value())) {
                buf.push_back(consume());
            }
            tokens.push_back({.type = TokenType::integer_literal, .value = buf});
            buf.clear();
            continue;
        } else if (peek().value() == '(') {
            consume();
            tokens.push_back({.type = TokenType::open_parenthesis});
            continue;
        } else if (peek().value() == ')') {
            consume();
            tokens.push_back({.type = TokenType::closed_parenthesis});
            continue;
        } else if (peek().value() == ';') {
            consume();
            tokens.push_back({.type = TokenType::semi_colon});
            continue;
        } else if (peek().value() == '=') {
            consume();
            tokens.push_back({.type = TokenType::equals});
            continue;
        } else if (peek().value() == '+') {
            consume();
            tokens.push_back({.type = TokenType::plus});
            continue;
        } else if (std::isspace(peek().value())) {
            consume();
            continue;
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