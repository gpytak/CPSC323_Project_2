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

using namespace std;

bool stringAccepted = true;
bool error = false;
int errorIndex;
int tokenIndex = 2;
bool isComment = false;
string s[30];
string currentRule;
string ruleList[30];
ofstream oFile;

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
bool productionE(int& tokenIndex);
bool productionQ(int& tokenIndex);
bool productionT(int& tokenIndex);
bool productionR(int& tokenIndex);
bool productionF(int& tokenIndex);

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
		const int tokenLength = (tokens.size());
		for (int i = 0; i < tokens.size(); i++)
		{
			// Print Lexeme and Token
			oFile << "Token: " << tokens[i].tokenName << " \t" << "Lexeme: " << tokens[i].lexemeValue;
			// Print used production rules
			oFile << ruleList[i] << endl;
			if (tokenLength == i+1)
			{
				oFile << "Token: END \tLexeme: $";
				while (!ruleList[i].empty())
				{
					// Print unused production rules
					i++;
					oFile << ruleList[i];
				}
			}
			if (errorIndex == i && error == true)
			{
				break;
			}
		}
		if (stringAccepted == true)
		{
			oFile << "\nString accepted.";
		}
		else
		{
			oFile << "\nString not accepted.";
		}

	}
	oFile.close();
	inFile.close();
	return 0;
}

// =====================================================================
// Production Parser
// =====================================================================
void productionParser(vector<Tokens> &tokens)
{
	const int tokenLength = (tokens.size() - 1);
	for (int i = 0; i < tokens.size(); i++)
	{
		s[i] = tokens[i].lexemeValue;
		if (i == tokenLength)
		{
			i++;
			s[i] = "$";
		}
	}
	ruleList[0] = "\n\t<Statement> -> <Assign>\n\t<Assign> -> <Identifier> = <Expression>";
	if (productionE(tokenIndex) != true)
	{
		error = true;
		errorIndex = tokenIndex;
		stringAccepted = false;
	}
}

// =====================================================================
// Production E
// =====================================================================
bool productionE(int& tokenIndex)
{
	bool E = false;
	currentRule = ("\n\t<Expression> -> <Term> <Expression Prime>");
	ruleList[tokenIndex] += currentRule;
	if (productionT(tokenIndex))
	{
		if (productionQ(tokenIndex))
		{
			E = true;
		}
	}
	return E;
}

// =====================================================================
// Production Q
// =====================================================================
bool productionQ(int& tokenIndex)
{
	bool Q = false;
	if (s[tokenIndex] == "+")
	{
		currentRule = ("\n\t<Expression Prime> -> + <Term> <Expression Prime>");
		ruleList[tokenIndex] += currentRule;
		tokenIndex++;
		if (productionT(tokenIndex))
		{
			if (productionQ(tokenIndex))
			{
				Q = true;
				return Q;
			}
		}
	}
	if (s[tokenIndex] == "-")
	{
		currentRule = ("\n\t<Expression Prime> -> - <Term> <Expression Prime>");
		ruleList[tokenIndex] += currentRule;
		tokenIndex++;
		if (productionT(tokenIndex))
		{
			if (productionQ(tokenIndex))
			{
				Q = true;
				return Q;
			}
		}
	}
	if (s[tokenIndex] == ")" || s[tokenIndex] == "$")
	{
		currentRule = ("\n\t<Expression Prime> -> <Epsilon>");
		ruleList[tokenIndex] += currentRule;
		if (s[tokenIndex] == ")")
		{
			tokenIndex++;
		}
		Q = true;
		return Q;
	}
	return Q;
}

// =====================================================================
// Production T
// =====================================================================
bool productionT(int& tokenIndex)
{
	bool T = false;
	currentRule = ("\n\t<Term> -> <Factor> <Term Prime>");
	ruleList[tokenIndex] += currentRule;
	if (productionF(tokenIndex))
	{
		if (productionR(tokenIndex))
		{
			T = true;
		}
	}
	return T;
}

// =====================================================================
// Production R
// =====================================================================
bool productionR(int& tokenIndex)
{
	bool R = false;
	if (s[tokenIndex] == ")" || s[tokenIndex] == "+" || s[tokenIndex] == "-" || s[tokenIndex] == "$")
	{
		currentRule = ("\n\t<Term Prime> -> <Epsilon>");
		ruleList[tokenIndex] += currentRule;
		R = true;
		return R;
	}
	if (s[tokenIndex] == "*")
	{
		currentRule = ("\n\t<Term Prime> -> * <Factor> <Term Prime>");
		ruleList[tokenIndex] += currentRule;
		tokenIndex++;
		if (productionF(tokenIndex))
		{
			if (productionR(tokenIndex))
			{
				R = true;
				return R;
			}
		}
	}
	if (s[tokenIndex] == "/")
	{
		currentRule = ("\n\t<Term Prime> -> / <Factor> <Term Prime>");
		ruleList[tokenIndex] += currentRule;
		tokenIndex++;
		if (productionF(tokenIndex))
		{
			if (productionR(tokenIndex))
			{
				R = true;
				return R;
			}
		}
	}
	return R;
}

// =====================================================================
// Production F
// =====================================================================
bool productionF(int& tokenIndex)
{
	bool F = false;
	if (s[tokenIndex] == "(")
	{
		currentRule = ("\n\t<Factor> -> ( <Expression> )");
		ruleList[tokenIndex] += currentRule;
		tokenIndex++;
		if (productionE(tokenIndex))
		{
			F = true;
			return F;
		}
	}
	if (s[tokenIndex] >= "a" || s[tokenIndex] <= "z")
	{
		currentRule = ("\n\t<Factor> -> <Identifier>");
		ruleList[tokenIndex] += currentRule;
		tokenIndex++;
		F = true;
		return F;
	}
	return F;
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
