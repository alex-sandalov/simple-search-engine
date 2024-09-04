#pragma once

#include <vector>
#include <string>
#include <unordered_map>

struct BM25 {
    constexpr static const double k1 = 2.0;
    constexpr static const double b = 0.75; 

    static double calculationIDF(size_t number_of_documents, size_t document_frequency);
    static double calculationTF(size_t word_frequency_in_document, 
                                size_t document_length,
                                double average_length_of_documents);
    static double calculation(size_t number_of_documents,
                            size_t document_frequency,
                            size_t word_frequency_in_document, 
                            size_t document_length,
                            double average_length_of_documents);                           
};

class Searcher {
public:
    explicit Searcher(const std::vector<std::string>& request);

    std::vector<std::pair<std::string, double>> GetBM25(
        const std::unordered_map<std::string, std::unordered_map<std::string, size_t>>& data_word
    );

    static std::vector<std::string> TokenizeExpression(const std::string& expression);

    static size_t GetWordCount(const std::string& filename);

private:
    std::vector<std::string> request_;
    std::unordered_map<std::string, size_t> count_word_in_file;
    double average_length_of_documents_;
    size_t count_documents_;
};