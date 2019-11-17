#include <iostream>
#include <map>
#include <stack>
#include <fstream>
#include <string>
#include <string.h>

using namespace std;


char operators[] = "*+-=/><%";
char separator[] = "'(){}[],:;";
char keywords[18][10] = { "int", "float", "bool", "if", "else", "then", "endif", "while", "whileend", "do", "doend","for", "forend", "input", "output", "and", "or", "function" };
bool hasSep = false;	// check if has separater
bool hasOpe = false;	// check if has operator
int currentIndex = 0;	// index for check isNumber() and isReal()

// FSM states for print
bool endOperator = false;
bool endSeparator = false;
bool endNumber = false;
bool endReal = false;
bool endIdentifier = false;

ifstream fin;		//read file
ofstream outputFile;
char ch;	// read file and get every character
char buffer[15];	//store characters in an array to determine isKeyword or isNumber or isReal or isIdentifier
int j = 0;		// index for buffer[]
bool isComments = false;	//check if it is comments
char detection[2] = { '\0' };	// detect '!' twice, if detect, add '!' to array according to occur times
int x = 0;	// used to counting detection array
string tokenLexemes[1000];
int token_count = 0;
int token_print_count = 0;

bool isKeyword(char buffer[]) {
	for (int i = 0; i < 18; ++i)
		if (strcmp(keywords[i], buffer) == 0)
			return true;
	return false;
}

bool isOperator(char ch) {
	for (int i = 0; i < 10; ++i) {
		if (ch == operators[i])
			return true;
	}
	return false;
}

bool isSeparator(char ch) {
	for (int i = 0; i < 10; ++i) {	//check separator
		if (ch == separator[i])
			return true;
	}
	return false;
}

bool isReal(char buffer[]) {
	string str(buffer);
	string substr(".");

	size_t found = str.find(substr);

	if (found != string::npos) {
		for (int i = 0; i < found; i++) {
			if (!isdigit(buffer[i]))
				return false;
			continue;
		}
		for (int i = found + 1; i < currentIndex; i++) {
			if (!isdigit(buffer[i]))
				return false;
			continue;
		}
		return true;
	}
	else
		return false;
}

bool isNumber(char buffer[]) {
	for (int i = 0; i <= (currentIndex - 1); i++) {
		if (!isdigit(buffer[i]))
			return false;
		continue;
	}
	return true;
}


enum Symbols {
	//Terminal
	T_L_PARENS, // (
	T_R_PARENS, // )
	T_ID,		// id
	T_PLUS,		// +
	T_MINUS,	// -
	T_MULT,		// *
	T_DIV,		// /
	T_EQUAL,	// =
	T_EPS,		// epsilon ~
	T_EOS,		// ;
	T_INVALID,	// invalid token

	//non-terminal
	NT_S,		// S
	NT_E,		// E
	NT_T,		// T
	NT_Q,		// Q
	NT_R,		// R
	NT_F,		//F

	FILE_END
};


Symbols print() {

	if (endIdentifier) {
		//cout << "IDENTIFIER      =     " << buffer << endl;
		//outputFile << "IDENTIFIER      =     " << buffer << endl;
		endIdentifier = false;
		string tmp = buffer;
		tokenLexemes[token_count] = "Token: IDENTIFIER          Lexemes: " + tmp;
		token_count++;
		return T_ID;
	}

	if (endNumber) {
		//cout << "NUMBER          =     " << buffer << endl;
		//outputFile << "NUMBER          =     " << buffer << endl;
		endNumber = false;
		string tmp = buffer;
		tokenLexemes[token_count] = "Token: NUMBER              Lexemes: " + tmp;
		token_count++;
		return T_ID;
	}

	if (endReal) {
		//cout << "REAL            =     " << buffer << endl;
		//outputFile << "REAL            =     " << buffer << endl;
		string tmp = buffer;
		tokenLexemes[token_count] = "Token: REAL                Lexemes: " + tmp;
		token_count++;
		endReal = false;
		return T_ID;
	}

	if (endSeparator) {	// single separator detected, print it
		//cout << "SEPARATOR       =     " << ch << endl;
		//outputFile << "SEPARATOR       =     " << ch << endl;
		endSeparator = false;
		hasSep = false;
		if (ch == '(' || ch == ')' || ch == ';') {
			string tmp (1,ch);
			tokenLexemes[token_count] = "Token: SEPARATOR           Lexemes: " + tmp;
			token_count++;
			if (ch == '(') { return T_L_PARENS; }
			else if (ch == ')') { return T_R_PARENS; }
			else if (ch == ';') { return T_EOS; }
		}
		

	}
	if (endOperator) { // single operator detected, print it
		//cout << "OPERATOR        =     " << ch << endl;
		//outputFile << "OPERATOR        =     " << ch << endl;
		endOperator = false;
		hasOpe = false;
		if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '=') {
			string tmp(1, ch);
			tokenLexemes[token_count] = "Token: OPERATOR            Lexemes: " + tmp;
			token_count++;
			if (ch == '+') { return T_PLUS; }
			else if (ch == '-') { return T_MINUS; }
			else if (ch == '*') { return T_MULT; }
			else if (ch == '/') { return T_DIV; }
			else if (ch == '=') { return T_EQUAL; }
		}
		
	}
	return T_INVALID;
}

