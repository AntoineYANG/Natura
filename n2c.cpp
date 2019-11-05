#include <iostream>
#include <stdlib.h>
#include <fstream>
using namespace std;

string checkFilename(const string filename, const string extension);
int compileCpp(const string fileName);
int execute(const string fileName);
void translate(const string fileName);


int main(int argc, char** argv) {
	string originFile = *(argv + 1);
	translate(originFile);
    compileCpp(originFile);
    execute(originFile);

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
	string request = "g++ " + checkFilename(filename, "cpp");
	return system(request.c_str());
}

int execute(const string fileName) {
	return system(checkFilename(fileName, "exe").c_str());
}

void translate(const string fileName) {
	ifstream natura;
	natura.open(checkFilename(fileName, "natura").c_str(), ios::in);
	string data = "";
	natura >> data;
	cout << data << endl;
	
	natura.close();
	return;
}

