#include <iostream>
#include <fstream>
#include <sstream>
#include <optional>
#include "../include/parser.h"
#include "../include/tokenization.h"
#include "../include/generation.h"
#include "../include/arenaAllocator.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Incorrect Usage" << std::endl;
        std::cerr << "hydro <input.hy> <output_file>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string inputFileName = argv[1];
    std::string outputFileName = argv[2];

    std::string contents;
    {
        std::fstream file(inputFileName, std::ios::in);
        std::stringstream contents_stream;
        contents_stream << file.rdbuf();
        contents = contents_stream.str();
    }

    Tokenizer tokenizer(contents);

    std::vector<Token> tokens = tokenizer.tokenize();
    Parser parser(tokens);
    std::optional<NodeProgram> program = parser.parse_program();

    if (!program.has_value()) {
        std::cerr << "Invalid Program" << std::endl;
        exit(EXIT_FAILURE);
    }

    Generator generator(program.value());
    {
        std::fstream file(outputFileName + ".ll", std::ios::out);
        file << generator.generate_program();
    }

    std::string llc_command = "clang -O3 -o " + outputFileName + " " + outputFileName + ".ll";
    if (std::system(llc_command.c_str()) != 0) {
        std::cerr << "Error running clang\n";
        return EXIT_FAILURE;
    }

//    std::remove((outputFileName + ".ll").c_str());

    return EXIT_SUCCESS;
}
