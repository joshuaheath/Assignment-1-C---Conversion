#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <regex>

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
// 7) Asks user for name of CPP file being read: I was too tierd
// 8) Creates html file in specified directory: DONE
// 9) Makes sure there is no file with same name: DONE

//Upload string as HTML in given directory, returns error code
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
static bool is_windows_dir(const std::string& path)
{
    static const regex windowsDirRegex(
        R"re(^[a-zA-Z]:\\(?:[^\\/:*?"<>|]+\\)*[^\\/:*?"<>|]*$)re"
    );
    //Early Return
    try
    {
        if (path == "") { throw invalid_argument("path is empty"); }

        if (path.length() >= 260) { throw invalid_argument("path is too long"); };
    }
    catch (invalid_argument& e)
    {
        cout << "Invalid argument: " << e.what() << endl;
        return false;
    }

    try
    {
        if (!regex_match(path, windowsDirRegex)) { throw invalid_argument("path is not a directory"); }
        return true;
    }
    catch (invalid_argument& e)
    {
        cout << "Invalid argument: " << e.what() << endl;
        return false;
    }
}

//Returns bool depending on if the path given is an existing one on the computer
static bool is_exsisting_directory(const string& validPath)
{
    cout << "\n=======================================================\n";
    if (filesystem::exists(validPath) && filesystem::is_directory(validPath))
    {
        cout << "Directory exists: " << validPath << endl;
        return true;
    }
    cout << "'" << validPath << "' is not an existing directory, Try again" << endl;
    return false;
}

//Loops though requesting user for a valid dictionary path
static string get_valid_directory()
{
    bool userInputValid = false;
    bool validDirString = false;
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
        if (is_windows_dir(userInput))
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
static bool is_html_string(string fileName)
{
    static const regex htmlRegex(R"re(^[A-Za-z_][A-Za-z0-9_]*\.html$)re");

    try
    {
        if (fileName == "") { throw invalid_argument("Name is empty"); }

        if (fileName.length() >= 260) { throw invalid_argument("Name too long"); };
    }
    catch (invalid_argument& e)
    {
        cout << "Invalid argument: " << e.what() << endl;
        return false;
    }

    try
    {
        if (!regex_match(fileName, htmlRegex)) { throw invalid_argument("Name is not a valid html name"); }
        return true;
    }
    catch (invalid_argument& e)
    {
        cout << "Invalid argument: " << e.what() << endl;
        return false;
    }
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
    bool validNameString = false;
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

// 5) Outputs the modified file as an html file
static int upload_string_as_html(const string& htmlString, const string& fileName, const string& dirPath)
{
    //Construct full filepath
    fs::path fullPath = fs::path(dirPath) / fileName;

    //Creates and opens file
    ofstream outFile(fullPath);
    if (!outFile)
    {
        cout << "Unable to open file for writing." << endl;
        return -1;
    }
    outFile << htmlString << endl;
    //Checks if the stream has failed (Failbit or bad)
    if (outFile.fail())
    {
        cout << "Error writing to file or bad stream" << endl;
        return -1;
    }
    //Closes file
    outFile.close();
    cout << "File uploaded successfully." << endl;
    return 0;
}

//Adds opening and closing "<PRE>" tags and html metadata to string
static string prep_string_as_html(const string& body, const string& fileName)
{
    return "<!DOCTYPE html>\n<html>\n<head>\n<title>" + fileName +
        "</title>\n</head>\n<body>\n<PRE>\n<h1>" + fileName + "</h1>\n" +
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
    upload_string_as_html(htmlString, htmlName, validPath);

    //Close file being read from
    file.close();
}
