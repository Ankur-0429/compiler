#include "tokenization.hpp"
#include <iostream>

Tokenizer::Tokenizer(std::string &src) : m_src(src) {}

std::vector<Token> Tokenizer::tokenize() {
    std::string buf;
    std::vector<Token> tokens;
    while (peak().has_value()) {
        if (isalpha(peak().value())) {
            buf.push_back(consume());
            while (peak().has_value() && isalnum(peak().value())) {
                buf.push_back(consume());
            }
            if (buf == "exit") {
                tokens.push_back({.type = TokenType::exit});
                buf.clear();
                continue;
            } else {
                std::cerr << "exit keyword not found" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else if (std::isdigit(peak().value())) {
            buf.push_back(consume());
            while (peak().has_value() && std::isdigit(peak().value())) {
                buf.push_back(consume());
            }
            tokens.push_back({.type = TokenType::integer_literal, .value = buf});
            buf.clear();
            continue;
        } else if (peak().value() == ';') {
            consume();
            tokens.push_back({.type = TokenType::semi_colon});
            continue;
        } else if (std::isspace(peak().value())) {
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

std::optional<char> Tokenizer::peak(int characters_ahead) const {
    if (m_current_index + 1 > m_src.length()) {
        return {};
    }
    return m_src.at(m_current_index);
}

char Tokenizer::consume() {
    return m_src.at(m_current_index++);
}