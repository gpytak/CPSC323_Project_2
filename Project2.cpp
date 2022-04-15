// ============================================================================
// Programmer: Vincent Hsu, Gregory Pytak, Ethan Ton
// Date: 04/17/2022
// Class: CPSC 323
// Project: Project 2
// ============================================================================
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <fstream>
#include <stack>

using namespace std;

bool isComment = false;
string ruleList[30];

// These are the inputs for the FSM.
enum TransitionStates {
	// REJECT is the starting state and final state.
	REJECT = 0,
	INTEGER = 1,
	REAL = 2,
	OPERATOR = 3,
	STRING = 4,
	SPACE = 5,
	SEPARATOR = 6,
	COMMENT = 7,
	UNKNOWN = 8
};

// ============================================================================
//  STRUCT Token
// ============================================================================
struct Tokens
{
	string lexemeValue;
	int lexeme;
	string tokenName;
};

// ============================================================================
//  Function Prototypes
// ============================================================================
vector<Tokens> lexer(string fileInput);
int getCol(char character);
string tokenName(string token, int lexeme);
void productionParser(vector<Tokens> &tokens);
//bool production

// ============================================================================
//  Integer Table
// ============================================================================
/* INTEGER, REAL, OPERATOR, STRING, SPACE, SEPARATOR, COMMENT, UNKNOWN  */
int table[9][9] =
{ {REJECT,   INTEGER,       REAL,          OPERATOR,      STRING,       SPACE,         SEPARATOR,    COMMENT,  UNKNOWN},
/* STATE 1 */ {INTEGER,   INTEGER    ,   REAL       ,   REJECT     ,   STRING     ,  REJECT     ,   REJECT     ,  COMMENT,  REJECT  },
/* STATE 2 */ {REAL,      REAL       ,   UNKNOWN    ,   REJECT     ,   REJECT     ,  REJECT     ,   REJECT     ,  COMMENT,  REJECT  },
/* STATE 3 */ {OPERATOR,  REJECT     ,   REJECT     ,   REJECT     ,   REJECT     ,  REJECT     ,   REJECT     ,  COMMENT,  REJECT  },
/* STATE 4 */ {STRING,    STRING     ,   REJECT     ,   REJECT     ,   STRING     ,  REJECT     ,   REJECT     ,  COMMENT,  REJECT  },
/* STATE 5 */ {SPACE,     REJECT     ,   REJECT     ,   REJECT     ,   REJECT     ,  REJECT     ,   REJECT     ,  COMMENT,  REJECT  },
/* STATE 6 */ {SEPARATOR, REJECT     ,   REJECT     ,   REJECT     ,   REJECT     ,  REJECT     ,   REJECT     ,  COMMENT,  REJECT  },
/* STATE 7 */ {COMMENT,  COMMENT     ,   COMMENT    ,   COMMENT    ,   COMMENT    ,  COMMENT    ,   COMMENT    ,  REJECT ,  COMMENT  },
/* STATE 8 */ {UNKNOWN,   UNKNOWN    ,   UNKNOWN    ,   UNKNOWN    ,   UNKNOWN    ,  UNKNOWN    ,   UNKNOWN    ,  UNKNOWN,  UNKNOWN  } };

// ============================================================================
//  MAIN
// ============================================================================
int main()
{
	ifstream inFile;
	ofstream oFile;
	int input;
	string fileInput = "";
	vector<Tokens> tokens;


	cout << "Please choose which input file to analyze:" << endl;
	cout << "1: Input File 1" << endl;
	cout << "2: Input File 2" << endl;
	cout << "3: Input File 3" << endl;

	// Receive input from user to select which file to open
	// Loops if input received is not acceptable
	while (true)
	{
		cin >> input;
		if (input == 1 || input == 2 || input == 3)
		{
			break;
		}
		cout << "Please input 1, 2, or 3." << endl;
	}

	// Uses user input to select which file to open
	if (input == 1)
	{
		inFile.open("Input1.txt");
		oFile.open("output1.txt");
	}
	else if (input == 2)
	{
		inFile.open("Input2.txt");
		oFile.open("output2.txt");
	}
	else
	{
		inFile.open("Input3.txt");
		oFile.open("output3.txt");
	}
	// Check if both out file and in file are properly opened
	if (!inFile.is_open() || !oFile.is_open())
	{
		cout << "Failed to open file." << endl;
		return 0;
	}

	// Output to file
	while (getline(inFile, fileInput))
	{
		// Lexical Analyzer
		tokens = lexer(fileInput);

		// Token Parser
		productionParser(tokens);
		ruleList[0] = "\n  <Statement> -> <Assign>\n  <Assign> -> <Identifier> = <Expression>";

		for (int i = 0; i < tokens.size(); i++)
		{
			// Print Lexeme and Token
			oFile << "Token: " << tokens[i].tokenName << " \t" << "Lexeme: " << tokens[i].lexemeValue;
			// Print production rules used
			oFile << ruleList[i] << endl;
		}
	}
	oFile.close();
	inFile.close();
	return 0;
}

