#include <gtest/gtest.h>
#include <algorithm>

#include "Indexer/Indexer.hpp"
#include "ParserArgument/ParserArgument.hpp"

const std::vector<std::string> array_words = {
    "apple", "banana", "car", "dog", "elephant", "flower", "grass", "house", "ice", "jacket",
    "kite", "lemon", "mountain", "night", "ocean", "piano", "queen", "river", "sun", "tree",
    "umbrella", "violin", "water", "xylophone", "yellow", "zebra", "ant", "boat", "cat", "door",
    "eagle", "fish", "garden", "hat", "island", "juice", "key", "lion", "moon", "nest",
    "orange", "penguin", "quilt", "rose", "star", "tiger", "unicorn", "vase", "whale", "x-ray",
    "yogurt", "zipper", "apricot", "butterfly", "cloud", "dolphin", "earth", "fire", "guitar", "helicopter",
    "internet", "jungle", "kangaroo", "laptop", "mango", "notebook", "octopus", "pumpkin", "quail", "rainbow",
    "snow", "train", "universe", "vacation", "window", "xenon", "yawn", "zephyr", "anchor", "balloon",
    "camera", "desert", "eraser", "fan", "giraffe", "happiness", "igloo", "jewel", "kettle", "leaf",
    "mirror", "needle", "ostrich", "popcorn", "quicksand", "rocket", "shell", "turtle", "umbra", "volcano"
};

void AddAndCheckWords(Indexer<true>& indexer, const std::vector<std::string>& words) {
    for (const auto& word : words) {
        indexer.AddWord(word);
    }
    
    for (const auto& word : words) {
        EXPECT_EQ(word.back(), *indexer.SearchWord(word));
    }
}

TEST(IndexerTest, IndexerWriteAndSearchWords) {
    Indexer<true> indexer;
    AddAndCheckWords(indexer, array_words);
}

TEST(IndexerTest, IndexerWriteAndSearchWordsFromFile) {
    Indexer<true> indexer;

    AddAndCheckWords(indexer, array_words);

    indexer.SaveIndexer();

    auto copy_array_words = array_words;
    std::sort(copy_array_words.begin(), copy_array_words.end());

    Indexer<false> indexer_from_file(ParserArgument::WordLeveling(copy_array_words));
    for (size_t i = 0; i < copy_array_words.size(); ++i) {
        EXPECT_EQ(copy_array_words[i][copy_array_words[i].size() - 1], 
                *indexer_from_file.SearchWord(copy_array_words[i]));
    }
}

TEST(IndexerTest, ReadBitTrie) {
    Indexer<true> indexer;

    AddAndCheckWords(indexer, array_words);

    indexer.SaveIndexer();

    
    auto copy_array_words = array_words;
    std::sort(copy_array_words.begin(), copy_array_words.end());

    std::vector<std::string> copy_bit_array_word;
    for (size_t i = 0; i < 43; ++i) {
        copy_bit_array_word.push_back(copy_array_words[i]);
    }

    Indexer<false> indexer_from_file(ParserArgument::WordLeveling(copy_bit_array_word));
    for (size_t i = 0; i < copy_bit_array_word.size(); ++i) {
        EXPECT_EQ(copy_bit_array_word[i][copy_bit_array_word[i].size() - 1], 
                *indexer_from_file.SearchWord(copy_bit_array_word[i]));
    }

    for (size_t i = 44; i < copy_array_words.size(); ++i) {
        EXPECT_EQ(indexer_from_file.SearchWord(copy_array_words[i]),
                indexer_from_file.end());
    }            
}

TEST(IndexerTest, IndexerWriteDirectoryId) {
    Indexer<true> indexer;

    AddAndCheckWords(indexer, array_words);

    std::unordered_map<size_t, std::string> id_directory = {
        {0, "/home/user/documents/report.docx"},
        {1, "/home/user/pictures/vacation/photo1.jpg"},
        {2, "/home/user/music/rock/song.mp3"},
        {3, "/home/user/videos/movies/movie.mp4"},
        {4, "/home/user/downloads/software/installer.exe"},
        {5, "/home/user/documents/work/project.xlsx"},
        {6, "/home/user/documents/notes.txt"},
        {7, "/home/user/pictures/family/photo2.png"},
        {8, "/home/user/music/pop/song2.mp3"},
        {9, "/home/user/videos/tutorials/tutorial1.avi"}
    };

    std::unordered_map<size_t, std::vector<size_t>> number_line {
        {0, {1, 2, 3, 4}},
        {1, {4, 21, 212, 453, 433}},
        {2, {434, 414, 32323, 333}},
        {3, {4, 1122, 2122, 3232, 323, 4343}},
        {4, {43, 131, 133, 1313, 1221}},
        {5, {5, 13, 32, 35454, 332, 344}}
    };

    for (size_t i = 0; i < array_words.size(); ++i) {
        auto iterator_word = indexer.SearchWord(array_words[i]);

        for (size_t j = 0; j < number_line[i % number_line.size()].size(); ++j) {
            iterator_word.insert(i % id_directory.size(), number_line[i % number_line.size()][j]);
        }
    }

    std::vector<size_t> ans;
    for (size_t i = 0; i < array_words.size(); ++i) {
        auto iterator_word = indexer.SearchWord(array_words[i]);

        for (auto it = iterator_word.GetStartArray(i % id_directory.size());
            it != iterator_word.GetEndArray(i % id_directory.size()); 
            ++it) {
            ans.push_back(*it);
        }

        std::sort(ans.begin(), ans.end());
        std::sort(number_line[i % number_line.size()].begin(), number_line[i % number_line.size()].end());
        for (size_t j = 0; j < number_line[i % number_line.size()].size(); ++j) {
            EXPECT_EQ(ans[j], number_line[i % number_line.size()][j]);
        }
        
        ans.clear();
    }
}

