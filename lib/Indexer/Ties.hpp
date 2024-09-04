#pragma once

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <filesystem>
#include <fstream>
#include <vector>

class Ties {
public:
    using value_type = char;	
private:
    struct TiesNode {
        char symbol;
        int id_node;
        std::unordered_map<char, std::shared_ptr<TiesNode>> children;
        std::unordered_map<size_t, std::unordered_set<size_t>> string_word;

        TiesNode();
        explicit TiesNode(int id_node);
        explicit TiesNode(int id_node, char symbol);
    };

    struct TiesHeader {
        char symbol;
        size_t children_size;
        size_t string_word_lenght;
        size_t word_string_size;

        TiesHeader() = default;
        TiesHeader(char symbol, size_t children_size, size_t string_word_lenght, size_t word_string_size);
    };

    class WrapperSetStringWord {
    public:
        explicit WrapperSetStringWord(std::unordered_set<size_t>::iterator iterator);
        
        WrapperSetStringWord& operator++();
        size_t operator*() const;

        friend bool operator==(const WrapperSetStringWord& lhs, const WrapperSetStringWord& rhs) {
            return lhs.current_value_ == rhs.current_value_;
        }

        friend bool operator!=(const WrapperSetStringWord& lhs, const WrapperSetStringWord& rhs) {
            return !(lhs == rhs);
        }
    private:
        std::unordered_set<size_t>::iterator current_value_;
    };

    class TiesIterator {
    public:
        TiesIterator() = default;
        explicit TiesIterator(std::shared_ptr<TiesNode> current_node);

        value_type operator*() const;

        WrapperSetStringWord GetStartArray(size_t index) const;
        WrapperSetStringWord GetEndArray(size_t index) const;
        std::unordered_set<size_t> GetKeyArray() const;

        void insert(size_t index, size_t value);
        bool empty(size_t index) const;

        size_t size(size_t index) const {
            return current_node_->string_word[index].size();
        }

        friend bool operator==(const TiesIterator& lhs, const TiesIterator& rhs) {
            return lhs.current_node_->id_node == rhs.current_node_->id_node;
        }

        friend bool operator!=(const TiesIterator& lhs, const TiesIterator& rhs) {
            return !(lhs == rhs);
        }
    private:
        std::shared_ptr<TiesNode> current_node_;
    };

    struct InfoNodeHeader {
        int depth;
        bool is_contains_set;
        TiesHeader header_info;
        std::shared_ptr<TiesNode> node;
    };
public:
    using iterator = TiesIterator;

    Ties();
    explicit Ties(std::unordered_map<size_t, std::unordered_set<char>> letters_by_level,
        const std::string& path_word_repository);

    void push(const std::string& word);
    iterator search(const std::string& word) const;
    iterator begin() const;
    iterator end() const;

    void SaveTies(const std::string& filename_ties);
private:
    size_t count_node_;
    std::shared_ptr<TiesNode> head_tree_;
    std::shared_ptr<TiesNode> end_tree_;

    void ReadTiesFromFile(std::unordered_map<size_t, std::unordered_set<char>> letters_by_level,
        const std::string& path_word_repository);
    TiesHeader ReadHeaderNode(std::ifstream& file_trie);
    void SaveNode(const std::shared_ptr<TiesNode>& save_node, std::ofstream& file_trie);
    void ReadWordsNode(std::ifstream& file_trie, std::shared_ptr<TiesNode> current_node, size_t lenght_word_string);
};