Symbols lexcial_analyzer() {

	if (ch == '!') {	//check if is comment
		detection[x] = ch;
		x++;
		if (detection[1] == ch) {
			isComments = false;
			x = 0;
			detection[0] = '\0';
			detection[1] = '\0';
		}
		else
			isComments = true;

		return T_INVALID;
	}
	if (!isComments) {	// if not comment, start lexical analyzer


		if (isSeparator(ch)) {
			hasSep = true;
			endSeparator = true;
		}

		if (isOperator(ch)) {
			hasOpe = true;
			endOperator = true;
		}

		if (isalnum(ch) || ch == '$' || ch == '.') {	// check ch is a number or letter or '$' or '.'
			buffer[j++] = ch;		// if so, add to buffer
			currentIndex++;
		}
		else if ((ch == ' ' || ch == '\n' || hasSep || hasOpe || fin.eof()) && (j != 0)) {	// end of variables by detecting if j(index for buffer[]) != 0
			buffer[j] = '\0';	// set last char in buffer( might be ' ', '\n', separator, operator
			j = 0;	// reset the index for buffer[]
			if (isKeyword(buffer)) {	// check if the variable is keyword
				cout << "KEYWORD         =     " << buffer << endl;
				//outputFile << "KEYWORD         =     " << buffer << endl;
			}
			else if (isReal(buffer))	//check if the variable is real( number with '.')
				endReal = true;
			else if (isNumber(buffer)) //check if the variable is number 
				endNumber = true;
			else {	//check if the variable is identifier
				if (!isdigit(buffer[0]))	// check if identifier not start with number
					endIdentifier = true;
			}

			currentIndex = 0;	// reset index for isNumber() and isReal()
		}
		return print();
	}
}

Symbols lexer(string c) {
	if (c == "(") { return T_L_PARENS; }
	else if (c == ")") { return T_R_PARENS; }
	else if (c == "id") { return T_ID; }
	else if (c == "+") { return T_PLUS; }
	else if (c == "-") { return T_MINUS; }
	else if (c == "*") { return T_MULT; }
	else if (c == "/") { return T_DIV; }
	else if (c == "=") { return T_EQUAL; }
	else if (c == ";") { return T_EOS; }
	else   return T_INVALID;
}

void printRule(int index) {
	switch (index)
	{
	case 1: cout << "	S -> id=E" << endl; break;
	case 2: cout << "	E -> TQ" << endl; break;
	case 3: cout << "	Q -> +TQ" << endl; break;
	case 4: cout << "	Q -> -TQ" << endl; break;
	case 5: cout << "	Q -> epsilon" << endl; break;
	case 6: cout << "	T -> FR" << endl; break;
	case 7: cout << "	R -> *FR" << endl; break;
	case 8: cout << "	R -> /FR" << endl; break;
	case 9: cout << "	R -> epsilon" << endl; break;
	case 10: cout << "	F -> (E)" << endl; break;
	case 11: cout << "	F -> id" << endl; break;
	default:   cout << "No Such Production Rule!"<< endl;
	}
}

string convert(Symbols g) {
	if (g == T_L_PARENS) return "(";
	else if (g == T_R_PARENS) return ")";
	else if (g == T_ID) return "id";
	else if (g == T_PLUS) return "+";
	else if (g == T_MINUS) return "-";
	else if (g == T_MULT) return "*";
	else if (g == T_DIV) return "/";
	else if (g == T_EQUAL) return "=";
	else if (g == T_EOS) return ";";
	else   return "@";
}

