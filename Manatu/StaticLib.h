#include <iostream>
#include <map>
using namespace std;


/* 将字符串解析为整数 */
int parseInt(string str) {
	int num = 0;
	int i = 0;
	for (; i < str.length(); i++) {
		if (str[i] < '0' || str[i] > '9') {
			break;
		} 
		num = num * 10 + (str[i] - '0');
	}
	if (i == 0) {
		string err = "Cannot parse string `";
		err += str;
		err += "` to Integer";
		throw err;
	}
	
	return num;
}

/* 将字符串解析为浮点数 */
double parseFloat(string str) {
	double num = 0;
	int i = 0;
	bool catchDigit = false;
	int digit = 0;
	for (; i < str.length(); i++) {
		if (str[i] == '.') {
			if (catchDigit) {
				string err = "Cannot parse string `";
				err += str;
				err += "` to Double";
				throw err;
			}
			else {
				catchDigit = true;
			}
			continue;
		}
		if (str[i] < '0' || str[i] > '9') {
			break;
		}
		num = num * 10 + (str[i] - '0');
		if (catchDigit) {
			digit++;
		}
	}
	if (i == 0) {
		string err = "Cannot parse string `";
		err += str;
		err += "` to Double";
		throw err;
	}
	while (digit--) {
		num *= 0.1;
	}
	
	return num;
}

string to_string(int num) {
	if (num == 0) {
		return "0";
	}
	string str = "";
	bool rev = false;
	if (num < 0) {
		rev = true;
		num = -num;
	}
	while (num) {
		int bt = num % 10;
		num = num / 10;
		str = char(bt + '0') + str;
	}
	if (rev) {
		return "-" + str;
	}
	else {
		return str;
	}
}

string to_string(double num) {
	if (num >= -1e-12 && num <= 1e-12) {
		return "0.0";
	}
	string str = "";
	bool rev = false;
	if (num < 0) {
		rev = true;
		num = -num;
	}
	int k = int(num);
	double r = num - k;
	if (k == 0) {
		str = "0";
	}
	while (k) {
		int bt = k % 10;
		k = k / 10;
		str = char(bt + '0') + str;
	}
	str += ".";
	int pos = 0;
	while (r > 1e-12 && pos++ < 12) {
		r *= 10;
		str += char(int(r) + '0');
		r -= int(r);
	}
	if (pos == 0) {
		str += "0";
	}
	if (rev) {
		return "-" + str;
	}
	else {
		return str;
	}
}


class Object;
class Number;
class Integer;
class Double;
class Operator;
class BlockSign;



class Object {
	protected:
		string* className;
		map<string, Object>* entryMap;
		int address;
		int classLength;
		int value;
		string str;
		double valuef;
	public:
		bool constant;
		bool defined;
		Object();
		Object(string info);
		string type();
		void extendedBy(string typeName);
		bool isInstanceOf(string typeName);
		bool typeIs(string typeName);
		int getValue();
		double getValuef();
		void setValue(int value);
		void setValuef(double value);
		void named(string key);
		string toString();
		Object operator[](string index);
};

class Number: public Object {
	public:
		Number();
		Number operator+(Number& b);
		Number operator-(Number& b);
		Number operator*(Number& b);
		Number operator/(Number& b);
		Number operator%(Number& b);
		Number operator+=(Number& b);
		Number operator-=(Number& b);
		Number operator*=(Number& b);
		Number operator/=(Number& b);
};

class Integer: public Number {
	public:
		Integer(int value);
		Integer(float value);
		Integer(double value);
		Integer(Number& b);
};

class Double: public Number {
	public:
		Double(int value);
		Double(float value);
		Double(double value);
		Double(Number& b);
};

class Operator: public Object {
	public:
		Operator(string str);
};

class BlockSign: public Object {
	public:
		BlockSign(char op);
};



Object::Object() {
	this->classLength = 1;
	this->className = new string[1];
	this->className[0] = "Object";
	this->value = 0;
	this->valuef = 0;
	this->str = "undefined";
	this->entryMap = new map<string, Object>;
	this->constant = true;
	this->defined = false;
}

