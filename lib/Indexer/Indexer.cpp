#include "Indexer.hpp"

#include <iostream>

template<bool IsWriteWords>
const std::unordered_set<std::string> IndexerBase<IsWriteWords>::kValidExtension = {
    ".hpp", ".cpp"
};

template<bool IsWriteWords>
Ties::iterator IndexerBase<IsWriteWords>::SearchWordAtRepository(const std::string& word) const {
    return word_repository_->search(ProcessingWord(word));
}

template<bool IsWriteWords>
void IndexerBase<IsWriteWords>::AddWordAtRepository(const std::string& word) {
    word_repository_->push(ProcessingWord(word));
}

template<bool IsWriteWords>
IndexerBase<IsWriteWords>::IndexerBase()
    : word_repository_(std::make_unique<Ties>())
{}

template<bool IsWriteWords>
IndexerBase<IsWriteWords>::IndexerBase(std::unordered_map<size_t, std::unordered_set<char>>
        letters_by_level, const std::string& path_word_repository)
    : word_repository_(std::make_unique<Ties>(letters_by_level, path_word_repository))
{
    ReadIdDirectoryFromBinFile();
}

template<bool IsWriteWords>
void IndexerBase<IsWriteWords>::WriteTiesToBinFile(const std::string& filename_ties) {
    word_repository_->SaveTies(filename_ties);
}

template<bool IsWriteWords>
void IndexerBase<IsWriteWords>::WriteIdDirectoryToBinFile(const char* filename_id_directory) {
    std::ofstream file_id_directory(filename_id_directory, std::ios::binary);

    size_t size_id_directory = id_directory_.size();
    file_id_directory.write(reinterpret_cast<char*>(&size_id_directory), sizeof(size_t));

    for (const auto& element_directory_id : id_directory_) {
        size_t tmp = element_directory_id.first;
        file_id_directory.write(reinterpret_cast<char*>(&tmp), sizeof(size_t));
        size_t str_size = element_directory_id.second.size();
        
        file_id_directory.write(reinterpret_cast<char*>(&str_size), sizeof(size_t));
        file_id_directory.write(element_directory_id.second.data(), str_size);
    }
}

template<bool IsWriteWords>
void IndexerBase<IsWriteWords>::ReadIdDirectoryFromBinFile(const char* filename_id_directory) {
    std::ifstream file_id_directory(filename_id_directory, std::ios::binary);

    size_t size_id_directory = 0;
    file_id_directory.read(reinterpret_cast<char*>(&size_id_directory), sizeof(size_t));
 
    for (size_t i = 0; i < size_id_directory; ++i) {
        size_t id_element_id_directory;
        file_id_directory.read(reinterpret_cast<char*>(&id_element_id_directory), sizeof(size_t));
        
        size_t str_size;
        file_id_directory.read(reinterpret_cast<char*>(&str_size), sizeof(size_t));
        std::string value_element_id_directory(str_size, ' ');
        file_id_directory.read(&value_element_id_directory[0], str_size);

        id_directory_[id_element_id_directory] = value_element_id_directory;
    }
}

template<>
Indexer<true>::Indexer()
    : IndexerBase<true>::IndexerBase()
{}

template<>
Indexer<false>::Indexer(std::unordered_map<size_t, std::unordered_set<char>> letters_by_level,
        const std::string& path_word_repository)
    : IndexerBase<false>::IndexerBase(letters_by_level, path_word_repository)
{}

template<>
void Indexer<true>::AddWord(const std::string& word) {
    return this->AddWordAtRepository(word);
}

template<>
void Indexer<false>::AddWord(const std::string& word) {
    throw std::runtime_error("error mode indexer");
}

template<bool IsWriteWords>
Ties::iterator Indexer<IsWriteWords>::SearchWord(const std::string& word) const {
    return this->SearchWordAtRepository(word);
}

template<>
Indexer<true>::~Indexer() {
    SaveIndexer();
}

template<>
Indexer<false>::~Indexer() {
}

template<bool IsWriteWords>
Ties::iterator Indexer<IsWriteWords>::begin() const {
    return this->word_repository_->begin();
}

template<bool IsWriteWords>
Ties::iterator Indexer<IsWriteWords>::end() const {
    return this->word_repository_->end();
}

template<bool IsWriteWords>
bool IndexerBase<IsWriteWords>::IsValidFile(const std::filesystem::path& file_path) {
    return kValidExtension.contains(file_path.extension());
}

template<>
void Indexer<true>::StartIndexer(const std::filesystem::path& directory_path) {   
    if (!std::filesystem::exists(directory_path) || !std::filesystem::is_directory(directory_path)) {
        throw std::runtime_error("could not find the folder");
    }

    for (const auto& entry : std::filesystem::directory_iterator(directory_path)) {
        if (entry.is_directory()) {
            StartIndexer(entry.path());
        } else if (entry.is_regular_file()) {
            if (this->IsValidFile(entry.path())) {
                ++file_id;
                this->SaveWordsFromFile(entry.path(), file_id);
            }
        }
    }
}

template<>
void Indexer<false>::StartIndexer(const std::filesystem::path& directory_path) {
    throw std::runtime_error("error mode indexer");
}

template<bool IsWriteWords>
std::string IndexerBase<IsWriteWords>::ProcessingWord(const std::string& word) {
    std::string result_word;
    
    for (char symbol : word) {
        if (std::isalpha(symbol)) {
            result_word += std::tolower(symbol);
        } else {
            result_word += symbol;
        }
    }

    return result_word;
}

template<bool IsWriteWords>
void Indexer<IsWriteWords>::SaveWordsFromFile(const std::filesystem::path& file_path, size_t& file_id) {
    if (!std::filesystem::exists(file_path)) {
        throw std::runtime_error("file not found");
    }
    
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("error open file");
    }

    this->id_directory_[file_id] = file_path;

    std::string line;
    size_t line_number = 0;
    
    while (std::getline(file, line)) {
        ++line_number;
        std::istringstream line_stream(line);
        std::string word;
        while (line_stream >> word) {
            if (word.size() >= this->kMaxLenghtWord) {
                continue;
            }
            
            AddWord(this->ProcessingWord(word));

            auto iterator_word = this->SearchWordAtRepository(word);
            iterator_word.insert(file_id, line_number);
        }
    }
}

template class Indexer<true>;
template class Indexer<false>;

template class IndexerBase<true>;
template class IndexerBase<false>;
