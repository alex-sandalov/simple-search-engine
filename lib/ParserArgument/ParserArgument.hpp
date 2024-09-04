#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>
#include <stack>
#include <cstdint>

class ParserArgument {
public:
    constexpr static const char* kOperationAND = "AND";
    constexpr static const char* kOperationOR = "OR";

    static std::unordered_map<size_t, std::unordered_set<char>>
    WordLeveling(const std::vector<std::string>& words);

    static std::vector<std::string> GetWordsFromExpression(const std::vector<std::string>& expression);

    void CreateStackRequest(const std::vector<std::string>& request);
    static bool IsOperation(const std::string& word);
    const std::vector<std::string>& GetPostfix() const;
    std::unordered_set<size_t> ExpressionCalculation(std::unordered_map<std::string, std::unordered_set<size_t>>&
                                            file_words_and_indexes);

    void OperatorAND(const std::unordered_set<size_t>& lhs, std::unordered_set<size_t>& rhs);
    void OperatorOR(const std::unordered_set<size_t>& lhs, std::unordered_set<size_t>& rhs);

    void SetPostfix(std::vector<std::string> postfix) {
        std::swap(postfix_, postfix);
    }                                     
private:
    std::vector<std::string> postfix_;
};