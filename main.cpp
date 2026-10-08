#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

//FROM CLASSROOM REQUIREMENTS
// 1) Implements file name & path validation on either the input file (.cpp) or output file (.html)
// 2) Reads in a C++ source file (.cpp)
// 3) Converts all < symbols to &lt and all > symbols to &gt;
// 4) Inserts the <PRE> and </PRE> tags to the front and end of the html file respectively.
// 5) Outputs the modified file as an html file

//OTHER REQUIREMENTS
// 1) Asks user for a directory to output to
// 2) Verifies Directory
// 3) Asks user for html file name
// 4) Verifies html file name
// 5) uses fin.fail and fout.fail
// 6) properly formats Html heading for display reasons
// 7) Asks user for name of CPP file being read
// 8) Creates html file in specsified directory
// 9) Makes sure there is no file with same name

//Upload string as HTML in given directory, returns error code
using namespace std;
namespace fs = std::filesystem;

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
    outFile.close();
    return 0;
}

//Adds opening and closing "<PRE>" tags and html metadata to string
static string prep_string_as_html(const string& body, const string& fileName)
{
    return "<!DOCTYPE html>\n<html>\n<head>\n<title>" + fileName +
        "</title>\n</head>\n<body>\n<PRE>\n<h1>" + fileName + "</h1>" +
        body + "</PRE>\n</body>\n</html>\n";
}

//Replaces the "<" and ">" symbol
string alter_cpp_for_html(ifstream& file)
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

//Get file from directory
static ifstream get_file(string readFilePath)
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

int main()
{
    string fileName = "test.html";
    string readFilePath = "../main.cpp";
    ifstream file = get_file(readFilePath);
    if (file.fail())
    {
        cout << "Error opening file\n";
        return -1;
    }
    string alteredFileString = alter_cpp_for_html(file);
    string htmlString = prep_string_as_html(alteredFileString, fileName);
    upload_string_as_html(htmlString, fileName, "../data");
}