int main() {
	// LL parser table, map <non-terminal, terminal> pair to action
	map <Symbols, map<Symbols, int> > table;
	stack<Symbols> ss;
	int index = 0;
	Symbols b[1000];
	for (int i = 0; i < 1000; i++) {
		b[i] = FILE_END;
	}
	Symbols * g; //input buffer
	string p;
	bool already_print = false;

	//initialize symbol stack
	ss.push(T_EOS);	//push $ (an end marker) onto stack
	ss.push(NT_S);	// push start state
	

	// initialize the symbol stream cursor
	//TODO: p = line of input ex: "a = a + b"
	string filename;
	cout << "Please enter input file name:(sample file: sample.txt) " << endl;
	cin >> filename;
	fin.open(filename);
	if (!fin.is_open()) {
		cout << "error while opening the file\n";
		exit(0);
	}
	while (!fin.eof()) {
		ch = fin.get();
		Symbols tmp = lexcial_analyzer();

		if (tmp != T_INVALID) {
			b[index] = tmp;
			index++;
		}
	}
	g = b;

	// set up parsing table
	table[NT_S][T_ID] = 1;
	table[NT_E][T_ID] = 2;
	table[NT_E][T_L_PARENS] = 2;
	table[NT_Q][T_PLUS] = 3;
	table[NT_Q][T_MINUS] = 4;
	table[NT_Q][T_R_PARENS] = 5;
	table[NT_Q][T_EOS] = 5;
	table[NT_T][T_ID] = 6;
	table[NT_T][T_L_PARENS] = 6;
	table[NT_R][T_PLUS] = 9;
	table[NT_R][T_MINUS] = 9;
	table[NT_R][T_MULT] = 7;
	table[NT_R][T_DIV] = 8;
	table[NT_R][T_R_PARENS] = 9;
	table[NT_R][T_EOS] = 9;
	table[NT_F][T_ID] = 11;
	table[NT_F][T_L_PARENS] = 10;


	int count = 0;
	while(b[count+1] != FILE_END){
		while (ss.size() > 0)
		{
			p = convert(*g);
			if (!already_print) {
				cout << tokenLexemes[token_print_count] << endl;
				already_print = true;
			}
			if (lexer(p) == ss.top()) {
				cout << endl;
				g++;
				count++;
				token_print_count++;
				already_print = false;
				ss.pop();
			}
			else {

				printRule(table[ss.top()][lexer(p)]);
				switch (table[ss.top()][lexer(p)])
				{
				case 1:		// 1) S -> id=E
					ss.pop();
					ss.push(NT_E);
					ss.push(T_EQUAL);
					ss.push(T_ID);
					break;

				case 2:		// 2) E -> TQ
					ss.pop();
					ss.push(NT_Q);
					ss.push(NT_T);
					break;

				case 3:		// 3) Q -> +TQ
					ss.pop();
					ss.push(NT_Q);
					ss.push(NT_T);
					ss.push(T_PLUS);
					break;

				case 4:		// 4) Q -> -TQ
					ss.pop();
					ss.push(NT_Q);
					ss.push(NT_T);
					ss.push(T_MINUS);
					break;

				case 5:		// 5) Q -> epsilon
					ss.pop();	
					break;

				case 6:		// 6) T -> FR
					ss.pop();
					ss.push(NT_R);
					ss.push(NT_F);
					break;

				case 7:		// 7) R -> *FR
					ss.pop();
					ss.push(NT_R);
					ss.push(NT_F);
					ss.push(T_MULT);
					break;

				case 8:		// 8) R -> /FR
					ss.pop();
					ss.push(NT_R);
					ss.push(NT_F);
					ss.push(T_DIV);
					break;

				case 9:		// 9) R -> epsilon
					ss.pop();
					break;

				case 10:		// 10) F -> (E)
					ss.pop();
					ss.push(T_R_PARENS);
					ss.push(NT_E);
					ss.push(T_L_PARENS);
					break;

				case 11:		// 11) F -> id
					ss.pop();
					ss.push(T_ID);
					break;

				default:
					cout << "Syntax Error!" << endl;
					system("pause");
					return 0;
				}
			}
		}
		ss.push(T_EOS);
		ss.push(NT_S);  //push the start symbol onto stack
	}
	cout << "finished parsing" << endl;
	system("pause");
	return 0;


}