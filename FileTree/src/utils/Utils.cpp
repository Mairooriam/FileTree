#include "Utils.h"
#include <algorithm>
#define MIR_ERROR 
#define MIR_WARN

namespace Mir {
    namespace Utils {
        namespace File {
            std::string getFileExtension(const std::filesystem::path& filepath) {
                return filepath.extension().string();
            }

            bool hasExtension(const std::filesystem::path& filepath, const std::string& extension) {
                std::string fileExt = getFileExtension(filepath);

                std::transform(fileExt.begin(), fileExt.end(), fileExt.begin(),
                               [](unsigned char c) { return std::tolower(c); });

                std::string extensionLower = extension;
                std::transform(extensionLower.begin(), extensionLower.end(), extensionLower.begin(),
                               [](unsigned char c) { return std::tolower(c); });

                if (!extensionLower.empty() && extensionLower[0] != '.') {
                    extensionLower = "." + extensionLower;
                }

                return fileExt == extensionLower;
            }

            std::vector<std::vector<std::string>> readCsv(const std::filesystem::path& _filepath) {
                std::vector<std::vector<std::string>> result;
                std::ifstream file(_filepath);
                if (!file.is_open()) {
                    MIR_ERROR("Failed to open CSV file: {0}", _filepath.string());
                    return result;
                }
                

                std::string content{std::istreambuf_iterator<char>(file), 
                                    std::istreambuf_iterator<char>()};
                            
                if (content.empty()) {
                    return result;
                }
                
                std::istringstream stream(content);
                std::string line;
                std::string accumulatedLine; // just for handling line break
                bool inQuotes = false;
                
                while (std::getline(stream, line)) {
                    for (char c : line) {
                        if (c == '"') {
                            
                            inQuotes = !inQuotes;
                        }
                    }
                    
                    if (accumulatedLine.empty()) {
                        accumulatedLine = line;
                    } else {
                        accumulatedLine += "\n" + line;
                    }
                    
                    if (!inQuotes) {
                        result.push_back(parseCsvLine(accumulatedLine, ','));
                        accumulatedLine.clear();
                    }
                }
                
                if (!accumulatedLine.empty()) {
                    result.push_back(parseCsvLine(accumulatedLine, ','));
                }
                return result;
            }

            std::vector<std::string> parseCsvLine(const std::string& _row, char _delimeter) {
                std::vector<std::string> fields;
                std::string field;
                bool inQuotes = false;
                
                for (char c : _row) {
                    if (!inQuotes && c == _delimeter) {
                        fields.push_back(field);
                        field.clear();
                    } else if (c == '"') {
                        inQuotes = !inQuotes;
                    } else {
                        field += c;
                    }
                }
                
                fields.push_back(field);
                
                return fields;
            }

            std::ifstream openFile(const std::filesystem::path& filepath) {
                std::ifstream file(filepath);

                if (!file.is_open()) {
                    std::string errorMsg = "Failed to open file: " + filepath.string();
                    if (std::error_code ec; !std::filesystem::exists(filepath, ec)) {
                        errorMsg += " [file does not exist]";
                    } else if (!std::filesystem::is_regular_file(filepath, ec)) {
                        errorMsg += " [not a regular file]";
                    } else {
                        errorMsg += " [possible permission issue]";
                    }
                    MIR_ERROR("Error opening file: {0}", errorMsg);
                }

                return file;
            }

            std::vector<std::string> readLines(std::istream& stream) {
                std::vector<std::string> lines;
                std::string line;

                while (std::getline(stream, line)) {
                    if (!line.empty() && line.back() == '\r') {
                        line.pop_back();
                    }
                    lines.push_back(line);
                }

                return lines;
            }

            std::vector<std::string> readLines(const std::filesystem::path& filepath) {
                std::ifstream file = openFile(filepath);

                if (!file.is_open()) {
                    return {};
                }

                return readLines(file);
            }

            std::string readFile(std::istream& stream) {
                if (!stream.good()) {
                    return "";
                }
                
                std::stringstream buffer;
                buffer << stream.rdbuf();
                return buffer.str();
            }

            std::string readFile(const std::filesystem::path& filepath) {
                std::ifstream file = openFile(filepath);

                if (!file.is_open()) {
                    return "";
                }

                return readFile(file);
            }

        }  // namespace File
        namespace Text {
            std::vector<std::string> splitAt(const std::string& line, char delimiter) {
                std::vector<std::string> tokens;
                std::string token;
                bool inQuotes = false;

                for (char c : line) {
                    if (c == '"') {
                        inQuotes = !inQuotes;
                    } else if (c == delimiter && !inQuotes) {
                        tokens.push_back(token);
                        token.clear();
                    } else {
                        token += c;
                    }
                }

                tokens.push_back(token);

                return tokens;
            }
            std::string getFirstCleanString(const std::string& str) {
                std::string result;
                for (const char& c : str) {
                    if (!isalnum(c))
                    {
                        // skip weird chars before alph
                        if (!result.size() == 0)
                        {
                            return result;
                        }
                    } else {
                        result += c;
                    }
                }
                return result;
            }
            