void productionParser(vector<Tokens> &tokens)
{
	string top, rule;
	bool nextToken = false;
	stack<string> parserStack;
	// S -> E
	parserStack.push("$");
	parserStack.push("E");
	parserStack.push("=");
	parserStack.push("i");

	for (int i = 0; i < tokens.size(); i++)
	{
		rule = "";
		nextToken = false;
		top = parserStack.top();

		if (top != "$" && !parserStack.empty())
		{
			while (nextToken == false)
			{
				// Top of parserStack
				top = parserStack.top();

				if (top == "=" || top == ")")
				{
					parserStack.pop();
					nextToken = true;
					break;
				}

				//=== Production E ===
				if (top == "E") // E -> TQ
				{
					rule += "\n  <Expression> -> <Term> <Expression Prime>";
					parserStack.pop();
					parserStack.push("Q");
					parserStack.push("T");
				}

				//=== Production Q ===
				if (top == "Q" || top == "+" || top == "-") // Q -> +TQ | -TQ | e
				{
					if (tokens[i].lexemeValue == "+" && top != "+")
					{
						parserStack.pop();
						parserStack.push("Q");
						parserStack.push("T");
						parserStack.push("+");
					}
					else if (tokens[i].lexemeValue == "+" && top == "+")
					{
						rule += "\n  <Expression Prime> -> + <Term> <Expression Prime>";
						parserStack.pop();
						nextToken = true;
						break;
					}
					else if (tokens[i].lexemeValue == "-" && top != "-")
					{
						parserStack.pop();
						parserStack.push("Q");
						parserStack.push("T");
						parserStack.push("-");
					}
					else if (tokens[i].lexemeValue == "-" && top == "-")
					{
						rule += "\n  <Expression Prime> -> - <Term> <Expression Prime>";
						parserStack.pop();
						nextToken = true;
						break;
					}
					else
					{
						rule += "\n  <Expression Prime> -> <Epsilon>";
						parserStack.pop();
					}
				}

				//=== Production T ===
				if (top == "T") // T -> FR
				{
					rule += "\n  <Term> -> <Factor> <Term Prime>";
					parserStack.pop();
					parserStack.push("R");
					parserStack.push("F");
				}

				//=== Production R ===
				if (top == "R" || top == "*" || top == "/") // R -> *FR | /FR | e
				{
					if (tokens[i].lexemeValue == "*" && top != "*")
					{
						parserStack.pop();
						parserStack.push("R");
						parserStack.push("F");
						parserStack.push("*");
					}
					else if (tokens[i].lexemeValue == "*" && top == "*")
					{
						rule += "\n  <Term Prime> -> * <Factor> <Term Prime>";
						parserStack.pop();
						nextToken = true;
						break;
					}
					else if (tokens[i].lexemeValue == "/" && top != "/")
					{
						parserStack.pop();
						parserStack.push("R");
						parserStack.push("F");
						parserStack.push("/");
					}
					else if (tokens[i].lexemeValue == "/" && top == "/")
					{
						rule += "\n  <Term Prime> -> / <Factor> <Term Prime>";
						parserStack.pop();
						nextToken = true;
						break;
					}
					else
					{
						rule += "\n  <Term Prime> -> <Epsilon>";
						parserStack.pop();
					}
				}

				// === Production F ===
				if (top == "F" || top == "(" || top == "i") // F -> (E) | i
				{
					if (tokens[i].lexemeValue == "(" && top != "(")
					{
						parserStack.pop();
						parserStack.push(")");
						parserStack.push("E");
						parserStack.push("(");
					}
					else if (tokens[i].lexemeValue == "(" && top == "(")
					{
						rule += "\n  <Factor> -> ( <Expression> )";
						parserStack.pop();
						nextToken = true;
						break;
					}
					else if (tokens[i].tokenName == "IDENTIFIER" && top != "i")
					{
						parserStack.pop();
						parserStack.push("i");
					}
					else if (tokens[i].tokenName == "IDENTIFIER" && top == "i")
					{
						rule += "\n  <Factor> -> <Identifier>";
						parserStack.pop();
						nextToken = true;
						break;
					}
				}
			}
		}
		else
		{
			if (!parserStack.empty())
				cout << "Error: Stack is empty" << endl;
			if (top != "$")
				cout << "Stack reached $" << endl;
			break;
		}

		// Before next input, record production rules used
		ruleList[i] = rule;
	}
}

