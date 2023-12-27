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
    int m_current_index{};

    [[nodiscard]] std::optional<char> peak(int characters_ahead = 1) const;
    char consume();
};