TEST(IndexerTest, IndexerWriteDirectoryIdFromFile) {
    Indexer<true> indexer;

    AddAndCheckWords(indexer, array_words);

    std::unordered_map<size_t, std::string> id_directory = {
        {0, "/home/user/documents/report.docx"},
        {1, "/home/user/pictures/vacation/photo1.jpg"},
        {2, "/home/user/music/rock/song.mp3"},
        {3, "/home/user/videos/movies/movie.mp4"},
        {4, "/home/user/downloads/software/installer.exe"},
        {5, "/home/user/documents/work/project.xlsx"},
        {6, "/home/user/documents/notes.txt"},
        {7, "/home/user/pictures/family/photo2.png"},
        {8, "/home/user/music/pop/song2.mp3"},
        {9, "/home/user/videos/tutorials/tutorial1.avi"}
    };

    std::unordered_map<size_t, std::vector<size_t>> number_line {
        {0, {1, 2, 3, 4}},
        {1, {4, 21, 212, 453, 433}},
        {2, {434, 414, 32323, 333}},
        {3, {4, 1122, 2122, 3232, 323, 4343}},
        {4, {43, 131, 133, 1313, 1221}},
        {5, {5, 13, 32, 35454, 332, 344}}
    };

    for (size_t i = 0; i < array_words.size(); ++i) {
        auto iterator_word = indexer.SearchWord(array_words[i]);

        for (size_t j = 0; j < number_line[i % number_line.size()].size(); ++j) {
            iterator_word.insert(i % id_directory.size(), number_line[i % number_line.size()][j]);
        }
    }

    indexer.SaveIndexer();

    Indexer<false> indexer_from_file(ParserArgument::WordLeveling(array_words));

    std::vector<size_t> ans;
    for (size_t i = 0; i < array_words.size(); ++i) {
        auto iterator_word = indexer_from_file.SearchWord(array_words[i]);

        for (auto it = iterator_word.GetStartArray(i % id_directory.size());
            it != iterator_word.GetEndArray(i % id_directory.size()); 
            ++it) {
            ans.push_back(*it);
        }

        std::sort(ans.begin(), ans.end());
        std::sort(number_line[i % number_line.size()].begin(), number_line[i % number_line.size()].end());
        for (size_t j = 0; j < number_line[i % number_line.size()].size(); ++j) {
            EXPECT_EQ(ans[j], number_line[i % number_line.size()][j]);
        }
        
        ans.clear();
    }

    for (size_t i = 0; i < array_words.size(); ++i) {
        auto iterator_word = indexer_from_file.SearchWord(array_words[i]);
        std::unordered_set<size_t> array_index = iterator_word.GetKeyArray();

        for (size_t index : array_index) {
            EXPECT_EQ(iterator_word.empty(index), false);
        }

    }
}

void CreateTestFile(const std::filesystem::path& path, const std::string& content) {
    std::ofstream file(path);
    file << content;
}

TEST(IndexerTest, StartIndexer_ValidDirectory) {
    std::filesystem::path test_dir = "test_dir";
    std::filesystem::create_directory(test_dir);

    std::filesystem::path file_path1 = test_dir / "file1.txt";
    CreateTestFile(file_path1, "This is a test file.\nAnother line.");

    std::filesystem::path file_path2 = test_dir / "file2.txt";
    CreateTestFile(file_path2, "Yet another test file.\nWith some more lines.");

    Indexer<true> indexer;
    EXPECT_NO_THROW(indexer.StartIndexer(test_dir));

    std::filesystem::remove_all(test_dir);
}

TEST(IndexerTest, StartIndexer_InvalidDirectory) {
    std::filesystem::path invalid_dir = "invalid_dir";

    Indexer<true> indexer;
    EXPECT_THROW(indexer.StartIndexer(invalid_dir), std::runtime_error);
}

// LOCAL TESTS
/*
TEST(IndexerTest, WriteAndReadIdDirectory) {
    IndexerBase<true> indexer;
    indexer.id_directory_ = {{1, "one"}, {2, "two"}, {3, "three"}};
    indexer.WriteIdDirectoryToBinFile();

    IndexerBase<true> indexer_read;
    indexer_read.ReadIdDirectoryFromBinFile();

    EXPECT_EQ(indexer_read.id_directory_.size(), 3);
    EXPECT_EQ(indexer_read.id_directory_[1], "one");
    EXPECT_EQ(indexer_read.id_directory_[2], "two");
    EXPECT_EQ(indexer_read.id_directory_[3], "three");
}


TEST(IndexerTest, EmptyDirectory) {
    IndexerBase<true> indexer;
    indexer.WriteIdDirectoryToBinFile();

    IndexerBase<true> indexer_read;
    indexer_read.ReadIdDirectoryFromBinFile();

    EXPECT_TRUE(indexer_read.id_directory_.empty());
}

TEST(IndexerTest, SingleElementDirectory) {
    IndexerBase<true> indexer;
    indexer.id_directory_ = {{42, "answer"}};
    indexer.WriteIdDirectoryToBinFile();

    IndexerBase<true> indexer_read;
    indexer_read.ReadIdDirectoryFromBinFile();

    ASSERT_EQ(indexer_read.id_directory_.size(), 1);
    EXPECT_EQ(indexer_read.id_directory_[42], "answer");
}
*/