            std::string findNumbers(const std::string& str) {
                std::string result;
                for (char c : str) {
                    if (std::isdigit(c)) {
                        result += c;
                    }
                }
                return result;
            }
            void toLowerCase(std::string& str) {
                std::transform(str.begin(), str.end(), str.begin(),
                    [](unsigned char c) { return std::tolower(c); });
            }
            std::string toLowerCase(const std::string& str) {
                std::string result = str;
                std::transform(result.begin(), result.end(), result.begin(),
                    [](unsigned char c) { return std::tolower(c); });
                return result;
            }
            std::string findNumbersAConsume(std::string& str) {
                std::string result;
                std::string newStr;

                for (char c : str) {
                    if (std::isdigit(c)) {
                        result += c;
                    } else {
                        newStr += c;
                    }
                }

                str = newStr;
                return result;
            }
            std::pair<std::string, std::string> splitCharsFromNums(const std::string& str) {
                std::string chars;
                std::string nums;
                
                for (char c : str) {
                    if (std::isdigit(c)) {
                        nums += c;
                    } else {
                        chars += c;
                    }
                }
                
                return std::make_pair(chars, nums);
            }
            std::string filterNumbers(const std::string& str) {
                std::string result;
                for (char c : str) {
                    if (!std::isdigit(c)) {
                        result += c;
                    } 
                }
                return result;
            }

            void replacePlaceholderIf(std::string& str,const std::string& placeholder, const std::string& replaceStr, const char oChar, const char cChar ) {
                size_t startPos = 0;
                while ((startPos = str.find(oChar, startPos)) != std::string::npos) {
                    size_t endPos = str.find(cChar, startPos);
                    if (endPos == std::string::npos) {
                        break;
                    }
                    std::string foundStr = str.substr(startPos + 1, endPos - startPos - 1);
                    if (foundStr == placeholder) {   
                        str.replace(startPos, endPos - startPos + 1, replaceStr);
                        startPos += replaceStr.length();
                    } else {
                        // if not correct ifStr skip
                        startPos = endPos + 1; 
                    }
                }
            }

            std::string findFirstCleanString(const std::string& str){
                std::string result;
                for (const char& c : str) {
                    if (!isalnum(c))
                    {
                        // skip weird chars before alph
                        if (!result.size() == 0)
                        {
                            return result;
                        }
                    } else {
                        result += c;
                    }
                }
                return result;
            }

            std::vector<std::string> findAllCleanString(const std::string& _str) {
                std::vector<std::string> results;
                std::string currentWord;
                
                for (const char& c : _str) {
                    if (std::isalnum(c)) {
                        currentWord += c;
                    } else if (!currentWord.empty()) {
                        results.push_back(currentWord);
                        currentWord.clear();
                    }
                }
                
                if (!currentWord.empty()) {
                    results.push_back(currentWord);
                }
                
                return results;
            }
            

            std::string findMatchFrom(const std::string& _str, const std::vector<std::string>& _from) {
                
                std::string lowerStr = _str;
                std::ranges::transform(lowerStr, lowerStr.begin(), 
                    [](unsigned char c){ return std::tolower(c); });
                for (const auto &str : _from){
                    if (lowerStr.contains(str)){
                        return str;
                    }
                }
                return std::string();
            }

            std::string findIOdatatype(const std::string& _str) {
                return findMatchFrom(_str, {"ai", "ao", "di", "do"});
            }

            std::string findCleanStringAt(const std::string& _str, const int& _pos) {
                std::string result;
                for (size_t i = 1; i < _pos + 1; i++)
                {
                    for (const char& c : _str) {
                        if (!isalnum(c))
                        {
                            // skip weird chars before alph
                            if (!result.size() == 0)
                            {
                                if (i == _pos)
                                {
                                    return result;
                                }else {
                                    i++;
                                    result = "";
                                }
                            }
                        } else {
                            result += c;
                        }
                    }
                    if (i == _pos)
                    {
                        return result;
                    }else {
                        i++;
                        result = "";
                    }
                }
                return "{overflow}";
            }

            std::vector<std::string> extractAllBetweenCurlyBraces(const std::string& str) {
                std::vector<std::string> results;
                std::string::size_type startPos = 0;
                while (startPos < str.length()) {
                    std::string::size_type openBrace = str.find('{', startPos);
                    if (openBrace == std::string::npos) break;
                    std::string::size_type closeBrace = str.find('}', openBrace + 1);
                    if (closeBrace == std::string::npos) break;
                    results.push_back(str.substr(openBrace + 1, closeBrace - openBrace - 1));
                    startPos = closeBrace + 1;
                }
                
                return results;
            }

            std::string removeCharactersFromStr(const std::string& _str, const std::string_view _chars) {
                if (_str.empty()){ MIR_WARN("[Utils::Text::removeCharactersFrom] was supplied with empty _str"); return _str;}
                if (_chars.empty()) {  MIR_WARN("[Utils::Text::removeCharactersFrom] was supplied with empty _chars"); return _str;}

                std::string result;
                result.reserve(_str.size());
                
                for (char c : _str) {
                    if (!_chars.contains(c)) {
                        result += c;
                    }
                }
                return result;
            }
            std::string removeCharactersFromStr(const std::string& _str, const std::string_view _chars, const std::string& _replacement) {
                if (_str.empty()){ MIR_WARN("[Utils::Text::removeCharactersFrom] was supplied with empty _str"); return _str;}
                if (_chars.empty()) {  MIR_WARN("[Utils::Text::removeCharactersFrom] was supplied with empty _chars"); return _str;}

                std::string result;
                result.reserve(_str.size());
                
                for (char c : _str) {
                    if (!_chars.contains(c)) {
                        result += c;
                    }else{
                        result += _replacement;
                    }
                }
                return result;
            }
        }  // namespace Text

    }  // namespace Utils
}  // Namespace Mir