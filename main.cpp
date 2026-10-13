#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <regex>
#include <stdexcept>

using namespace std;
namespace fs = std::filesystem;

//========================================================================================

static bool is_windows_directory(const string& path)
{
    //SOURCE: https://quickref.me/regex.html
    static const regex windowsDirRegex(
        R"delim(^[a-zA-Z]:\\(?:[^\\/:*?"<>|]+\\)*[^\\/:*?"<>|]*$)delim"
    );
    if (path.empty())           { cout << "Path is empty\n"; return false; }
    if (path.length() >= 260)   { cout << "Path is too long\n"; return false; }
    if (!regex_match(path, windowsDirRegex)) { cout << "Path is not a valid directory format\n"; return false; }
    return true;
}

static bool is_exsisting_directory(const string& validPath)
{
    cout << "\n=======================================================\n";

    error_code ec;
    bool dirExsist = fs::exists(validPath, ec) && fs::is_directory(validPath, ec);
    if (ec)
    {
        cout << "File System Error: " << ec.message() << endl;
        return false;
    }
    if (dirExsist)
    {
        cout << "Directory exists: '" << validPath << "'\n";
        return true;
    }
    cout << "'" << validPath << "' is not an existing directory, Try again" << endl;
    return false;
}

static string get_valid_directory()
{
    bool userInputValid = false;
    string userInput;

    while (!userInputValid)
    {
        cout << "\n=======================================================\n";
        cout << "Expected directory format: "
            "\n - \t C:\\"
            "\n - \t D:\\downloads"
            "\n - \t C:\\users\\me\\documents\n";
        cout << "Please enter a valid directory: ";
        getline(cin, userInput);
        if (is_windows_directory(userInput))
        {
            cout << "checking if '" << userInput << "' exists " << endl;
            userInputValid = is_exsisting_directory(userInput);
        }
    }
    return userInput;
}

//========================================================================================
static bool is_html_string(const string& fileName)
{
    //SOURCE https://stackoverflow.com/questions/6768779/test-filename-with-regular-expression
    static const regex htmlRegex(R"delim(^[A-Za-z_][A-Za-z0-9_]*\.html$)delim");

    if (fileName.empty())         { cout << "Name is empty\n"; return false; }
    if (fileName.length() >= 250) { cout << "Name too long\n"; return false; }
    if (!regex_match(fileName, htmlRegex)) { cout << "Name is not a valid html name\n"; return false; }
    return true;
}

static bool is_existing_html(const string& fileName, const string& directory)
{
    fs::path fullPath = fs::path(directory) / fileName;
    if (fs::exists(fullPath))
    {
        cout << "File already exists: " << fileName << ", try again\n";
        return true;
    }
    return false;
}

static string get_html_name(string chosenDirectory)
{
    bool userInputValid = false;
    string userInput;

    while (!userInputValid)
    {
        cout << "\n=======================================================\n";
        cout << "Expected file name format: "
            "\n - \t output.html"
            "\n - \t CppFile.html"
            "\n - \t Hello.html\n";
        cout << "Please enter a valid html file name: ";
        getline(cin, userInput);
        if (is_html_string(userInput))
        {
            cout << "checking if '" << userInput << "' already exists" << endl;
            if (!is_existing_html(userInput, chosenDirectory))
            {
                userInputValid = true;
            }
        }
    }
    return userInput;
}

//========================================================================================
// ENUM SOURCE:https://www.geeksforgeeks.org/cpp/enumeration-in-cpp/
enum class UploadResult
{
    Success,
    OpenFailed,
    WriteFailed,
    CloseFailed
};

static const char* describe_upload_result(UploadResult result)
{
    switch (result)
    {
    case UploadResult::OpenFailed:  return "Error opening output file.";
    case UploadResult::WriteFailed: return "Error writing to output file.";
    case UploadResult::CloseFailed: return "Error closing output file.";
    default:                        return "";
    }
}

static UploadResult upload_string_as_html(const string& htmlString, const string& fileName, const string& dirPath)
{
    fs::path fullPath = fs::path(dirPath) / fileName;

    ofstream writingFile(fullPath);
    if (!writingFile) { return UploadResult::OpenFailed; }

    writingFile << htmlString << endl;
    if (writingFile.fail()) { return UploadResult::WriteFailed; }

    writingFile.close();
    if (writingFile.fail()) { return UploadResult::CloseFailed; }

    return UploadResult::Success;
}

static string prep_string_as_html(const string& body, const string& fileName)
{
    return "<!DOCTYPE html>\n<html>\n<head>\n<title>" + fileName +
        "</title>\n</head>\n<body>\n<h1>" + fileName + "</h1>\n<PRE>" +
        body + "</PRE>\n</body>\n</html>\n";
}

static string alter_cpp_for_html(ifstream& file)
{
    string line, result;
    while (getline(file, line))
    {
        for (const char c : line)
        {
            if (c == '<') result += "&lt;";
            else if (c == '>') result += "&gt;";
            else result += c;
        }
        result += '\n';
    }
    return result;
}

static ifstream get_file(const string& readFilePath)
{
    ifstream readFile(readFilePath);
    if (!readFile.is_open())
    {
        cout << "Error opening file\n";
        return ifstream("");
    }
    return readFile;
}

//=======================================================================================//

static int run()
{
    const string readFilePath = "../OriginalCPP.cpp";

    string validPath = get_valid_directory();
    string htmlName  = get_html_name(validPath);

    ifstream file = get_file(readFilePath);
    if (file.fail()) { return 1; }

    string alteredFileString = alter_cpp_for_html(file);
    string htmlString        = prep_string_as_html(alteredFileString, htmlName);

    UploadResult result = upload_string_as_html(htmlString, htmlName, validPath);
    if (result != UploadResult::Success)
    {
        cout << describe_upload_result(result) << endl;
        return 1;
    }

    cout << "File uploaded successfully." << endl;
    return 0;
}
int main()
{
    try
    {
        return run();
    }
    catch (const exception& e)
    {
        cout << e.what() << endl;
        return 1;
    }
}
