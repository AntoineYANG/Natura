#include <iostream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <fstream>
using namespace std;

string checkFilename(const string filename, const string extension);
string compile(string code);
bool translate(const string fileName);

char stops[] = {
	' ', '\t', '\r', '\n', '\0', '~', '`', '!', '@', '#', '$', '%', '^', '*', '(', ')',
	'{', '}', ';', ':', '"', '\'', '<', ',', '>', '.', '?', '/', '&', '-', '+', '=', '|'
};

struct object {
	string symbol;
	string type;
};

//enum OriginType {
//	number, string, object
//};

enum CharType {
	none, oprt, front, letter, number, command, end, escape
};

vector<object> symbols;
map<string, object> lib;


int main(int argc, char** argv) {
	string originFile = *(argv + 1);
	if (!translate(originFile)) {
		return 0;
	}

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

bool translate(const string fileName) {
	symbols = *(new vector<object>());
	lib = *(new map<string, object>());
	ifstream manatu;
	manatu.open(checkFilename(fileName, "mnt").c_str(), ios::in);
	if (!manatu.is_open()) {
		cerr << "Error: Can not find a file named `" << checkFilename(fileName, "mnt") << "`." << endl;
		return false;
	}
	string data = "";
	while (getline(manatu, data)) {
		string res = compile(data);
	}
	manatu.close();
	
	symbols.clear();
	lib.clear();
	
	return true;
}

string compile(string code) {
	string smb = "";
	CharType ct = none;
	for (string::iterator it = code.begin(); it != code.end(); it++) {
		bool toContinue = true;
		if (ct == oprt && smb.length() == 1) {
			if ((smb[0] == '&' && *it == '&') || (smb[0] == '-' && *it == '-')
			|| (smb[0] == '+' && *it == '+') || (smb[0] == '|' && *it == '|')) {
				smb += *it;
				continue;
			}
			else if ((smb[0] == '+' || smb[0] == '-') 
			&& (symbols.size() && symbols[symbols.size() - 1].type.compare("number") != 0)) {
				smb += *it;
				ct = number;
				continue;
			}
			else {
				toContinue = false;
			}
		}
		else if (ct == number && *it == '.') {
			smb += *it;
			continue;
		}
		else {
			for (int i = 0; i < 33; i++) {
				if (*it == stops[i]) {
					toContinue = false;
					break;
				}
			}
		}
		if (toContinue) {
			if ((*it >= '0' && *it <= '9')) {
				if (ct == none) {
					ct = number;
				}
				else if (ct == letter || ct == number) {
					smb += *it;
				}
				else {
					toContinue = false;
				}
			}
			else if ((*it >= 'A' && *it <= 'Z') || (*it >= 'a' && *it <= 'z') || *it == '_') {
				if (ct == none || ct == letter || ct == front) {
					smb += *it;
					ct == letter;
				}
				else if (ct == command) {
					smb += *it;
				}
				else if (ct == number && (*it == 'e' || *it == 'E')) {
					smb += *it;
				}
				else {
					toContinue = false;
				}
			}
			else if (*it == '\\') {
				// do nothing
			}
			else {
				cout << "unexpected continuing char: " << *it << endl;
			}
		}
		if (!toContinue) {
			if (smb.length()) {
				object obj;
				obj.type = (ct == 0 ? "none"
				: ct == 1 ? "operator"
					: ct == 2 ? "front-only"
						: ct == 3 ? "symbol"
							: ct == 4 ? "number"
								: ct == 5 ? "command"
									: ct == 6 ? "end"
										: "escape");
				obj.symbol = smb;
				symbols.push_back(obj);
				cout << "{type: '" <<
					(ct == 0 ? "none"
						: ct == 1 ? "operator"
							: ct == 2 ? "front-only"
								: ct == 3 ? "symbol"
									: ct == 4 ? "number"
										: ct == 5 ? "command"
											: ct == 6 ? "end"
												: "escape")
				<< "', symbol: '" << smb << "'}" << endl;
			}
			if (*it != ' ' && *it != '\t' && *it != '\r' && *it != '\n' && *it != '\0') {
				smb = *it;
				if (*it >= '0' && *it <= '9') {
					ct = number;
					smb = *it;
				}
				else if ((*it >= 'A' && *it <= 'Z') || (*it >= 'a' && *it <= 'z') || *it == '_') {
					ct = letter;
					smb = *it;
				}
				else if (*it == '\\') {
					// do nothing
				}
				else if (*it == '$') {
					ct = front;
					smb = *it;
				}
				else if (*it == '@' || *it == '#') {
					ct = command;
					smb = *it;
				}
				else if (*it == '!' || *it == '%' || *it == '^' || *it == '&' || *it == '*'
				|| *it == '(' || *it == ')' || *it == '-' || *it == '+' || *it == '=' || *it == '{'
				|| *it == '[' || *it == '}' || *it == ']' || *it == '|' || *it == '<' || *it == '>'
				|| *it == '?') {
					smb = *it;
					ct = oprt;
				}
				else if (*it == ';') {
					ct = end;
				}
			}
			else {
				smb = "";
			}
		}
	}
	if (smb.length()) {
		object obj;
		obj.symbol = smb;
		obj.type = (ct == 0 ? "none"
				: ct == 1 ? "operator"
					: ct == 2 ? "front-only"
						: ct == 3 ? "symbol"
							: ct == 4 ? "number"
								: ct == 5 ? "command"
									: ct == 6 ? "end"
										: "escape");
		symbols.push_back(obj);
		cout << "{type: '" <<
			(ct == 0 ? "none"
				: ct == 1 ? "operator"
					: ct == 2 ? "front-only"
						: ct == 3 ? "symbol"
							: ct == 4 ? "number"
								: ct == 5 ? "command"
									: ct == 6 ? "end"
										: "escape")
		<< "', symbol: '" << smb << "'}" << endl;
	}
	
	return "";
}