Object::Object(string info) {
	this->classLength = 1;
	this->className = new string[1];
	this->className[0] = "Object";
	this->value = 0;
	this->valuef = 0;
	this->str = info;
	this->entryMap = new map<string, Object>;
	this->constant = true;
	this->defined = false;
}

Object Object::operator[](string index) {
	return (*this->entryMap)[index];
}

string Object::type() {
	return this->className[this->classLength - 1];
}

void Object::extendedBy(string typeName) {
	string* nextClass = new string[++this->classLength];
	for (int i = 0; i < this->classLength - 1; i++) {
		nextClass[i] = this->className[i];
	}
	delete this->className;
	this->className = nextClass;
	this->className[this->classLength - 1] = typeName;
}

int Object::getValue() {
	if (!this->defined) {
		cerr << this->toString() << " is not initialized." << endl;
		exit(-1);
	}
	return this->value;
}

double Object::getValuef() {
	if (!this->defined) {
		cerr << this->toString() << " is not initialized." << endl;
		exit(-1);
	}
	return this->valuef;
}

string Object::toString() {
	return this->str;
}

bool Object::isInstanceOf(string typeName) {
	for (int i = this->classLength - 1; i >= 0; i--) {
		if (this->className[i].compare(typeName) == 0) {
			return true;
		}
	}
	return false;
}

bool Object::typeIs(string typeName) {
	return this->type().compare(typeName) == 0 ? true : false;
}

void Object::setValue(int value) {
	if (this->constant) {
		if (!this->defined) {
			this->value = value;
			this->valuef = value;
			this->defined = true;
			if (this->typeIs("Object")) {
				this->extendedBy("Number");
			}
			if (this->typeIs("Number")){
				this->extendedBy("Integer");
			}
		}
		cerr << this->toString() << " is constant, resetting value is not allowed." << endl;
		exit(-1);
	}
	this->value = value;
	this->valuef = value;
	if (!this->defined) {
		this->defined = true;
		if (this->typeIs("Object")) {
			this->extendedBy("Number");
		}
		if (this->typeIs("Number")){
			this->extendedBy("Integer");
		}
	}
}

void Object::setValuef(double value) {
	if (this->constant) {
		if (!this->defined) {
			this->value = value;
			this->valuef = value;
			this->defined = true;
			if (this->typeIs("Object")) {
				this->extendedBy("Number");
			}
			if (this->typeIs("Number")){
				this->extendedBy("Double");
			}
		}
		cerr << this->toString() << " is constant, resetting value is not allowed." << endl;
		exit(-1);
	}
	this->value = value;
	this->valuef = value;
	if (!this->defined) {
		this->defined = true;
		if (this->typeIs("Object")) {
			this->extendedBy("Number");
		}
		if (this->typeIs("Number")){
			this->extendedBy("Double");
		}
	}
}

void Object::named(string key) {
	if (this->constant) {
		cerr << this->toString() << " is constant, renaming is not allowed." << endl;
		exit(-1);
	}
	this->str = key;
}


const Object null = *(new Object("null"));
const Object undefined = *(new Object());


Number::Number() {
	this->extendedBy("Number");
	this->value = 0;
	this->valuef = 0;
	this->str = "NaN";
}



Integer::Integer(int value) {
	this->value = value;
	this->valuef = value;
	this->str = to_string(value);
	this->extendedBy("Integer");
	this->defined = true;
}

Integer::Integer(float value) {
	this->value = int(value);
	this->valuef = int(value);
	this->str = to_string(int(value));
	this->extendedBy("Integer");
	this->defined = true;
}

Integer::Integer(double value) {
	this->value = int(value);
	this->valuef = int(value);
	this->str = to_string(int(value));
	this->extendedBy("Integer");
	this->defined = true;
}

Integer::Integer(Number& b) {
	this->value = b.getValue();
	this->valuef = b.getValue();
	this->str = to_string(b.getValue());
	this->extendedBy("Integer");
	this->defined = true;
}



