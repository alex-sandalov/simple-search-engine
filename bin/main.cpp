#include "lib/Indexer/Indexer.hpp"
#include "lib/ParserArgument/ParserArgument.hpp"
#include "lib/Searcher/Searcher.hpp"

#include <iostream>
#include <fstream>
#include <chrono> 

const char* indexer_flag = "--indexer";
const char* searcher_flag = "--searcher";

int main(int argc, char* argv[]) {
    if (argc <= 1) {
        throw std::runtime_error("error argv");
    }

    std::string argument_1 = argv[1];
    std::cout << argument_1 << '\n';
    if (argument_1 == indexer_flag && argc >= 2) {
        Indexer<true> indexer;
        std::filesystem::path path_folder = argv[2];
        std::cout << "indexer folder: " << path_folder << '\n';
        indexer.StartIndexer(path_folder);
    }

    if (argument_1 == searcher_flag) {
        std::string command;

        while(true) {
            std::cin >> command;

            std::vector<std::string> command_expression = Searcher::TokenizeExpression(command);
            std::vector<std::string> words_from_expression = ParserArgument::GetWordsFromExpression(command_expression);

            Indexer<false> indexer(ParserArgument::WordLeveling(words_from_expression));

            ParserArgument parser_argument;
            parser_argument.CreateStackRequest(command_expression);
            
            std::unordered_map<std::string, std::unordered_set<size_t>> file_words_and_indexes;
            std::unordered_map<std::string, Ties::iterator> name_ties_iterator;
            for (size_t i = 0; i < words_from_expression.size(); ++i) {
                auto iterator = indexer.SearchWord(words_from_expression[i]);

                if (iterator == indexer.end()) {
                    std::cout << words_from_expression[i] << " not found\n";
                } else {
                    std::cout << "found " << words_from_expression[i] << '\n';
                    file_words_and_indexes[words_from_expression[i]] = iterator.GetKeyArray();
                    name_ties_iterator[words_from_expression[i]] = iterator;
                }
            }
            
            std::unordered_set<size_t> result_calculation = parser_argument.ExpressionCalculation(file_words_and_indexes);
            std::unordered_map<std::string, size_t> reverse_directory_id;

            std::vector<std::string> name_file_result;
            for (size_t v : result_calculation) {
                name_file_result.push_back(indexer.StringIndex(v));
                reverse_directory_id[indexer.StringIndex(v)] = v;

            }

            Searcher searcher(name_file_result);

            std::unordered_map<std::string, std::unordered_map<std::string, size_t>> info_for_bm25;
            for (const std::string& word : words_from_expression) {
                for (const auto& e : file_words_and_indexes[word]) {
                    info_for_bm25[word][indexer.StringIndex(e)] = name_ties_iterator[word].size(e);
                }
            }

            std::vector<std::pair<std::string, double>> result = searcher.GetBM25(info_for_bm25);

            for (const auto& elemet : result) {
                std::cout << "filename: " << elemet.first << '\n';
                
                for (const auto& element_iterator : name_ties_iterator) {
                    if (!element_iterator.second.size(reverse_directory_id[elemet.first])) {
                        continue;
                    }
                    for (auto it = element_iterator.second.GetStartArray(reverse_directory_id[elemet.first]);
                        it != element_iterator.second.GetEndArray(reverse_directory_id[elemet.first]); ++it) {
                        std::cout << element_iterator.first << " " << *it << '\n';
                    }
                }
            }

            std::cout << "end\n";

        }
    }
 }
