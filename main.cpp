#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <regex>
#include <stdexcept>

//FROM CLASSROOM REQUIREMENTS
// 1) Implements file name & path validation on either the input file (.cpp) or output file (.html): DONE
// 2) Reads in a C++ source file (.cpp): DONE
// 3) Converts all < symbols to &lt and all > symbols to &gt;: DONE
// 4) Inserts the <PRE> and </PRE> tags to the front and end of the html file respectively.: DONE
// 5) Outputs the modified file as an html file: DONE

//OTHER REQUIREMENTS
// 1) Asks user for a directory to output to: DONE
// 2) Verifies Directory: DONE
// 3) Asks user for html file name: DONE
// 4) Verifies html file name: DONE
// 5) uses fin.fail and fout.fail: DONE
// 6) properly formats Html heading for display reasons: DONE
// 7) Asks user for name of CPP file being read
// 8) Creates html file in specified directory: DONE
// 9) Makes sure there is no file with same name: DONE

using namespace std;
namespace fs = std::filesystem;

//========================================================================================

/*Returns boolean if given string is valid windows directory
    ACCEPTS:
        C:\
        C:\Users
        C:\Users\Me\Documents\

    DECLINES:
        Users\Me
        C:\Us*ers
        C:\\Users
 */
static bool is_windows_directory(const string& path)
{
    static const regex windowsDirRegex(
        R"delim(^[a-zA-Z]:\\(?:[^\\/:*?"<>|]+\\)*[^\\/:*?"<>|]*$)delim"
    );
    if (path.empty())
    {
        cout << "Path is empty\n";
        return false;
    }
    if (path.length() >= 260)
    {
        cout << "Path is too long\n";
        return false;
    }
    if (!regex_match(path, windowsDirRegex))
    {
        cout << "Path is not a valid directory format\n";
        return false;
    }
    return true;
}

//Returns bool depending on if the path given is an existing one on the computer
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

//Loops though requesting user for a valid dictionary path
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
//Correct input/output: 'output.html','CppFile.html','Hello.html'
//Incorrect input: '.html', '$%#.html', '14.html'
static bool is_html_string(const string& fileName)
{
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

//returns valid windows file name for an html file given by user
//returns false if not valid name or inputs name of a file that already exist in chosen directory
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

// 5) Outputs the modified file as an html file
static UploadResult upload_string_as_html(const string& htmlString, const string& fileName, const string& dirPath)
{
    fs::path fullPath = fs::path(dirPath) / fileName;

    //Creates and opens file using 'fullPath'
    ofstream writingFile(fullPath);
    if (!writingFile) { return UploadResult::OpenFailed; }

    //writes htmlString to opened file
    writingFile << htmlString << endl;
    if (writingFile.fail()) { return UploadResult::WriteFailed; }

    //Closes file
    writingFile.close();
    if (writingFile.fail()) { return UploadResult::CloseFailed; }

    return UploadResult::Success;
}

//Adds opening and closing "<PRE>" tags and html metadata to string
static string prep_string_as_html(const string& body, const string& fileName)
{
    return "<!DOCTYPE html>\n<html>\n<head>\n<title>" + fileName +
        "</title>\n</head>\n<body>\n<h1>" + fileName + "</h1>\n<PRE>" +
        body + "</PRE>\n</body>\n</html>\n";
}

//Replaces the "<" and ">" symbol
static string alter_cpp_for_html(ifstream& file)
{
    string line, result;
    //Loops through each line
    while (getline(file, line))
    {
        //checks each character in line and adds
        for (const char c : line)
        {
            if (c == '<') result += "&lt;";
            else if (c == '>') result += "&gt;";
            else result += c;
        }
        //Maintains each line
        result += '\n';
    }
    return result;
}

//Get file from directory
static ifstream get_file(const string& readFilePath)
{
    //Error handling for reading file
    ifstream readFile(readFilePath);
    if (!readFile.is_open())
    {
        cout << "Error opening file\n";
        return ifstream("");
    }
    return readFile;
}

//========================================================================================

int main()
{
    //What cpp file we are translating
    const string readFilePath = "../OriginalCPP.cpp";

    //get directory to populate
    string validPath = get_valid_directory();

    //get name for html file
    string htmlName = get_html_name(validPath);

    ifstream file = get_file(readFilePath);

    if (!file.is_open())
    {
        cout << "Unable to open file for writing." << endl;
        return -1;
    }
    //Checks if the stream has failed (Fail bit or bad bit)
    if (file.fail())
    {
        cout << "Error writing to file or bad stream" << endl;
        return -1;
    }
    //4) Inserts the <PRE> and </PRE> tags to the front and end of the html file respectively.
    string alteredFileString = alter_cpp_for_html(file);

    //6) properly formats Html heading for display reasons:
    string htmlString = prep_string_as_html(alteredFileString, htmlName);

    // 5) Outputs the modified file as an html file
    UploadResult result = upload_string_as_html(htmlString, htmlName, validPath);
    if (result != UploadResult::Success)
    {
        cout << describe_upload_result(result) << endl;
        return -1;
    }

    return 0;
}
