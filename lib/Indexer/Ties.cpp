#include "Ties.hpp"

#include <iostream>
#include <queue>

Ties::TiesNode::TiesNode()
    : id_node(-3)
{}

Ties::TiesNode::TiesNode(int id_node)
    : id_node(id_node)
{}

Ties::TiesNode::TiesNode(int id_node, char symbol)
    : id_node(id_node)
    , symbol(symbol)
{}

Ties::WrapperSetStringWord::WrapperSetStringWord(std::unordered_set<size_t>::iterator iterator)
    : current_value_(iterator)
{}

Ties::WrapperSetStringWord& Ties::WrapperSetStringWord::operator++() {
    current_value_ = ++current_value_;
    return *this;
}

size_t Ties::WrapperSetStringWord::operator*() const {
    return *current_value_;
}

Ties::TiesIterator::TiesIterator(std::shared_ptr<TiesNode> current_node)
    : current_node_(std::move(current_node))
{}	

Ties::value_type Ties::TiesIterator::operator*() const {
    return current_node_->symbol;
}

Ties::WrapperSetStringWord Ties::TiesIterator::GetStartArray(size_t index) const {
    return WrapperSetStringWord(current_node_->string_word[index].begin());
}

Ties::WrapperSetStringWord Ties::TiesIterator::GetEndArray(size_t index) const {
    return WrapperSetStringWord(current_node_->string_word[index].end());
}

void Ties::TiesIterator::insert(size_t index, size_t value) {
    current_node_->string_word[index].insert(value);
}

Ties::Ties()
    : head_tree_(std::make_shared<TiesNode>(-1))
    , end_tree_(std::make_shared<TiesNode>(-2))
{}

Ties::Ties(std::unordered_map<size_t, std::unordered_set<char>> letters_by_level,
        const std::string& path_word_repository)
    : end_tree_(std::make_shared<TiesNode>(-2))
{
    ReadTiesFromFile(letters_by_level, path_word_repository);
}

void Ties::push(const std::string& word) {
    std::shared_ptr<TiesNode> current_node_tree = head_tree_;

    for (size_t i = 0; i < word.size(); ++i) {
        char current_symbol = word[i];

        if (current_node_tree->children[current_symbol] == nullptr) {
            current_node_tree->children[current_symbol] = std::make_shared<TiesNode>(++count_node_, current_symbol);
        }

        current_node_tree = current_node_tree->children[current_symbol];
    }
}

Ties::iterator Ties::search(const std::string& word) const {
    std::shared_ptr<TiesNode> current_node_tree = head_tree_;

    for (size_t i = 0; i < word.size(); ++i) {
        current_node_tree = current_node_tree->children[word[i]];
        if (current_node_tree == nullptr) {
            return iterator(end_tree_);
        }
    }

    return iterator(current_node_tree);
}

Ties::iterator Ties::begin() const {
    return iterator(head_tree_);
}

Ties::iterator Ties::end() const {
    return iterator(end_tree_);
}

void Ties::SaveTies(const std::string& filename_ties) {
    std::ofstream file_trie(filename_ties, std::ios::binary);
    if (!file_trie.is_open()) {
        std::cerr << "Error open file " << filename_ties << '\n';
        return;
    }
    
    std::queue<std::shared_ptr<TiesNode>> queue_node;
    queue_node.push(head_tree_);
    
    while (!queue_node.empty()) {
        std::shared_ptr<TiesNode> current_node = queue_node.front();
        queue_node.pop();
    	    
        SaveNode(current_node, file_trie);

        for (const auto& child : current_node->children) {
            if (child.second == nullptr) {
                std::cout << "nullptr: " << child.first << '\n';
                continue;
            }
            queue_node.push(child.second);
        }

    }
}

