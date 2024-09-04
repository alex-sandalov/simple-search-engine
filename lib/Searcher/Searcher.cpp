#include "Searcher.hpp"

#include <memory>
#include <cmath>
#include <sstream>
#include <fstream>
#include <algorithm>

Searcher::Searcher(const std::vector<std::string>& request)
    : request_(std::move(request))
{
    for (const std::string& v : request) {
        count_word_in_file[v] = GetWordCount(v);
        average_length_of_documents_ += count_word_in_file[v];
    }
    average_length_of_documents_ /= request.size();
    count_documents_ = request.size();
}

double BM25::calculationIDF(size_t number_of_documents, size_t document_frequency) {
    return std::log((number_of_documents - document_frequency + 0.5) / (document_frequency + 0.5));
}

double BM25::calculationTF(size_t word_frequency_in_document, size_t document_length, double average_length_of_documents) {
    return (word_frequency_in_document * (BM25::k1 + 1)) / 
           (word_frequency_in_document + BM25::k1 * (1 - BM25::b + BM25::b * (document_length / average_length_of_documents)));
}

double BM25::calculation(size_t number_of_documents, size_t document_frequency, size_t word_frequency_in_document, size_t document_length, double average_length_of_documents) {
    return calculationIDF(number_of_documents, document_frequency) * 
           calculationTF(word_frequency_in_document, document_length, average_length_of_documents);                            
}

std::vector<std::string> Searcher::TokenizeExpression(const std::string& expression) {
    std::vector<std::string> tokens;
    std::stringstream ss(expression);
    std::string token;
        
    while (ss >> token) {
        if (token[0] == '(') {
            while (token.size() != 0 && token[0] == '(') {
                token.erase(token.begin());
                tokens.push_back("(");
            }
            tokens.push_back(token);
        } else if (token[token.size() - 1] == ')') {
            size_t count = 0;
            while (token.size() != 0 && token[token.size() - 1] == ')') {
                token.erase(token.begin() + token.size() - 1);
                ++count;
            }

            tokens.push_back(token);
            
            for (size_t i = 0; i < count; ++i) {
                tokens.push_back(")");
            }
        } else {
            tokens.push_back(token);
        }
        
    }
        
    return tokens;
}

size_t Searcher::GetWordCount(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return 0;
    }

    std::string word;
    size_t wordCount = 0;

    while (file >> word) {
        ++wordCount;
    }

    return wordCount;
}

std::vector<std::pair<std::string, double>> Searcher::GetBM25(
    const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& data_word) {
    std::unordered_map<std::string, double> bm25_file;

    for (const auto& name_word : data_word) {
        size_t document_frequency = 0;
        std::unordered_map<std::string, size_t> word_frequency_in_document;

        for (const auto& name_file : name_word.second) {
            if (std::find(request_.begin(), request_.end(), name_file.first) != request_.end()) {
                document_frequency += name_file.second;
                word_frequency_in_document[name_file.first] = name_file.second;
            }
        }

        for (const auto& word : request_) {
            size_t word_freq_in_doc = word_frequency_in_document.count(word) ? word_frequency_in_document[word] : 0;
            bm25_file[word] += BM25::calculation(
                count_documents_,
                document_frequency,
                word_freq_in_doc,
                count_word_in_file[word],
                average_length_of_documents_
            );
        }
    }

    std::vector<std::pair<std::string, double>> result;
    for (const auto& u : bm25_file) {
        result.emplace_back(u.first, u.second);
    }

    std::sort(result.begin(), result.end(),
    [](const std::pair<std::string, double>& lhs, const std::pair<std::string, double>& rhs) {
        return lhs.second > rhs.second;
    });

    return result;
}
