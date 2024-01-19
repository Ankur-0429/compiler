#include <gtest/gtest.h>
#include "../include/tokenization.h"
#include "../include/parser.h"
#include <variant>

TEST(ParserTest, CanParseIntegerLiteral43) {
    std::vector<Token> tokens = {
            {TokenType::integer_literal, "42"},
            {TokenType::plus},
            {TokenType::integer_literal, "3"},
            {TokenType::star},
            {TokenType::open_parenthesis},
            {TokenType::integer_literal, "5"},
            {TokenType::sub},
            {TokenType::integer_literal, "2"},
            {TokenType::closed_parenthesis}
    };
    Parser parser(tokens);

    auto result = parser.parse_expr();

    ASSERT_TRUE(result.has_value());
    ASSERT_TRUE(std::holds_alternative<NodeBinaryExpression*>(result.value()->var));
    NodeBinaryExpression* binaryExpression = std::get<NodeBinaryExpression*>(result.value()->var);
    ASSERT_TRUE(std::holds_alternative<NodeBinaryExpressionAdd*>(binaryExpression->var));
    NodeBinaryExpressionAdd* add = std::get<NodeBinaryExpressionAdd*>(binaryExpression->var);
    ASSERT_TRUE(std::holds_alternative<NodeTerm*>(add->lhs->var));
    NodeTerm* term = std::get<NodeTerm*>(add->lhs->var);
    ASSERT_TRUE(std::holds_alternative<NodeTermIntegerLiteral*>(term->var));
    NodeTermIntegerLiteral* integerLiteral = std::get<NodeTermIntegerLiteral*>(term->var);
    ASSERT_TRUE(integerLiteral->integer_literal.value.has_value() && integerLiteral->integer_literal.value.value() == "42");
}

TEST(ParserTest, CanParseIntegerLiteral3) {
    std::vector<Token> tokens = {
            {TokenType::integer_literal, "42"},
            {TokenType::plus},
            {TokenType::integer_literal, "3"},
            {TokenType::star},
            {TokenType::open_parenthesis},
            {TokenType::integer_literal, "5"},
            {TokenType::sub},
            {TokenType::integer_literal, "2"},
            {TokenType::closed_parenthesis}
    };
    Parser parser(tokens);

    auto result = parser.parse_expr();
    NodeBinaryExpression* binaryExpression = std::get<NodeBinaryExpression*>(result.value()->var);
    NodeBinaryExpressionAdd* add = std::get<NodeBinaryExpressionAdd*>(binaryExpression->var);

    ASSERT_TRUE(std::holds_alternative<NodeBinaryExpression*>(add->rhs->var));
    NodeBinaryExpression* rhsBinaryExpression = std::get<NodeBinaryExpression*>(add->rhs->var);
    ASSERT_TRUE(std::holds_alternative<NodeBinaryExpressionMultiplication*>(rhsBinaryExpression->var));
    NodeBinaryExpressionMultiplication* mult = std::get<NodeBinaryExpressionMultiplication*>(rhsBinaryExpression->var);
    ASSERT_TRUE(std::holds_alternative<NodeTerm*>(mult->lhs->var));
    NodeTerm *term = std::get<NodeTerm*>(mult->lhs->var);
    ASSERT_TRUE(std::holds_alternative<NodeTermIntegerLiteral*>(term->var));
    NodeTermIntegerLiteral* integerLiteral = std::get<NodeTermIntegerLiteral*>(term->var);
    ASSERT_TRUE(integerLiteral->integer_literal.value.has_value() && integerLiteral->integer_literal.value.value() == "3");
}

TEST(ParserTest, CanParseIntegerParenthesis) {
    std::vector<Token> tokens = {
            {TokenType::integer_literal, "42"},
            {TokenType::plus},
            {TokenType::integer_literal, "3"},
            {TokenType::star},
            {TokenType::open_parenthesis},
            {TokenType::integer_literal, "5"},
            {TokenType::sub},
            {TokenType::integer_literal, "2"},
            {TokenType::closed_parenthesis}
    };
    Parser parser(tokens);

    auto result = parser.parse_expr();
    NodeBinaryExpression* binaryExpression = std::get<NodeBinaryExpression*>(result.value()->var);
    NodeBinaryExpressionAdd* add = std::get<NodeBinaryExpressionAdd*>(binaryExpression->var);

    NodeBinaryExpression* rhsBinaryExpression = std::get<NodeBinaryExpression*>(add->rhs->var);
    ASSERT_TRUE(std::holds_alternative<NodeBinaryExpressionMultiplication*>(rhsBinaryExpression->var));
    NodeBinaryExpressionMultiplication* mult = std::get<NodeBinaryExpressionMultiplication*>(rhsBinaryExpression->var);
    ASSERT_TRUE(std::holds_alternative<NodeTerm*>(mult->rhs->var));
    NodeTerm *term = std::get<NodeTerm*>(mult->rhs->var);
    ASSERT_TRUE(std::holds_alternative<NodeTermParenthesis*>(term->var));
    NodeTermParenthesis *paren = std::get<NodeTermParenthesis*>(term->var);
    ASSERT_TRUE(std::holds_alternative<NodeBinaryExpression*>(paren->expr->var));
    NodeBinaryExpression* secondRhsBinaryExpression = std::get<NodeBinaryExpression*>(paren->expr->var);
    ASSERT_TRUE(std::holds_alternative<NodeBinaryExpressionSubtraction*>(secondRhsBinaryExpression->var));
    NodeBinaryExpressionSubtraction* sub = std::get<NodeBinaryExpressionSubtraction*>(secondRhsBinaryExpression->var);

    ASSERT_TRUE(std::holds_alternative<NodeTerm*>(sub->lhs->var));
    NodeTerm* termSecond = std::get<NodeTerm*>(sub->lhs->var);
    ASSERT_TRUE(std::holds_alternative<NodeTermIntegerLiteral*>(termSecond->var));
    NodeTermIntegerLiteral* integerLiteralSecond = std::get<NodeTermIntegerLiteral*>(termSecond->var);
    ASSERT_TRUE(integerLiteralSecond->integer_literal.value.has_value() && integerLiteralSecond->integer_literal.value.value() == "5");

    ASSERT_TRUE(std::holds_alternative<NodeTerm*>(sub->rhs->var));
    NodeTerm* termThird = std::get<NodeTerm*>(sub->rhs->var);
    ASSERT_TRUE(std::holds_alternative<NodeTermIntegerLiteral*>(termThird->var));
    NodeTermIntegerLiteral* integerLiteralThird = std::get<NodeTermIntegerLiteral*>(termThird->var);
    ASSERT_TRUE(integerLiteralThird->integer_literal.value.has_value() && integerLiteralThird->integer_literal.value.value() == "2");
}