void Ties::SaveNode(const std::shared_ptr<TiesNode>& save_node, std::ofstream& file_trie) {
    size_t size_node = 0;
    for (const auto& node_set : save_node->string_word) {
        size_node += node_set.second.size() * sizeof(size_t) + 2 * sizeof(size_t);
    }
    
    TiesHeader header_node(save_node->symbol, save_node->children.size(),
                            save_node->string_word.size(), size_node);

    file_trie.write(reinterpret_cast<char*>(&header_node), sizeof(TiesHeader));

    for (const auto& node_set : save_node->string_word) {
        size_t size_node_set = node_set.second.size();
        file_trie.write(reinterpret_cast<char*>(&size_node_set), sizeof(size_node_set));
        file_trie.write(reinterpret_cast<const char*>(&node_set.first), sizeof(node_set.first));
        for (const auto element_set : node_set.second) {
            file_trie.write(reinterpret_cast<const char*>(&element_set), sizeof(element_set));
        }
    }
}

Ties::TiesHeader::TiesHeader(char symbol, size_t children_size, size_t string_word_lenght
    , size_t word_string_size)
    : symbol(symbol)
    , children_size(children_size)
    , string_word_lenght(string_word_lenght)
    , word_string_size(word_string_size)
{}

void Ties::ReadTiesFromFile(std::unordered_map<size_t, std::unordered_set<char>> letters_by_level,
        const std::string& path_word_repository) {

    std::ifstream file_trie(path_word_repository, std::ios::binary);
    if (!file_trie.is_open()) {
        std::cerr << "Error open file " << path_word_repository << '\n';
    }

    std::queue<InfoNodeHeader> queue_node;
    
    TiesHeader current_header = ReadHeaderNode(file_trie);
    head_tree_ = std::make_shared<TiesNode>(-1);

    InfoNodeHeader head_header_tree = InfoNodeHeader{-1, true, current_header, head_tree_};
    queue_node.push(head_header_tree);

    while(!queue_node.empty()) {
        InfoNodeHeader info_current_node = queue_node.front();
        queue_node.pop();

        for (size_t i = 0; i < info_current_node.header_info.children_size; ++i) {
            TiesHeader childred_current_node = ReadHeaderNode(file_trie);
            InfoNodeHeader info_childred;
        
            if (!info_current_node.is_contains_set || 
            !letters_by_level[info_current_node.depth + 1].contains(childred_current_node.symbol)) {
                file_trie.seekg(file_trie.tellg() + static_cast<std::streamoff>(childred_current_node.word_string_size)
                , std::ios::beg);

                info_childred = {info_current_node.depth + 1, false, childred_current_node};
                queue_node.push(info_childred);
            } else {
                info_current_node.node->children[childred_current_node.symbol] = 
                std::make_shared<TiesNode>(count_node_++, childred_current_node.symbol);

                ReadWordsNode(file_trie, info_current_node.node->children[childred_current_node.symbol]
                    , childred_current_node.string_word_lenght);

                info_childred = {info_current_node.depth + 1, true, childred_current_node, 
                                info_current_node.node->children[childred_current_node.symbol]};
                queue_node.push(info_childred);
            }
        }

    }
}

Ties::TiesHeader Ties::ReadHeaderNode(std::ifstream& file_trie) {
    TiesHeader header_node;

    file_trie.read(reinterpret_cast<char*>(&header_node), sizeof(TiesHeader));

    return header_node;
}

void Ties::ReadWordsNode(std::ifstream& file_trie, std::shared_ptr<TiesNode> current_node, size_t lenght_word_string) {
    for (size_t i = 0; i < lenght_word_string; ++i) {
        size_t size_node_set = 0;
        size_t index_node_set = 0;
        file_trie.read(reinterpret_cast<char*>(&size_node_set), sizeof(size_t));
        file_trie.read(reinterpret_cast<char*>(&index_node_set), sizeof(size_t));
        for (size_t j = 0; j < size_node_set; ++j) {
            size_t current_element_set;
            file_trie.read(reinterpret_cast<char*>(&current_element_set), sizeof(size_t));
            current_node->string_word[index_node_set].insert(current_element_set);
        }
    }
}

std::unordered_set<size_t> Ties::TiesIterator::GetKeyArray() const {
    std::unordered_set<size_t> index_array;
    for (const auto& [key, value] : current_node_->string_word) {
        index_array.insert(key);
    }
    return index_array;
}

bool Ties::TiesIterator::empty(size_t index) const {
    return current_node_->string_word[index].size() == 0;
}