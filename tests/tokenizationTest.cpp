#include "gtest/gtest.h"
#include "../include/tokenization.h"

TEST(TokenizerTest, TokenizeSimpleExpression) {
    std::string input = "uint32 x = 42; exit(1);";
    Tokenizer tokenizer(input);
    std::vector<Token> tokens = tokenizer.tokenize();

    ASSERT_EQ(tokens.size(), 10);

    EXPECT_EQ(tokens[0].type, TokenType::uint32);
    EXPECT_EQ(tokens[0].value, "uint32");

    EXPECT_EQ(tokens[1].type, TokenType::identifier);
    EXPECT_EQ(tokens[1].value, "x");

    EXPECT_EQ(tokens[2].type, TokenType::equals);

    EXPECT_EQ(tokens[3].type, TokenType::integer_literal);
    EXPECT_EQ(tokens[3].value, "42");

    EXPECT_EQ(tokens[4].type, TokenType::semi_colon);

    EXPECT_EQ(tokens[5].type, TokenType::exit);

    EXPECT_EQ(tokens[6].type, TokenType::open_parenthesis);

    EXPECT_EQ(tokens[7].type, TokenType::integer_literal);

    EXPECT_EQ(tokens[8].type, TokenType::closed_parenthesis);
}