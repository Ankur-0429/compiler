#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include "parser.h"
#include "tokenization.h"
#include "generation.h"

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
    Parser parser(tokens);
    std::optional<NodeExit> tree = parser.parse();

    if (!tree.has_value()) {
        std::cerr << "Unable to parse correctly" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(tree.value());
    {
        std::fstream file("out.s", std::ios::out);
        file << generator.generate();
    }

    system("as out.s -o out.o");
    system("ld out.o -o out");

    return EXIT_SUCCESS;
}
