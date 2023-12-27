#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include "./tokenization.hpp"

std::string tokens_to_asm(const std::vector<Token>& tokens) {
    std::stringstream output_stream;
    output_stream << ".global _main\n.align 2\n_main:\n";
    for (int i = 0; i < tokens.size(); i++) {
        const Token& token = tokens.at(i);
        if (token.type == TokenType::exit) {
            if (i + 1 < tokens.size() && tokens.at(i+1).type == TokenType::integer_literal) {
                if (i + 2 < tokens.size() && tokens.at(i+2).type == TokenType::semi_colon) {
                    output_stream << "    mov x0, #" << tokens.at(i+1).value.value() << "\n";
                    output_stream << "    mov x16, #1\n";
                    output_stream << "    svc 0";
                }
            }
        }
    }
    return output_stream.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Incorrect Usage" << std::endl;
        std::cerr << "hydro <input.hy>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string contents;
    {
        std::fstream file(argv[1], std::ios::in);
        std::stringstream contents_stream;
        contents_stream << file.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(contents);

    std::vector<Token> tokens = tokenizer.tokenize();
    {
        std::fstream file("out.s", std::ios::out);
        file << tokens_to_asm(tokens);
    }

//    system("as out.s -o out.o");
//    system("ld out.o -o out");

    return EXIT_SUCCESS;
}
