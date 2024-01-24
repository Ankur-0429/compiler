#pragma once

#include <string>
#include <vector>
#include <optional>

enum class TokenType {
    exit,
    integer_literal,
    semi_colon,
    open_parenthesis,
    closed_parenthesis,
    identifier,
    uint32,
    equals,
    plus,
    star,
    sub,
    div,
    open_curly,
    closed_curly,
    if_,
    greater_than,
    greater_than_or_equal_to,
};

struct Token {
    TokenType type;
    std::optional<std::string> value {};
};

class Tokenizer {
public:
    explicit Tokenizer(std::string& src);
    std::vector<Token> tokenize();

private:
    const std::string m_src;
    size_t m_current_index{};

    Token tokenizeBuffer();
    Token tokenizeIntegerLiteral();
    std::optional<Token> tokenizeChar();

    [[nodiscard]] std::optional<char> peek(int offset = 0) const;
    char consume();
};