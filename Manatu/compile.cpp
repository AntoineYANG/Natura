#include <iostream>
#include <stdlib.h>
#include <vector>
#include <map>
#include <fstream>
#include "StaticLib.h"
#include "LinkStack.h"
#include "LinkList.h"
using namespace std;

string checkFilename(const string filename, const string extension);
string compile(string code);
bool translate(const string fileName);
Object eval(string formula);
Object compute(LinkList<Object> tokens);


Namespace Globe = *(new Namespace());
Namespace* curNsp = &Globe;


int main(int argc, char** argv) {
//	string originFile = *(argv + 1);
	string originFile = "test2";
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
	ifstream manatu;
	manatu.open(checkFilename(fileName, "mnt").c_str(), ios::in);
	if (!manatu.is_open()) {
		cerr << "Error: Can not find a file named `" << checkFilename(fileName, "mnt") << "`." << endl;
		return false;
	}
	loadStaticLib(&Globe);
	string data = "";
	while (getline(manatu, data)) {
		string res = compile(data);
	}
	manatu.close();
	
	return true;
}

string totalLine = "";

string compile(string code) {
	for (string::iterator it = code.begin(); it != code.end(); it++) {
		// �Էֺŷָ���� 
		if (*it == ';') {
			Object res = eval(totalLine);
			if (res.toString().compare("NaN") != 0 && res.defined) {
				cout << to_string(res) << endl;
			}
			totalLine = "";
		}
		else {
			totalLine += *it;
		} 
	}
	
	return "";
}


enum TokenType {
	$d,				// ������
	$f,				// ��������
	$oprt,			// �����
	$block,			// �����
	$name,			// ���� 
	$def,			// ������������ 
	$cst,			// ������������
	$type,			// �������� 
};

