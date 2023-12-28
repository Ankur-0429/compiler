#pragma once

#include <string>
#include <vector>
#include <optional>

enum class TokenType {
    exit,
    integer_literal,
    semi_colon,
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

    [[nodiscard]] std::optional<char> peek() const;
    char consume();
};