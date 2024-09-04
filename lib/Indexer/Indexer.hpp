#pragma once

#include <string>
#include <string>

#include "Ties.hpp"

template<bool IsWriteWords>
class IndexerBase {
protected:	
    constexpr static const char* kFileNameTrie = "trie.bin";
    constexpr static const char* kFileNameIdDirectory = "id_directory.bin";
    constexpr static const size_t kMaxLenghtWord = 32;

    static const std::unordered_set<std::string> kValidExtension;

    IndexerBase();
    explicit IndexerBase(std::unordered_map<size_t, std::unordered_set<char>> letters_by_level,
        const std::string& path_word_repository);

    Ties::iterator SearchWordAtRepository(const std::string& word) const;
    void AddWordAtRepository(const std::string& word);

    void WriteTiesToBinFile(const std::string& filename_ties = kFileNameTrie);
    void WriteIdDirectoryToBinFile(const char* filename_id_directory = kFileNameIdDirectory);

    std::string StringIndexFromUnMap(size_t index) {
        return id_directory_[index];
    }

    std::unique_ptr<Ties> word_repository_;
    std::unordered_map<size_t, std::string> id_directory_;

    static bool IsValidFile(const std::filesystem::path& file_path);
    static std::string ProcessingWord(const std::string& word);
private:
    void ReadIdDirectoryFromBinFile(const char* filename_id_directory = kFileNameIdDirectory);
};

template<bool IsWriteWords>
struct Indexer : IndexerBase<IsWriteWords> {
    Indexer();
    explicit Indexer(std::unordered_map<size_t, std::unordered_set<char>> letters_by_level,
        const std::string& path_word_repository = IndexerBase<IsWriteWords>::kFileNameTrie);


    Ties::iterator SearchWord(const std::string& word) const;
    void AddWord(const std::string& word);
    void StartIndexer(const std::filesystem::path& directory_path);
    void SaveWordsFromFile(const std::filesystem::path& file_path, size_t& file_id);

    std::string StringIndex(size_t index) {
        return this->StringIndexFromUnMap(index);
    }

    Ties::iterator begin() const;
    Ties::iterator end() const;

    void SaveIndexer() {
        this->WriteTiesToBinFile();
        this->WriteIdDirectoryToBinFile();
    }
    
    ~Indexer();

    size_t file_id = 0;

};