/* ������������Ϊ�����б� */ 
Object eval(string formula) try {
    LinkList<Object> tokens = *(new LinkList<Object>());
    string token = "";
    TokenType type;
    string varname = "";
	for (string::iterator it = formula.begin(); it < formula.end(); it++) {
		// ���ַ��ض����� 
		if (*it == ' ' || *it == '\t' || *it == '\r' || *it == '\n' || *it == '\0') {
			if (token.length()) {
				Object tk;
				if (type == $d) {
					tk = *(new Integer(parseInt(token)));
					token = "";
					tokens.push_back(tk);
				}
				else if (type == $f) {
					tk = *(new Double(parseFloat(token)));
					token = "";
					tokens.push_back(tk);
				}
				else if (type == $oprt) {
					tk = *(new Operator(token));
					token = "";
					tokens.push_back(tk);
				}
				else if (type == $name) {
					if (token.compare("let") == 0) {
						type = $def;
						token = "";
					}
					else if (token.compare("const") == 0) {
						type = $cst;
						token = "";
					}
					else {
						tk = *((*curNsp)[token]);
						if (tk.typeIs("Object") && tk.toString().compare("undefined") == 0) {
							cerr << token << " is undefined." << endl;
							exit(0);
						}
						tokens.push_back(tk);
						token = "";
					}
				}
				else if (type == $def) {
					curNsp->def(token, *(new Object()));
					tk = *((*curNsp)[token]);
					tokens.push_back(tk);
					token = "";
					type = $name;
				}
				else if (type == $cst) {
					curNsp->def(token, *(new Object()));
					tk = *((*curNsp)[token]);
					tokens.push_back(tk);
					token = "";
					type = $name;
				}
				else {
					cout << "Undefined token: " << token << endl;
					token = "";
				}
			}
			continue;
		}
		// �ж������Ƿ���� 
		else if (token.length()) {
			// ��ǰΪ��ʶ��
			if (type == $name || type == $def || type == $cst) {
				if ((*it >= '0' && *it <= '9') || (*it >= 'a' && *it <= 'z') || (*it >= 'A' && *it <= 'Z')
				|| *it == '_' || *it == '$') {
					token += *it;
				}
				else {
					Object tk = *((*curNsp)[token]);
					tokens.push_back(tk);
					token = "";
					type = $name;
				}
			}
			// ��ǰ����Ϊ���� 
			else if (type == $d) {
				if (*it >= '0' && *it <= '9') {
					token += *it;
				}
				else if (*it == '.') {
					// ת��Ϊ������ 
					token += *it;
					type = $f;
				}
				else {
					// �ض����ƣ���ʼ������ 
					Object tk;
					tk = *(new Integer(parseInt(token)));
					token = "";
					tokens.push_back(tk);
				}
			}
			else if (type == $f) {
				if (*it >= '0' && *it <= '9') {
					token += *it;
				}
				else {
					// �ض����ƣ���ʼ������ 
					Object tk;
					tk = *(new Double(parseFloat(token)));
					token = "";
					tokens.push_back(tk);
				}
			}
			else if (type == $oprt) {
				Object tk;
				tk = *(new Operator(token));
				token = "";
				tokens.push_back(tk);
			}
			else if (type == $block) {
				Object tk;
				tk = *(new BlockSign(token[0]));
				token = "";
				tokens.push_back(tk);
			}
		}
		// �µ����� 
		if (token.length() == 0) {
			if (type == $def || type == $cst) {
				token = *it;
			}
			// ������ 
			else if (*it >= '0' && *it <= '9') {
				token = *it;
				type = $d;
			}
			// �������ֵĲ�����
			else if (*it == '?' || *it == '~' || *it == ':') {
				token = *it;
				type = $oprt;
			}
			// �Ƚ������ 
			else if (*it == '<' || *it == '>') {
				token = *it;
				type = $oprt;
				if (it + 1 < formula.end()) {
					if (*(it + 1) == '=' || *(it + 1) == *it) {
						token += *(it++);
					}
				}
			}
			else if (*it == '!') {
				token = *it;
				type = $oprt;
				if (it + 1 < formula.end()) {
					if (*(it + 1) == '=') {
						token += *(it++);
					}
				}
			}
			// +, -, ++, --, +=, -=
			else if (*it == '+' || *it == '-') {
				token = *it;
				type = $oprt;
				if (it + 1 < formula.end()) {
					if (*(it + 1) == *it || *(it + 1) == '=') {
						token += *(it++);
					}
				}
			}
			// *, *=, %, %=
			else if (*it == '*' || *it == '%') {
				token = *it;
				type = $oprt;
				if (it + 1 < formula.end()) {
					if (*(it + 1) == '=') {
						token += *(it++);
					}
				}
			}
			// �����������ֵĲ�����
			else if (*it == '&' || *it == '=' || *it == '|') {
				token = *it;
				type = $oprt;
				if (it + 1 < formula.end()) {
					if (*(it + 1) == *it) {
						token += *(it++);
					}
				}
			}
			// ���ܳ�Ϊע�͵Ĳ�����
			else if (*it == '/') {
				token = *it;
				type = $oprt;
				if (it + 1 < formula.end()) {
					if (*(it + 1) == '/') {
						return *(new Number());	// ����Ϊע�� 
					}
					// /=
					else if (*(it + 1) == '=') {
						token += *(it++);
					}
				}
			}
			// ���� 
			else if (*it == '{' || *it == '}') {
				token = *it;
				type = $block;
			}
			// ���� 
			else if (*it == '(' || *it =='[' || *it == ')' || *it == ']') {
				token = *it;
				type = $oprt;
			}
			// ��Ա�����
			else if (*it == '.') {
				token = *it;
				type = $oprt;
			}
			// �±������
			else if (*it == '[' || *it == ']') {
				token = *it;
				type = $oprt;
			} 
			
			else {
				token += *it;
				if (type != $def || type != $cst) {
					type = $name;
				}
			}
		}
	}
	// ����ʣ�µ��ַ� 
	if (token.length()) {
		Object tk;
		if (type == $d) {
			tk = *(new Integer(parseInt(token)));
			token = "";
			tokens.push_back(tk);
		}
		else if (type == $f) {
			tk = *(new Double(parseFloat(token)));
			token = "";
			tokens.push_back(tk);
		}
		else if (type == $oprt) {
			tk = *(new Operator(token));
			token = "";
			tokens.push_back(tk);
		}
		else if (type == $block) {
			tk = *(new BlockSign(token[0]));
			token = "";
			tokens.push_back(tk);
		}
		else if (type == $name) {
			if (token.compare("let") == 0 || token.compare("const") == 0) {
				cerr << "Defination not illigal here." << endl;
				exit(-1);
			}
			else {
				tk = *((*curNsp)[token]);
				if (tk.typeIs("Object") && tk.toString().compare("undefined") == 0) {
					cerr << token << " is undefined." << endl;
					exit(0);
				}
				tokens.push_back(tk);
				token = "";
			}
		}
		else if (type == $def) {
			curNsp->def(token, *(new Object()));
			tk = *((*curNsp)[token]);
			tokens.push_back(tk);
			token = "";
		}
		else if (type == $cst) {
			curNsp->def(token, *(new Object()));
			tk = *((*curNsp)[token]);
			tokens.push_back(tk);
			token = "";
		}
		else {
			cout << "Undefined token: " << token << endl;
			token = "";
		}
	}

    return compute(tokens);
} catch (const char* msg) {
	std::cerr << msg << std::endl;

    return *(new Object());
}

