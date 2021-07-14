#ifndef UTILS
#define UTILS

#include <io.h>
#include <string>
#include <vector>


using namespace std;

// string operations
void splitStr(const string& s, vector<string>& tokens, const char& delim);

void replaceStr(const string& src_str, const string &old_str, const string& new_str, string& ret, int count);


// file operations
const int getDirs(const string& path, vector<string>& dirs);

const int getFilesFormat(const string& path, const string& format, vector<string>& files);

#endif

