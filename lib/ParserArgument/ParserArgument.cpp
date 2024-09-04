#include "ParserArgument.hpp"

#include <iostream>

std::unordered_map<size_t, std::unordered_set<char>>
ParserArgument::WordLeveling(const std::vector<std::string>& words){
    std::unordered_map<size_t, std::unordered_set<char>> word_level;

    for (size_t i = 0; i < words.size(); ++i) {
        for (size_t j = 0; j < words[i].size(); ++j) {
            word_level[j].insert(words[i][j]);
        }
    }

    return word_level;
}

bool ParserArgument::IsOperation(const std::string& word) {
    return word == kOperationAND || word == kOperationOR;
}

void ParserArgument::CreateStackRequest(const std::vector<std::string>& request) {
    std::vector<std::string> stack_operators;
    std::string current_number;

    for (const std::string& word : request) {
        if (!IsOperation(word) && word != "(" && word != ")") {
            current_number = word;
            continue;
        }

        if (!current_number.empty()) {
            postfix_.push_back(current_number);
            current_number.clear();
        }

        if (word == "(") {
            stack_operators.push_back("(");
        } else if (word == ")") {
            if (stack_operators.empty()) {
                throw std::invalid_argument("Mismatched parentheses: too many closing parentheses");
            }
            while (stack_operators.back() != "(") {
                postfix_.push_back(stack_operators.back());
                stack_operators.pop_back();
                if (stack_operators.empty()) {
                    throw std::invalid_argument("Mismatched parentheses: too many closing parentheses");
                }
            }
            stack_operators.pop_back();
        } else {
            while (!stack_operators.empty() && stack_operators.back() != "(" && (word == "OR" || stack_operators.back() == "AND")) {
                postfix_.push_back(stack_operators.back());
                stack_operators.pop_back();
            }
            stack_operators.push_back(word);
        }
    }

    if (!current_number.empty()) {
        postfix_.push_back(current_number);
    }

    while (!stack_operators.empty()) {
        if (stack_operators.back() == "(") {
            throw std::invalid_argument("Mismatched parentheses: too many opening parentheses");
        }
        postfix_.push_back(stack_operators.back());
        stack_operators.pop_back();
    }

    postfix_.insert(postfix_.end(), stack_operators.rbegin(), stack_operators.rend());
}

const std::vector<std::string>& ParserArgument::GetPostfix() const {
    return postfix_;
}

std::unordered_set<size_t> ParserArgument::ExpressionCalculation(
std::unordered_map<std::string, std::unordered_set<size_t>>& file_words_and_indexes) {
    std::vector<std::unordered_set<size_t>> result_expression_calculation;
    std::vector<std::string> word_stack;

    for (const std::string& token : postfix_) {
        if (!IsOperation(token) && token != "(" && token != ")") {
            if (file_words_and_indexes.contains(token)) {
                result_expression_calculation.push_back(file_words_and_indexes[token]);
            }
        } else {
            std::unordered_set<size_t> lhs = result_expression_calculation.back();
            result_expression_calculation.pop_back();
            std::unordered_set<size_t>& rhs = result_expression_calculation.back();

            if (token == kOperationAND) {
                OperatorAND(lhs, rhs);
            } else if (token == kOperationOR) {
                OperatorOR(lhs, rhs);
            }
        }
    }

    return result_expression_calculation.back();
}

void ParserArgument::OperatorAND(
const std::unordered_set<size_t>& lhs, std::unordered_set<size_t>& rhs) {
    for (auto it = rhs.begin(); it != rhs.end();) {
        if (lhs.find(*it) == lhs.end()) {
            it = rhs.erase(it);
        } else {
            ++it;
        }
    }
}

void ParserArgument::OperatorOR(
const std::unordered_set<size_t>& lhs, std::unordered_set<size_t>& rhs) {
    for (auto it = lhs.begin(); it != lhs.end(); ++it) {
        rhs.insert(*it);
    }
}

std::vector<std::string> ParserArgument::GetWordsFromExpression(const std::vector<std::string>& expression) {
    std::vector<std::string> result_expression;

    for (size_t i = 0; i < expression.size(); ++i) {
        if (!IsOperation(expression[i]) && expression[i] != "(" && expression[i] != ")") {
            result_expression.push_back(expression[i]);
        }
    }

    return result_expression;
}