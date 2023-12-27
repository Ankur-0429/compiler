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
    inline explicit Tokenizer(std::string& src):m_src(src){}

    inline std::vector<Token> tokenize() {
        std::string buf;
        std::vector<Token> tokens;
        while (peak().has_value()) {
            if (isalpha(peak().value())) {
                buf.push_back(consume());
                while(peak().has_value() && isalnum(peak().value())) {
                    buf.push_back(consume());
                }
                if (buf == "exit") {
                    tokens.push_back({.type = TokenType::exit});
                    buf.clear();
                    continue;
                }
                else {
                    std::cerr << "exit keyword not found" << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
            else if (std::isdigit(peak().value())) {
                buf.push_back(consume());
                while (peak().has_value() && std::isdigit(peak().value())) {
                    buf.push_back(consume());
                }
                tokens.push_back({.type = TokenType::integer_literal, .value = buf});
                buf.clear();
                continue;
            }
            else if (peak().value() == ';') {
                consume();
                tokens.push_back({.type = TokenType::semi_colon});
                continue;
            }
            else if (std::isspace(peak().value())) {
                consume();
                continue;
            }
            else {
                std::cerr << "tokenization failed" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        m_current_index = 0;

        return tokens;
    }
private:
    const std::string m_src;
    int m_current_index{};

    [[nodiscard]] std::optional<char> peak(int characters_ahead = 1) const {
        if (m_current_index + characters_ahead > m_src.length()) {
            return {};
        }
        return m_src.at(m_current_index);
    }

    char consume() {
        return m_src.at(m_current_index++);
    }
};