Double::Double(int value) {
	this->value = value;
	this->valuef = value;
	this->str = to_string(double(value));
	this->extendedBy("Double");
	this->defined = true;
}

Double::Double(float value) {
	this->value = value;
	this->valuef = value;
	this->str = to_string(double(value));
	this->extendedBy("Double");
	this->defined = true;
}

Double::Double(double value) {
	this->value = value;
	this->valuef = value;
	this->str = to_string(value);
	this->extendedBy("Double");
	this->defined = true;
}

Double::Double(Number& b) {
	this->value = b.getValue();
	this->valuef = b.getValuef();
	this->str = to_string(double(this->valuef));
	this->extendedBy("Double");
	this->defined = true;
}



Operator::Operator(string str) {
	this->str = str;
	// 优先级 
	this->value = (str.compare("(") == 0 || str.compare(")") == 0) ? 0
		: (str.compare("[") == 0 || str.compare("]") == 0 || str.compare(".") == 0) ? 1
		: (str.compare("++") == 0 || str.compare("--") || str.compare("!") == 0 || str.compare("~")) == 0 ? 2
		: (str.compare("*") == 0 || str.compare("/") == 0 || str.compare("%") == 0) ? 3
		: (str.compare("+") == 0 || str.compare("-") == 0) ? 4
		: (str.compare("<<") == 0 || str.compare(">>") == 0) ? 5
		: (str.compare(">") == 0 || str.compare(">=") || str.compare("<") == 0 || str.compare("<=") == 0) ? 6
		: (str.compare("==") == 0 || str.compare("!=") == 0) ? 7
		: (str.compare("&") == 0) ? 8
		: (str[0] == '^') ? 9
		: (str.compare("|") == 0) ? 10
		: (str.compare("&&") == 0) ? 11
		: (str.compare("||") == 0) ? 12
		: (str[0] == '?' || str[0] == ':') ? 13
		: 14;
	this->valuef = this->value;
	this->extendedBy("Operator");
	this->defined = true;
}



BlockSign::BlockSign(char op) {
	this->str = op;
	this->extendedBy("BlockSign");
	this->defined = true;
}



string to_string(Object obj) {
	if (obj.isInstanceOf("Integer")) {
		return to_string(obj.getValue());
	}
	else if (obj.isInstanceOf("Double")) {
		return to_string(obj.getValuef());
	}
	else {
		return obj.toString();
	}
}



class Namespace {
	public:
		bool hasParent;
		Namespace* parent;
		Namespace();
		Namespace(Namespace& parent);
		~Namespace();
		map<string, Object> dir;
		Object* operator[](string key);
		void add(string key, const Object& value);
		void def(string key, const Object& value);
};

Namespace::Namespace() {
	this->hasParent = false;
}

Namespace::Namespace(Namespace& parent) {
	this->hasParent = true;
	this->parent = &parent;
}

Namespace::~Namespace() {
	this->dir.clear();
}

Object* Namespace::operator[](string key) {
	map<string, Object>::iterator iter = this->dir.find(key);
	if (iter == this->dir.end()) {
		if (this->hasParent) {
			return (*this->parent)[key];
		}
		else {
			return new Object(); // undefined
		}
	}
	else {
		return &(iter->second);
	}
}

void Namespace::add(string key, const Object& value) {
	map<string, Object>::iterator iter = this->dir.find(key);
	if (iter == this->dir.end()) {
		this->dir[key] = value;
	}
	else {
		cerr << "Doubledefination of variable `" << key << "`" << endl;
		exit(-1);
	}
}

void Namespace::def(string key, const Object& value) {
	map<string, Object>::iterator iter = this->dir.find(key);
	if (iter == this->dir.end()) {
		this->dir[key] = value;
		this->dir[key].constant = false;
		this->dir[key].named(key);
	}
	else {
		cerr << "Double defination of variable `" << key << "`" << endl;
		exit(-1);
	}
}


void loadStaticLib(Namespace* nsp) {
	nsp->add("null", null);
	nsp->add("undefined", undefined);
}



