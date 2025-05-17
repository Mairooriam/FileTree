#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
namespace Mir {
namespace Utils{
    namespace File
    {
        std::ifstream openFile(const std::filesystem::path &filepath);
        std::vector<std::string> readLines(const std::filesystem::path& filepath);
        std::string readFile(const std::filesystem::path& filepath);
        std::vector<std::string> readLines(std::istream& stream);
        std::string readFile(std::istream& stream);
        std::string getFileExtension(const std::filesystem::path& filepath);
        bool hasExtension(const std::filesystem::path& filepath, const std::string& extension);
        std::vector<std::vector<std::string>> readCsv(const std::filesystem::path& _filepath);
        std::vector<std::string> parseCsvLine(const std::string& line, char delimiter = ',');

   
    } // namespace File
    namespace Text
    {
        std::vector<std::string> splitAt(const std::string& line, char delimeter = ' ');
        std::string getFirstCleanString(const std::string& str);
        std::string findNumbers(const std::string& str);
        void toLowerCase(std::string& str);
        std::string toLowerCase(const std::string& str);
        // Modifies the original & returns numbers as string.
        std::string findNumbersAConsume(std::string& str);
        std::pair<std::string, std::string> splitCharsFromNums(const std::string& str);
        std::string filterNumbers(const std::string& str);
        void replacePlaceholderIf(std::string& str, const std::string& placeholder, const std::string& replaceStr, const char oChar = '{', const char cChar = '}' );
        std::string findFirstCleanString(const std::string& str);
        std::vector<std::string> findAllCleanString(const std::string& _str);
        std::string findMatchFrom(const std::string& _str, const std::vector<std::string>& _from);
        std::string findIOdatatype(const std::string& _str);
        std::string findCleanStringAt(const std::string& _str, const int& _pos);
        std::vector<std::string> extractAllBetweenCurlyBraces(const std::string& str);
        std::string removeCharactersFromStr(const std::string& _str, const std::string_view _chars);
        std::string removeCharactersFromStr(const std::string& _str, const std::string_view _chars, const std::string& _replacement);
        
    } // namespace String
    

    template<typename MapType, typename KeyType>
    bool mapContains(const MapType& map, const KeyType& key)
    {
                // Try direct lookup first
        if (map.find(key) != map.end())
            return true;
        
        // If key is a string, check for BOM prefixed version
        if constexpr (std::is_convertible_v<KeyType, std::string_view>) {
            std::string bomKey = "\xEF\xBB\xBF" + std::string(key);
            return map.find(bomKey) != map.end();
        }
    
    return false;
    }





} // namespace Utils
} // Namepsace Mir