/* ִ����� */ 
Object compute(LinkList<Object> tokens) {
	if (tokens.length() == 0) {
		return *(new Integer(0));
	}
	
//	cout << "\n(";
//	for (int i = 0; i < tokens.length(); i++) {
//		cout << "<" << tokens[i].toString() << "> ";
//	}
//	cout << endl;
//	cout << ") => (";
	
	LinkStack<Object> tokenSet = *(new LinkStack<Object>(tokens.length()));
	for (int i = 0; i < tokens.length(); i++) {
		Object obj = tokens[i];
		if (obj.typeIs("BlockSign")) {
		}
		else if (obj.typeIs("Operator")) {
			string str = obj.toString();
			if (str.length() == 1) {
				char letter = str[0];
				int value = obj.getValue();
				bool isFirst = true;
				// �������� 
				if (tokens.length() == 0 || letter == '(' || letter == '[' || letter == '?' || letter == ':') {
					tokenSet.push(obj);
					continue;
				}
				// �ر����� 
				else if (letter == ')') {
					LinkList<Object> inner = *(new LinkList<Object>());
					while (tokenSet.length()) {
						Object o = tokenSet.pop();
						if (o.typeIs("Operator") && o.toString().compare("(") == 0) {
							break;
						}
						inner.insert(o, 0);
					}
					// �����ڲ� 
					if (inner.length()) {
						tokenSet.push(compute(inner));
					}
					else {
						cerr << "Cannot compile ??? at Line x." << endl;
						exit(-1);
					}
					continue;
				}
				else if (letter == ']') {
					LinkList<Object> inner = *(new LinkList<Object>());
					while (tokenSet.length()) {
						Object o = tokenSet.pop();
						if (o.typeIs("Operator") && o.toString().compare("[") == 0) {
							break;
						}
						inner.insert(o, 0);
					}
					// �����ڲ� 
					if (inner.length()) {
						Object index = compute(inner);
						Object set = tokenSet.pop();
						tokenSet.push(set[index.toString()]);
					}
					else {
						cerr << "Cannot compile ??? at Line x." << endl;
						exit(-1);
					}
					continue;
				}
				else {
					for (int i = tokenSet.length() - 1; i >= 0; i--) {
						if (tokenSet[i].typeIs("Operator")) {
							if (tokenSet[i].toString().compare("[") == 0
							|| tokenSet[i].toString().compare("(") == 0) {
								break;
							}
							int v = tokenSet[i].getValue();
							// ������Ҫ�ȴ�������� 
							if (v <= value) {
								// ��ֵ 
								if (tokenSet[i].toString().compare("=") == 0) {
									if (i != tokenSet.length() - 1 && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object right = tokenSet.pop();
										if (right.isInstanceOf("Number")) {
											tokenSet.pop();
											if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
											&& !tokenSet.getTop().typeIs("Callable")) {
												Object* left = (*curNsp)[tokenSet.pop().toString()];
												if (!left->defined || left->isInstanceOf("Number")) {
													if (right.isInstanceOf("Integer")) {
														left->setValue(right.getValue());
													}
													else {
														left->setValuef(right.getValuef());
													}
													tokenSet.push(*left);
												}
												else {
													cerr << "Cannot compile operator '=' with type <"
														<< left->type() << ">." << endl;
													exit(-1);
												}
											}
											else {
												cerr << "Cannot compile operator '=' with no left values." << endl;
												exit(-1);
											}
										}
										else {
											cerr << "Cannot compile operator '=' with type <"
												<< right.type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '=' with no right values." << endl;
										exit(-1);
									}
								}
								// ��Ա���� 
								else if (tokenSet[i].toString().compare(".") == 0) {
									LinkList<Object> inner = *(new LinkList<Object>());
									while (tokenSet.length()) {
										Object o = tokenSet.pop();
										if (o.typeIs("Operator") && o.toString().compare(".") == 0) {
											break;
										}
										inner.insert(o, 0);
									}
									if (inner.length()) {
										Object index = compute(inner);
										Object set = tokenSet.pop();
										tokenSet.push(set[index.toString()]);
									}
									else {
										cerr << "Cannot compile ??? at Line x." << endl;
										exit(-1);
									}
									tokenSet.push(obj);
								}
								// �������� 
								else if (tokenSet[i].toString().compare("++") == 0) {
									// ������
									if (i != tokenSet.length() - 1 && !tokenSet[i + 1].typeIs("Operator")
									&& !tokenSet[i + 1].typeIs("Callable")) {
										// ������
										if (tokenSet[i + 1].isInstanceOf("Integer")) {
											const int nextValue = tokenSet[i + 1].getValue() + 1;
											tokenSet[i + 1].setValue(nextValue);
										}
										else {
											cerr << "Cannot compile operator '++' with type <"
												<< tokenSet[i + 1].type() << ">." << endl;
											exit(-1);
										}
									}
									// ������
									else if (i != 0 && !tokenSet[i - 1].typeIs("Operator")
									&& !tokenSet[i - 1].typeIs("Callable")) {
										if (tokenSet[i - 1].isInstanceOf("Integer")) {
											// ������
											tokens.push_back(tokenSet[i - 1]);
											tokens.push_back(*(new Operator("+=")));
											tokens.push_back(*(new Integer(1)));
										}
										else {
											cerr << "Cannot compile operator '++' with type <"
												<< tokens[i - 1].type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '++' with no objects." << endl;
										exit(-1);
									}
								}
								// �Լ����� 
								else if (tokenSet[i].toString().compare("--") == 0) {
									// ���Լ�
									if (i != tokenSet.length() - 1 && !tokenSet[i + 1].typeIs("Operator")
									&& !tokenSet[i + 1].typeIs("Callable")) {
										// ���Լ�
										if (tokenSet[i + 1].isInstanceOf("Integer")) {
											const int nextValue = tokenSet[i + 1].getValue() - 1;
											tokenSet[i + 1].setValue(nextValue);
										}
										else {
											cerr << "Cannot compile operator '++' with type <"
												<< tokenSet[i + 1].type() << ">." << endl;
											exit(-1);
										}
									}
									// ���Լ�
									else if (i != 0 && !tokenSet[i - 1].typeIs("Operator")
									&& !tokenSet[i - 1].typeIs("Callable")) {
										if (tokenSet[i - 1].isInstanceOf("Integer")) {
											// ���Լ�
											tokens.push_back(tokenSet[i - 1]);
											tokens.push_back(*(new Operator("-=")));
											tokens.push_back(*(new Integer(1)));
										}
										else {
											cerr << "Cannot compile operator '++' with type <"
												<< tokens[i - 1].type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '--' with no objects." << endl;
										exit(-1);
									}
								}
								// ��̾��
								else if (tokenSet[i].toString().compare("!") == 0) {
									// ��ȡ�� 
									if (i != tokenSet.length() - 1 && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object right = tokenSet.pop();
										if (right.isInstanceOf("Number")) {
											if (right.getValue() == 0) {
												tokenSet.push(*(new Integer(1)));
											}
											else {
												tokenSet.push(*(new Integer(0)));
											}
										}
										else {
											cerr << "Cannot compile operator '!' with type <"
												<< right.type() << ">." << endl;
											exit(-1);
										}
									}
									// �ң����� 
									else if (i != 0 && !tokenSet[i - 1].typeIs("Operator")
									&& !tokenSet[i - 1].typeIs("Callable")) {
										// TODO
									}
									else {
										cerr << "Cannot compile operator '!' with no objects." << endl;
										exit(-1);
									}
								}
								// ��λȡ��
								else if (tokenSet[i].toString().compare("~") == 0) {
									// ��ȡ�� 
									if (i != tokenSet.length() - 1 && !tokenSet[i + 1].typeIs("Operator")
									&& !tokenSet[i + 1].typeIs("Callable")) {
										if (tokenSet[i + 1].isInstanceOf("Number")) {
											// TODO
										}
										else {
											cerr << "Cannot compile operator '!' with type <"
												<< tokenSet[i + 1].type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '~' with no objects." << endl;
										exit(-1);
									}
								}
								// �˷�����
								else if (tokenSet[i].toString().compare("*") == 0) {
									int leftValue;
									int rightValue;
									double leftValuef;
									double rightValuef;
									bool isInteger = true;
									if (i != tokenSet.length() - 1 && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object right = tokenSet.pop();
										if (right.isInstanceOf("Number")) {
											if (right.isInstanceOf("Integer")) {
												rightValue = right.getValue();
												rightValuef = right.getValuef();
											}
											else {
												rightValuef = right.getValuef();
												isInteger = false;
											}
										}
										else {
											cerr << "Cannot compile operator '*' with type <"
												<< right.type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '*' with no right values." << endl;
										exit(-1);
									}
									tokenSet.pop();
									if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object left = tokenSet.pop();
										if (left.isInstanceOf("Number")) {
											if (left.isInstanceOf("Integer")) {
												leftValue = left.getValue();
												leftValuef = left.getValuef();
											}
											else {
												leftValuef = left.getValuef();
												isInteger = false;
											}
										}
										else {
											cerr << "Cannot compile operator '*' with type <"
												<< left.type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '*' with no left values." << endl;
										exit(-1);
									}
									if (isInteger || leftValuef * rightValuef == 0) {
										tokenSet.push(*(new Integer(leftValue * rightValue)));
									}
									else {
										tokenSet.push(*(new Double(leftValuef * rightValuef)));
									}
								}
								// ��������
								else if (tokenSet[i].toString().compare("/") == 0) {
									int leftValue;
									int rightValue;
									double leftValuef;
									double rightValuef;
									bool isInteger = true;
									if (i != tokenSet.length() - 1 && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object right = tokenSet.pop();
										if (right.isInstanceOf("Number")) {
											if (right.isInstanceOf("Integer")) {
												rightValue = right.getValue();
												rightValuef = right.getValuef();
											}
											else {
												rightValuef = right.getValuef();
												isInteger = false;
											}
										}
										else {
											cerr << "Cannot compile operator '/' with type <"
												<< right.type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '/' with no right values." << endl;
										exit(-1);
									}
									tokenSet.pop();
									if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object left = tokenSet.pop();
										if (left.isInstanceOf("Number")) {
											if (left.isInstanceOf("Integer")) {
												leftValue = left.getValue();
												leftValuef = left.getValuef();
											}
											else {
												leftValuef = left.getValuef();
												isInteger = false;
											}
										}
										else {
											cerr << "Cannot compile operator '/' with type <"
												<< left.type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '/' with no left values." << endl;
										exit(-1);
									}
									if (isInteger && leftValue % rightValue == 0) {
										tokenSet.push(*(new Integer(leftValue / rightValue)));
									}
									else {
										tokenSet.push(*(new Double(leftValuef / rightValuef)));
									}
								}
								// ģ������
								else if (tokenSet[i].toString().compare("%") == 0) {
									int leftValue;
									int rightValue;
									if (i != tokenSet.length() - 1 && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object right = tokenSet.pop();
										if (right.isInstanceOf("Integer")) {
											rightValue = right.getValue();
										}
										else {
											cerr << "Cannot compile operator '%' with type <"
												<< right.type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '%' with no right values." << endl;
										exit(-1);
									}
									tokenSet.pop();
									if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object left = tokenSet.pop();
										if (left.isInstanceOf("Integer")) {
											leftValue = left.getValue();
										}
										else {
											cerr << "Cannot compile operator '%' with type <"
												<< left.type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '%' with no left values." << endl;
										exit(-1);
									}
									tokenSet.push(*(new Integer(leftValue % rightValue)));
								}
								// �ӷ�����
								else if (tokenSet[i].toString().compare("+") == 0) {
									int leftValue;
									int rightValue;
									double leftValuef;
									double rightValuef;
									bool isInteger = true;
									if (i != tokenSet.length() - 1 && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object right = tokenSet.pop();
										if (right.isInstanceOf("Number")) {
											if (right.isInstanceOf("Integer")) {
												rightValue = right.getValue();
												rightValuef = right.getValuef();
											}
											else {
												rightValuef = right.getValuef();
												isInteger = false;
											}
										}
										else {
											cerr << "Cannot compile operator '+' with type <"
												<< right.type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '+' with no right values." << endl;
										exit(-1);
									}
									tokenSet.pop();
									if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object left = tokenSet.pop();
										if (left.isInstanceOf("Number")) {
											if (left.isInstanceOf("Integer")) {
												leftValue = left.getValue();
												leftValuef = left.getValuef();
											}
											else {
												leftValuef = left.getValuef();
												isInteger = false;
											}
										}
										else {
											cerr << "Cannot compile operator '+' with type <"
												<< left.type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '+' with no left values." << endl;
										exit(-1);
									}
									if (isInteger) {
										tokenSet.push(*(new Integer(leftValue + rightValue)));
									}
									else {
										tokenSet.push(*(new Double(leftValuef + rightValuef)));
									}
								}
								// ��������
								else if (tokenSet[i].toString().compare("-") == 0) {
									int leftValue;
									int rightValue;
									double leftValuef;
									double rightValuef;
									bool isInteger = true;
									if (i != tokenSet.length() - 1 && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object right = tokenSet.pop();
										if (right.isInstanceOf("Number")) {
											if (right.isInstanceOf("Integer")) {
												rightValue = right.getValue();
												rightValuef = right.getValuef();
											}
											else {
												rightValuef = right.getValuef();
												isInteger = false;
											}
										}
										else {
											cerr << "Cannot compile operator '-' with type <"
												<< right.type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '-' with no right values." << endl;
										exit(-1);
									}
									tokenSet.pop();
									if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
									&& !tokenSet.getTop().typeIs("Callable")) {
										Object left = tokenSet.pop();
										if (left.isInstanceOf("Number")) {
											if (left.isInstanceOf("Integer")) {
												leftValue = left.getValue();
												leftValuef = left.getValuef();
											}
											else {
												leftValuef = left.getValuef();
												isInteger = false;
											}
										}
										else {
											cerr << "Cannot compile operator '-' with type <"
												<< left.type() << ">." << endl;
											exit(-1);
										}
									}
									else {
										cerr << "Cannot compile operator '-' with no left values." << endl;
										exit(-1);
									}
									if (isInteger) {
										tokenSet.push(*(new Integer(leftValue - rightValue)));
									}
									else {
										tokenSet.push(*(new Double(leftValuef - rightValuef)));
									}
								}
								break;
							}
						}
					}
					// ���� 
					tokenSet.push(obj);
				}
			} 
		}
		// ���ɵ��ö���ʵ�� 
		else {
			tokenSet.push(obj);
		}
	}
	// ��β
	LinkStack<Object> side = *(new LinkStack<Object>);
	while (tokenSet.length() > 0) {
		Object passed = tokenSet.pop();
		if (passed.typeIs("Operator")) {
			// ��ֵ
			if (passed.toString().compare("=") == 0) {
				if (side.length() && !side.getTop().typeIs("Operator")
				&& !side.getTop().typeIs("Callable")) {
					Object right = side.pop();
					if (right.isInstanceOf("Number")) {
						if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
						&& !tokenSet.getTop().typeIs("Callable")) {
							Object* left = (*curNsp)[tokenSet.pop().toString()];
							if (!left->defined || left->isInstanceOf("Number")) {
								if (right.isInstanceOf("Integer")) {
									left->setValue(right.getValue());
								}
								else {
									left->setValuef(right.getValuef());
								}
								tokenSet.push(*left);
							}
							else {
								cerr << "Cannot compile operator '=' with type <"
									<< left->type() << ">." << endl;
								exit(-1);
							}
						}
						else {
							cerr << "Cannot compile operator '=' with no left values." << endl;
							exit(-1);
						}
					}
					else {
						cerr << "Cannot compile operator '=' with type <"
							<< right.type() << ">." << endl;
						exit(-1);
					}
				}
				else {
					cerr << "Cannot compile operator '=' with no right values." << endl;
					exit(-1);
				}
			} 
			// ��������
			else if (passed.toString().compare("/") == 0) {
				int leftValue;
				int rightValue;
				double leftValuef;
				double rightValuef;
				bool isInteger = true;
				if (side.length() && !side.getTop().typeIs("Operator")
				&& !side.getTop().typeIs("Callable")) {
					Object right = side.pop();
					if (right.isInstanceOf("Number")) {
						if (right.isInstanceOf("Integer")) {
							rightValue = right.getValue();
							rightValuef = right.getValuef();
						}
						else {
							rightValuef = right.getValuef();
							isInteger = false;
						}
					}
					else {
						cerr << "Cannot compile operator '/' with type <"
							<< right.type() << ">." << endl;
						exit(-1);
					}
				}
				else {
					cerr << "Cannot compile operator '/' with no right values." << endl;
					exit(-1);
				}
				if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
				&& !tokenSet.getTop().typeIs("Callable")) {
					Object left = tokenSet.pop();
					if (left.isInstanceOf("Number")) {
						if (left.isInstanceOf("Integer")) {
							leftValue = left.getValue();
							leftValuef = left.getValuef();
						}
						else {
							leftValuef = left.getValuef();
							isInteger = false;
						}
					}
					else {
						cerr << "Cannot compile operator '/' with type <"
							<< left.type() << ">." << endl;
						exit(-1);
					}
				}
				else {
					cerr << "Cannot compile operator '/' with no left values." << endl;
					exit(-1);
				}
				if (isInteger && leftValue % rightValue == 0) {
					tokenSet.push(*(new Integer(leftValue / rightValue)));
				}
				else {
					tokenSet.push(*(new Double(leftValuef / rightValuef)));
				}
			}
			// �˷�����
			else if (passed.toString().compare("*") == 0) {
				int leftValue;
				int rightValue;
				double leftValuef;
				double rightValuef;
				bool isInteger = true;
				if (side.length() && !side.getTop().typeIs("Operator")
				&& !side.getTop().typeIs("Callable")) {
					Object right = side.pop();
					if (right.isInstanceOf("Number")) {
						if (right.isInstanceOf("Integer")) {
							rightValue = right.getValue();
							rightValuef = right.getValuef();
						}
						else {
							rightValuef = right.getValuef();
							isInteger = false;
						}
					}
					else {
						cerr << "Cannot compile operator '*' with type <"
							<< right.type() << ">." << endl;
						exit(-1);
					}
				}
				else {
					cerr << "Cannot compile operator '*' with no right values." << endl;
					exit(-1);
				}
				if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
				&& !tokenSet.getTop().typeIs("Callable")) {
					Object left = tokenSet.pop();
					if (left.isInstanceOf("Number")) {
						if (left.isInstanceOf("Integer")) {
							leftValue = left.getValue();
							leftValuef = left.getValuef();
						}
						else {
							leftValuef = left.getValuef();
							isInteger = false;
						}
					}
					else {
						cerr << "Cannot compile operator '*' with type <"
							<< left.type() << ">." << endl;
						exit(-1);
					}
				}
				else {
					cerr << "Cannot compile operator '*' with no left values." << endl;
					exit(-1);
				}
				if (isInteger || leftValuef * rightValuef == 0) {
					tokenSet.push(*(new Integer(leftValue * rightValue)));
				}
				else {
					tokenSet.push(*(new Double(leftValuef * rightValuef)));
				}
			}
			// ģ������
			else if (passed.toString().compare("%") == 0) {
				int leftValue;
				int rightValue;
				if (side.length() && !side.getTop().typeIs("Operator")
				&& !side.getTop().typeIs("Callable")) {
					Object right = side.pop();
					if (right.isInstanceOf("Integer")) {
						rightValue = right.getValue();
					}
					else {
						cerr << "Cannot compile operator '%' with type <"
							<< right.type() << ">." << endl;
						exit(-1);
					}
				}
				else {
					cerr << "Cannot compile operator '%' with no right values." << endl;
					exit(-1);
				}
				if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
				&& !tokenSet.getTop().typeIs("Callable")) {
					Object left = tokenSet.pop();
					if (left.isInstanceOf("Integer")) {
						leftValue = left.getValue();
					}
					else {
						cerr << "Cannot compile operator '%' with type <"
							<< left.type() << ">." << endl;
						exit(-1);
					}
				}
				else {
					cerr << "Cannot compile operator '%' with no left values." << endl;
					exit(-1);
				}
				tokenSet.push(*(new Integer(leftValue % rightValue)));
			}
			// �ӷ�����
			else if (passed.toString().compare("+") == 0) {
				int leftValue;
				int rightValue;
				double leftValuef;
				double rightValuef;
				bool isInteger = true;
				if (side.length() && !side.getTop().typeIs("Operator")
				&& !side.getTop().typeIs("Callable")) {
					Object right = side.pop();
					if (right.isInstanceOf("Number")) {
						if (right.isInstanceOf("Integer")) {
							rightValue = right.getValue();
							rightValuef = right.getValuef();
						}
						else {
							rightValuef = right.getValuef();
							isInteger = false;
						}
					}
					else {
						cerr << "Cannot compile operator '+' with type <"
							<< right.type() << ">." << endl;
						exit(-1);
					}
				}
				else {
					cerr << "Cannot compile operator '+' with no right values." << endl;
					exit(-1);
				}
				if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
				&& !tokenSet.getTop().typeIs("Callable")) {
					Object left = tokenSet.pop();
					if (left.isInstanceOf("Number")) {
						if (left.isInstanceOf("Integer")) {
							leftValue = left.getValue();
							leftValuef = left.getValuef();
						}
						else {
							leftValuef = left.getValuef();
							isInteger = false;
						}
					}
					else {
						cerr << "Cannot compile operator '+' with type <"
							<< left.type() << ">." << endl;
						exit(-1);
					}
				}
				else {
					cerr << "Cannot compile operator '+' with no left values." << endl;
					exit(-1);
				}
				if (isInteger) {
					tokenSet.push(*(new Integer(leftValue + rightValue)));
				}
				else {
					tokenSet.push(*(new Double(leftValuef + rightValuef)));
				}
			}
			// ��������
			else if (passed.toString().compare("-") == 0) {
				int leftValue;
				int rightValue;
				double leftValuef;
				double rightValuef;
				bool isInteger = true;
				if (side.length() && !side.getTop().typeIs("Operator")
				&& !side.getTop().typeIs("Callable")) {
					Object right = side.pop();
					if (right.isInstanceOf("Number")) {
						if (right.isInstanceOf("Integer")) {
							rightValue = right.getValue();
							rightValuef = right.getValuef();
						}
						else {
							rightValuef = right.getValuef();
							isInteger = false;
						}
					}
					else {
						cerr << "Cannot compile operator '-' with type <"
							<< right.type() << ">." << endl;
						exit(-1);
					}
				}
				else {
					cerr << "Cannot compile operator '-' with no right values." << endl;
					exit(-1);
				}
				if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
				&& !tokenSet.getTop().typeIs("Callable")) {
					Object left = tokenSet.pop();
					if (left.isInstanceOf("Number")) {
						if (left.isInstanceOf("Integer")) {
							leftValue = left.getValue();
							leftValuef = left.getValuef();
						}
						else {
							leftValuef = left.getValuef();
							isInteger = false;
						}
					}
					else {
						cerr << "Cannot compile operator '-' with type <"
							<< left.type() << ">." << endl;
						exit(-1);
					}
				}
				else {
					cerr << "Cannot compile operator '-' with no left values." << endl;
					exit(-1);
				}
				if (isInteger) {
					tokenSet.push(*(new Integer(leftValue - rightValue)));
				}
				else {
					tokenSet.push(*(new Double(leftValuef - rightValuef)));
				}
			}
			// ��̾�� 
			else if (passed.toString().compare("!") == 0) {
				// ��ȡ�� 
				if (side.length() && !side.getTop().typeIs("Operator")
				&& !side.getTop().typeIs("Callable")) {
					Object right = side.pop();
					if (right.isInstanceOf("Number")) {
						if (right.getValue() == 0) {
							tokenSet.push(*(new Integer(1)));
						}
						else {
							tokenSet.push(*(new Integer(0)));
						}
					}
					else {
						cerr << "Cannot compile operator '!' with type <"
							<< right.type() << ">." << endl;
						exit(-1);
					}
				}
				else if (tokenSet.length() && !tokenSet.getTop().typeIs("Operator")
				&& !tokenSet.getTop().typeIs("Callable")) {
					// TODO
				}
				else {
					cerr << "Cannot compile operator '!' with no values." << endl;
					exit(-1);
				}
			}
		}
		else {
			side.push(passed);
		}
	}
	
	while (side.length()) {
		tokenSet.push(side.pop());
	}
	
	int start = 0;
	while (start < tokenSet.length()) {
		if (!tokenSet[start].isInstanceOf("Operator") && !tokenSet[start].isInstanceOf("Callable")) {
			break;
		}
		else {
			start++;
		}
	}
	
	Object result = tokenSet.length() ? tokenSet[start] : *(new Integer(0));
	tokens.clear();
	
//	cout << "<" << to_string(result) << ">)" << endl;
	
	return result;
}

