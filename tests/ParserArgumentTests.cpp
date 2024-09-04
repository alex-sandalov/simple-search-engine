#include <gtest/gtest.h>

#include "ParserArgument/ParserArgument.hpp"

TEST(ParserArgumentTest, HandlesEmptyInput) {
    std::vector<std::string> words;
    auto result = ParserArgument::WordLeveling(words);
    
    EXPECT_TRUE(result.empty());
}

TEST(ParserArgumentTest, HandlesSingleWord) {
    std::vector<std::string> words = {"hello"};
    auto result = ParserArgument::WordLeveling(words);
    
    std::unordered_map<size_t, std::unordered_set<char>> expected = {
        {0, {'h'}}, {1, {'e'}}, {2, {'l'}}, {3, {'l'}}, {4, {'o'}}
    };
    
    EXPECT_EQ(result, expected);
}

TEST(ParserArgumentTest, HandlesMultipleWords) {
    std::vector<std::string> words = {"cat", "car", "bat"};
    auto result = ParserArgument::WordLeveling(words);
    
    std::unordered_map<size_t, std::unordered_set<char>> expected = {
        {0, {'c', 'b'}}, {1, {'a'}}, {2, {'t', 'r'}}
    };
    
    EXPECT_EQ(result, expected);
}

TEST(ParserArgumentTest, HandlesDifferentLengths) {
    std::vector<std::string> words = {"apple", "bat", "cat"};
    auto result = ParserArgument::WordLeveling(words);
    
    std::unordered_map<size_t, std::unordered_set<char>> expected = {
        {0, {'a', 'b', 'c'}}, {1, {'p', 'a'}}, {2, {'p', 't'}}, {3, {'l'}}, {4, {'e'}}
    };
    
    EXPECT_EQ(result, expected);
}

TEST(ParserArgumentTest, HandlesSpecialCharacters) {
    std::vector<std::string> words = {"!@#", "$%^", "&*("};
    auto result = ParserArgument::WordLeveling(words);
    
    std::unordered_map<size_t, std::unordered_set<char>> expected = {
        {0, {'!', '$', '&'}}, {1, {'@', '%', '*'}}, {2, {'#', '^', '('}}
    };
    
    EXPECT_EQ(result, expected);
}


TEST(ParserArgumentTest, RequestWithOperators) {
    ParserArgument parser;

    std::vector<std::string> request = {"word1", "AND", "word2", "OR", "word3"};
    parser.CreateStackRequest(request);
    std::vector<std::string> expected = {"word1", "word2", "AND", "word3", "OR"};

    auto postfix = parser.GetPostfix();
    for (size_t i = 0; i < postfix.size(); ++i) {
        EXPECT_EQ(postfix[i], expected[i]);
    }
}

TEST(ParserArgumentTest, RequestWithParentheses) {
    ParserArgument parser;

    std::vector<std::string> request = {"(", "word1", "AND", "word2", ")", "OR", "word3"};
    parser.CreateStackRequest(request);
    std::vector<std::string> expected = {"word1", "word2", "AND", "word3", "OR"};
    EXPECT_EQ(parser.GetPostfix(), expected);
}

TEST(ParserArgumentTest, RequestWithNestedParentheses) {
    ParserArgument parser;

    std::vector<std::string> request = {"(", "word1", "OR", "(", "word2", "AND", "word3", ")", ")", "AND", "word4"};
    parser.CreateStackRequest(request);
    std::vector<std::string> expected = {"word1", "word2", "word3", "AND", "OR", "word4", "AND"};
    EXPECT_EQ(parser.GetPostfix(), expected);
}

TEST(ParserArgumentTest, RequestWithExtraClosingParenthesis) {
    ParserArgument parser;

    std::vector<std::string> request = {"(", "word1", "AND", "word2", ")", ")"};
    EXPECT_ANY_THROW(parser.CreateStackRequest(request));
}	

TEST(ParserArgumentTest, OperatorANDTest) {
    ParserArgument parser;

    std::unordered_set<size_t> lhs = {1, 2, 3, 4};
    std::unordered_set<size_t> rhs = {3, 4, 5, 6};

    parser.OperatorAND(lhs, rhs);

    std::unordered_set<size_t> expected = {3, 4};
    EXPECT_EQ(rhs, expected);
}

TEST(ParserArgumentTest, OperatorORTest) {
    ParserArgument parser;

    std::unordered_set<size_t> lhs = {1, 2, 3, 4};
    std::unordered_set<size_t> rhs = {3, 4, 5, 6};

    parser.OperatorOR(lhs, rhs);

    std::unordered_set<size_t> expected = {1, 2, 3, 4, 5, 6};
    EXPECT_EQ(rhs, expected);
}

