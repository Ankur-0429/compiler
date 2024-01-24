#include "../include/tokenization.h"
#include <iostream>

Tokenizer::Tokenizer(std::string &src) : m_src(src) {}

Token Tokenizer::tokenizeBuffer() {
    std::string buf;
    buf.push_back(consume());

    while (peek().has_value() && std::isalnum(peek().value())) {
        buf.push_back(consume());
    }

    if (buf == "exit") {
        return {.type = TokenType::exit};
    } else if (buf == "uint32") {
        return {.type = TokenType::uint32, .value = buf};
    } else if (buf == "if") {
        return {.type = TokenType::if_};
    } else {
        return {.type = TokenType::identifier, .value = buf};
    }
}

Token Tokenizer::tokenizeIntegerLiteral() {
    std::string buf;
    buf.push_back(consume());

    while (peek().has_value() && std::isdigit(peek().value())) {
        buf.push_back(consume());
    }

    return {.type = TokenType::integer_literal, .value = buf};
}

std::optional<Token> Tokenizer::tokenizeChar() {
    auto currentChar = peek().value();

    auto consumeAndAdd = [this](TokenType type) {
        consume();
        return Token{.type = type};
    };

    switch (currentChar) {
        case '(':
            return consumeAndAdd(TokenType::open_parenthesis);
        case ')':
            return consumeAndAdd(TokenType::closed_parenthesis);
        case ';':
            return consumeAndAdd(TokenType::semi_colon);
        case '=':
            return consumeAndAdd(TokenType::equals);
        case '+':
            return consumeAndAdd(TokenType::plus);
        case '-':
            return consumeAndAdd(TokenType::sub);
        case '*':
            return consumeAndAdd(TokenType::star);
        case '/':
            return consumeAndAdd(TokenType::div);
        case '{':
            return consumeAndAdd(TokenType::open_curly);
        case '}':
            return consumeAndAdd(TokenType::closed_curly);
        case '>':
            if (peek(1).has_value() && peek(1).value() == '=') {
                consume();
                consume();
                return Token{.type = TokenType::greater_than_or_equal_to};
            }
            return consumeAndAdd(TokenType::greater_than);
        case '<':
            if (peek(1).has_value() && peek(1).value() == '=') {
                consume();
                consume();
                return Token{.type = TokenType::less_than_or_equal_to};
            }
            return consumeAndAdd(TokenType::less_than);
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            consume();
            return {};
        default:
            std::cerr << "Tokenization failed" << std::endl;
            exit(EXIT_FAILURE);
    }
}

std::vector<Token> Tokenizer::tokenize() {
    std::string buf;
    std::vector<Token> tokens;
    while (peek().has_value()) {
        char currentChar = peek().value();

        if (std::isalpha(currentChar)) {
            tokens.push_back(tokenizeBuffer());
            continue;
        }

        if (std::isdigit(currentChar)) {
            tokens.push_back(tokenizeIntegerLiteral());
            continue;
        }

        if (auto characterToken = tokenizeChar()) {
            tokens.push_back(characterToken.value());
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