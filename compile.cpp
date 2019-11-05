#include <iostream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

string checkFilename(const string filename, const string extension);
string compile(string code);
int compileCpp(const string fileName);
int execute(const string fileName);
void loadDict();
bool translate(const string fileName);

vector<string> words = *(new vector<string>());

map<string, string> nsp = *(new map<string, string>());


int main(int argc, char** argv) {
	loadDict();
	string originFile = *(argv + 1);
	if (!translate(originFile)) {
		return 0;
	}
//    compileCpp(originFile);
//    execute(originFile);

    return 0;
}


string checkFilename(const string filename, const string extension) {
	unsigned short size = filename.length();
	string::size_type point = filename.find(".");
	if (point == string::npos) {
		return filename + "." + extension;
	}
	else {
		return filename.substr(0, point) + "." + extension;
	}
}

int compileCpp(const string filename) {
	string request = "g++ -o " + checkFilename(filename, "exe") + " " + checkFilename(filename, "cpp");
	return system(request.c_str());
}

int execute(const string fileName) {
	return system(fileName.c_str());
}

bool translate(const string fileName) {
	ifstream natura;
	natura.open(checkFilename(fileName, "natura").c_str(), ios::in);
	if (!natura.is_open()) {
		cerr << "Error: Can not find a file named `" << checkFilename(fileName, "natura") << "`." << endl;
		return false;
	}
	ofstream cpp;
	cpp.open(checkFilename(fileName, "cpp").c_str(), ios::out);
	string data = "";
	int i = 0;
	while (getline(natura, data)) {
		string cppSentence = compile(data);
		if (cppSentence.length()) {
			cpp << cppSentence << endl;
		}
	}
	natura.close();
	cpp.close();
	
	return true;
}

bool isEmpty(const char c) {
	return c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\0';
}

bool isSemicolon(const char c) {
	return c == ';';
}

int start = 0;
char skipSpace = '\0';
bool escape = false;

string compile(string code) {
	string ready = "";
	for (string::iterator it = code.begin(); it != code.end(); it++) {
		if (skipSpace == '\0') {
			if (isEmpty(*it)) {
				if (ready.length() > 0) {
					words.push_back(ready);
					ready = "";
				}
			}
			else if (isSemicolon(*it)) {
				if (ready.length() > 0) {
					words.push_back(ready);
					ready = "";
				}
				words.push_back(";");
			}
			else {
				if (!escape) {
					if (*it == '"' || *it == '\'' || *it == '`') {
						skipSpace = *it;
					}
				}
				else if (*it == '\\') {
					escape = true;
				}
				escape = *it == '\\';
				if (ready.length()) {
					if (ready[0] >= '0' && ready[0] <= '9') {
						if ((*it >= '0' && *it <= '9') || *it == '.') {
							ready += *it;
						}
						else {
							if (ready.length() > 0) {
								words.push_back(ready);
								ready = *it;
							}
						}
					}
					else if ((ready[0] >= 'a' && ready[0] <= 'z') || (ready[0] >= 'A' && ready[0] <= 'Z') || ready[0] == '_' || ready[0] == '$' || ready[0] == '@') {
						if ((*it >= 'a' && *it <= 'z') || (*it >= 'A' && *it <= 'Z') || *it == '_' || *it == '$' || *it == '@') {
							ready += *it;
						}
						else {
							if (ready.length() > 0) {
								words.push_back(ready);
								ready = *it;
							}
						}
					}
				}
				else {
					ready += *it;
				}
			}
		}
		else {
			if (!escape && *it == skipSpace) {
				if (ready.length() > 0) {
					words.push_back(ready);
					ready = "";
				}
				skipSpace = '\0';
			}
			escape = *it == '\\';
			ready += *it;
		}
	}
	if (ready.length() > 0) {
		words.push_back(ready);
		ready = "";
	}
	
	string target = "";
	for (vector<string>::iterator it = words.begin(); it != words.end(); it++) {
		string t = *it;
		if (t.length() == 0 || isEmpty(t[0])) {
			continue;
		}
		if (it == words.begin() && t[0] == '@') {
			cout << "[" << t << "]" << endl;
			target += t;
		}
		else if ((t[0] >= 'a' && t[0] <= 'z')|| (t[0] >= 'A' && t[0] <= 'Z')) {
			if (it != words.begin()) {
				target += " ";
			}
			if (nsp.find(t) != nsp.end()) {
				t = nsp[t];
//				target += t;
			}
			target += t;
		}
		else if (t[0] >= '0' && t[0] <= '9') {
			if (it != words.begin()) {
				target += " ";
			}
			if (t.length()) {
				target += t;
			}
		}
		else {
			if (t.length()) {
				target += t;
			}
		}
	}
	words.clear();
	
	return target;
}

void loadDict() {
	nsp["import"] = "#include";
	nsp["@Main"] = "int main";
	nsp["exit(0)"] = "return 0";
	nsp["=>"] = "";
}