TEST(ParserArgumentTest, ExpressionCalculationTest) {
    ParserArgument parser;

    std::unordered_map<std::string, std::unordered_set<size_t>> file_words_and_indexes = {
        {"word1", {1, 2, 3}},
        {"word2", {3, 4, 5}},
        {"word3", {5, 6, 7}}
    };

    std::vector<std::string> postfix = {"word1", "word2", "AND", "word3", "OR"};
    parser.SetPostfix(postfix);

    std::unordered_set<size_t> result = parser.ExpressionCalculation(file_words_and_indexes);

    std::unordered_set<size_t> expected = {3, 5, 6, 7};
    EXPECT_EQ(result, expected);
}

TEST(ParserArgumentTest, SingleWordTest) {
    std::unordered_map<std::string, std::unordered_set<size_t>> file_words_and_indexes = {
        {"word1", {1, 2, 3}}
    };

    ParserArgument parser;

    std::vector<std::string> postfix = {"word1"};
    parser.SetPostfix(postfix);

    std::unordered_set<size_t> result = parser.ExpressionCalculation(file_words_and_indexes);

    std::unordered_set<size_t> expected = {1, 2, 3};
    EXPECT_EQ(result, expected);
}

TEST(ParserArgumentTest, ANDOperationTest) {
    std::unordered_map<std::string, std::unordered_set<size_t>> file_words_and_indexes = {
        {"word1", {1, 2, 3}},
        {"word2", {2, 3, 4}}
    };

    ParserArgument parser;

    std::vector<std::string> postfix = {"word1", "word2", "AND"};
    parser.SetPostfix(postfix);

    std::unordered_set<size_t> result = parser.ExpressionCalculation(file_words_and_indexes);

    std::unordered_set<size_t> expected = {2, 3};
    EXPECT_EQ(result, expected);
}

TEST(ParserArgumentTest, OROperationTest) {
    std::unordered_map<std::string, std::unordered_set<size_t>> file_words_and_indexes = {
        {"word1", {1, 2, 3}},
        {"word2", {3, 4, 5}}
    };

    ParserArgument parser;

    std::vector<std::string> postfix = {"word1", "word2", "OR"};
    parser.SetPostfix(postfix);

    std::unordered_set<size_t> result = parser.ExpressionCalculation(file_words_and_indexes);

    std::unordered_set<size_t> expected = {1, 2, 3, 4, 5};
    EXPECT_EQ(result, expected);
}

TEST(ParserArgumentTest, ComplexExpressionTest1) {
    std::unordered_map<std::string, std::unordered_set<size_t>> file_words_and_indexes = {
        {"word1", {1, 2}},
        {"word2", {2, 3}},
        {"word3", {3, 4}},
        {"word4", {4, 5}}
    };

    ParserArgument parser;

    std::vector<std::string> postfix = {"word1", "word2", "AND", "word3", "OR", "word4", "AND"};
    parser.SetPostfix(postfix);

    std::unordered_set<size_t> result = parser.ExpressionCalculation(file_words_and_indexes);

    std::unordered_set<size_t> expected = {4};
    EXPECT_EQ(result, expected);
}

TEST(ParserArgumentTest, ComplexExpressionTest2) {
    std::unordered_map<std::string, std::unordered_set<size_t>> file_words_and_indexes = {
        {"word1", {1, 2}},
        {"word2", {2, 3}},
        {"word3", {3, 4}},
        {"word4", {4, 5}},
        {"word5", {5, 6}}
    };

    ParserArgument parser;

    std::vector<std::string> postfix = {"word1", "word2", "AND", "word3", "AND", "word4", "OR", "word5", "AND"};
    parser.SetPostfix(postfix);

    std::unordered_set<size_t> result = parser.ExpressionCalculation(file_words_and_indexes);

    std::unordered_set<size_t> expected = {5};
    EXPECT_EQ(*expected.begin(), *result.begin());
}

TEST(GetWordsFromExpressionTest, OnlyOperationsAndBrackets) {
    std::vector<std::string> expression = {"(", "OR", "AND", ")", "OR", "AND"};
    std::vector<std::string> expected = {};
    std::vector<std::string> result = ParserArgument::GetWordsFromExpression(expression);
    ASSERT_EQ(result, expected);
}

TEST(GetWordsFromExpressionTest, MixedElements) {
    std::vector<std::string> expression = {"x", "OR", "3", "AND", "(", "y", "OR", "2", ")"};
    std::vector<std::string> expected = {"x", "3", "y", "2"};
    std::vector<std::string> result = ParserArgument::GetWordsFromExpression(expression);
    ASSERT_EQ(result, expected);
}

TEST(GetWordsFromExpressionTest, NoOperationsOrBrackets) {
    std::vector<std::string> expression = {"alpha", "beta", "gamma"};
    std::vector<std::string> expected = {"alpha", "beta", "gamma"};
    std::vector<std::string> result = ParserArgument::GetWordsFromExpression(expression);
    ASSERT_EQ(result, expected);
}

TEST(GetWordsFromExpressionTest, EmptyExpression) {
    std::vector<std::string> expression = {};
    std::vector<std::string> expected = {};
    std::vector<std::string> result = ParserArgument::GetWordsFromExpression(expression);
    ASSERT_EQ(result, expected);
}