// ============================================================================
//  Vector lexer
// ============================================================================
vector<Tokens> lexer(string fileInput)
{
	Tokens type;
	vector<Tokens> tokens;
	string currentToken = "";
	int col = REJECT;
	int currentState = REJECT;
	int previousState = REJECT;
	char currentChar = ' ';

	for (int i = 0; i < fileInput.length();)
	{
		currentChar = fileInput[i];
		col = getCol(currentChar);
		currentState = table[currentState][col];

		if (isComment == false && col == COMMENT)
		{
			isComment = true;
		}
		else if (isComment == true && col == COMMENT)
		{
			isComment = false;
		}
		else if (isComment == true && col != COMMENT)
		{
			currentState = COMMENT;
		}

		if (currentState == REJECT)
		{
			if (previousState != SPACE && previousState != COMMENT)
			{
				type.lexemeValue = currentToken;
				type.lexeme = previousState;
				type.tokenName = tokenName(type.lexemeValue, type.lexeme);
				tokens.push_back(type);
			}
			currentToken = "";
		}
		else
		{
			currentToken += currentChar;
			i++;
		}
		previousState = currentState;
	}
	if (currentState != SPACE && currentToken != "" && previousState != COMMENT)
	{
		type.lexemeValue = currentToken;
		type.lexeme = currentState;
		type.tokenName = tokenName(type.lexemeValue, type.lexeme);
		tokens.push_back(type);
	}
	return tokens;
}

// ============================================================================
//  int getCol
//      INPUT  - char
//      OUTPUT - Column number associated with the current input character
// ============================================================================
int getCol(char character)
{
	if (character == '!')
	{
		return COMMENT;
	}
	if (isspace(character))
	{
		return SPACE;
	}
	else if (isdigit(character))
	{
		return INTEGER;
	}
	else if (character == '.')
	{
		return REAL;
	}
	else if (isalpha(character) || character == '$')
	{
		return STRING;
	}
	else if (character == '+' || character == '-' || character == '/' || character == '*' || character == '==' || character == '=' ||
		character == '>' || character == '<' || character == '%')
	{
		return OPERATOR;
	}
	else if (character == '(' || character == ')' || character == '{' || character == '}' || character == ',' || character == ';' || character == ':')
	{
		return SEPARATOR;
	}
	else if (ispunct(character))
	{
		return SEPARATOR;
	}
	return UNKNOWN;
}

// ============================================================================
//  tokenName
//      INPUT  - currentState
//      OUTPUT - Name of Lexeme
// ============================================================================
string tokenName(string token, int lexeme)
{
	switch (lexeme)
	{
	case INTEGER:
		return "INTEGER";
		break;
	case REAL:
		return "REAL  ";
		break;
	case OPERATOR:
		return "OPERATOR";
		break;
	case SEPARATOR:
		return "SEPARATOR";
		break;
	case STRING:
		if (token == "int" || token == "float" || token == "bool" || token == "if" || token == "else" || token == "then" ||
			token == "do" || token == "while" || token == "for" || token == "and" || token == "or" || token == "function")
		{
			return "KEYWORD";
		}
		else
		{
			return "IDENTIFIER";
		}
		break;
	case COMMENT:
		return "COMMENT";
		break;
	case UNKNOWN:
		return "UNKNOWN";
		break;
	case SPACE:
		return "SPACE";
		break;
	default:
		return "ERROR";
		break;
	}
}// =	===========================================================================
