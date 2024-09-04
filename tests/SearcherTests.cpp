#include <gtest/gtest.h>
#include "Searcher/Searcher.hpp"

#include <fstream>

TEST(BM25Test, CalculationIDF) {
    EXPECT_NEAR(BM25::calculationIDF(1000, 10), 4.59511985, 1e-5);
    EXPECT_NEAR(BM25::calculationIDF(1000, 500), 0.0, 1e-5);
    EXPECT_NEAR(BM25::calculationIDF(1000, 999), -6.906755, 1e-5);
}

TEST(BM25Test, CalculationTF) {
    EXPECT_NEAR(BM25::calculationTF(3, 100, 50.0), 1.9212121, 1e-5);
    EXPECT_NEAR(BM25::calculationTF(5, 100, 100.0), 2.5, 1e-5);
    EXPECT_NEAR(BM25::calculationTF(1, 50, 50.0), 0.861702, 1e-5);
}

TEST(BM25Test, Calculation) {
    EXPECT_NEAR(BM25::calculation(1000, 10, 3, 100, 50.0), 4.59511985 * 1.9212121, 1e-5);
    EXPECT_NEAR(BM25::calculation(1000, 500, 5, 100, 100.0), 0.0, 1e-5);
    EXPECT_NEAR(BM25::calculation(1000, 999, 1, 50, 50.0), -6.906755 * 0.861702, 1e-5);
}

TEST(TokenizeExpressionTest, BasicTokenization) {
    std::string expression = "(a + b) * c";
    std::vector<std::string> expected_tokens = {"(", "a", "+", "b", ")", "*", "c"};
    std::vector<std::string> tokens = Searcher::TokenizeExpression(expression);
    ASSERT_EQ(tokens, expected_tokens);
}

TEST(TokenizeExpressionTest, NumberAndVariableTokenization) {
    std::string expression = "3 * (x + 2)";
    std::vector<std::string> expected_tokens = {"3", "*", "(", "x", "+", "2", ")"};
    std::vector<std::string> tokens = Searcher::TokenizeExpression(expression);
    ASSERT_EQ(tokens, expected_tokens);
}

TEST(TokenizeExpressionTest, SingleTokenExpression) {
    std::string expression = "variable";
    std::vector<std::string> expected_tokens = {"variable"};
    std::vector<std::string> tokens = Searcher::TokenizeExpression(expression);
    ASSERT_EQ(tokens, expected_tokens);
}

TEST(TokenizeExpressionTest, EmptyExpression) {
    std::string expression = "";
    std::vector<std::string> expected_tokens = {};
    std::vector<std::string> tokens = Searcher::TokenizeExpression(expression);
    ASSERT_EQ(tokens, expected_tokens);
}

TEST(TokenizeExpressionTest, SingleCharacterExpression) {
    std::string expression = "x";
    std::vector<std::string> expected_tokens = {"x"};
    std::vector<std::string> tokens = Searcher::TokenizeExpression(expression);
    ASSERT_EQ(tokens, expected_tokens);
}

TEST(TokenizeExpressionTest, ExpressionWithBrackets) {
    std::string expression = "(a + (b - c)) * (d / e)";
    std::vector<std::string> expected_tokens = {"(", "a", "+", "(", "b", "-", "c", ")", ")", "*", "(", "d", "/", "e", ")"};
    std::vector<std::string> tokens = Searcher::TokenizeExpression(expression);
    ASSERT_EQ(tokens, expected_tokens);
}

TEST(TokenizeExpressionTest, ExpressionWithOnlyBrackets) {
    std::string expression = "()";
    std::vector<std::string> expected_tokens = {"(", ")"};
    std::vector<std::string> tokens = Searcher::TokenizeExpression(expression);
    ASSERT_EQ(tokens, expected_tokens);
}

TEST(TokenizeExpressionTest, ExpressionWithBrackets2) {
    std::string expression = "((a + b) - c) * (d / e)";
    std::vector<std::string> expected_tokens = {"(", "(", "a", "+", "b", ")", "-", "c", ")", "*", "(", "d", "/", "e", ")"};
    std::vector<std::string> tokens = Searcher::TokenizeExpression(expression);
    ASSERT_EQ(tokens, expected_tokens);
}

TEST(SearcherTest, GetWordCountEmptyFile) {
    std::string emptyFilename = "empty.txt";
    std::ofstream emptyFile(emptyFilename);
    emptyFile.close();

    EXPECT_EQ(Searcher::GetWordCount(emptyFilename), 0);
}

TEST(SearcherTest, GetWordCountNonExistentFile) {
    EXPECT_EQ(Searcher::GetWordCount("nonexistentfile.txt"), 0);
}

TEST(SearcherTest, GetWordCountSingleWord) {
    std::string filename = "singleword.txt";
    std::ofstream singleWordFile(filename);
    singleWordFile << "Hello";
    singleWordFile.close();

    EXPECT_EQ(Searcher::GetWordCount(filename), 1);
}

TEST(SearcherTest, GetWordCountMultipleWords) {
    std::string filename = "multiplewords.txt";
    std::ofstream multipleWordsFile(filename);
    multipleWordsFile << "This is a test file";
    multipleWordsFile.close();

    EXPECT_EQ(Searcher::GetWordCount(filename), 5);
}

TEST(SearcherTest, GetWordCountWithSpecialCharacters) {
    std::string filename = "specialcharacters.txt";
    std::ofstream specialCharsFile(filename);
    specialCharsFile << "Hello, world! How are you?";
    specialCharsFile.close();

    EXPECT_EQ(Searcher::GetWordCount(filename), 5);
}

TEST(SearcherTest, GetWordCountLargeFile) {
    std::string filename = "largefile.txt";
    std::ofstream largeFile(filename);
    
    const int numWords = 100000;
    for (int i = 0; i < numWords; ++i) {
        largeFile << "word" << i << " ";
    }
    
    largeFile.close();

    EXPECT_EQ(Searcher::GetWordCount(filename), numWords);
}