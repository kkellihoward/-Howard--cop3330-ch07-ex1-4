
/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Kelli Howard
 */

/*
	Fixed program of calculator08buggy.cpp
    Fixed errors and added drills from book for program
    Added excercies into code

*/

// Exercise 01: allow underscores in variable names
// Exercise 02: allow user to use "="" as assignment operator
// Exercise 03: allow user to name their variables with const operator
// Exercise 04: add symbol table class

#include <C:\Users\kkell\OneDrive\Desktop\Calculatorbug\std_lib_facilities.h>

// change Token from struct to class to accomidate new code
class Token 
{
	public:
		char kind;
		double value;
		string name;
		Token(char ch) :kind(ch), value(0) { }
		Token(char ch, double val) :kind(ch), value(val) { }
		// If token is made up of a char and a string
    	Token(char ch, string n) :kind(ch), name(n) { }
};

class Token_stream 
{
	public:
		Token_stream();
		Token get();
		// a replacement for unget
		void push(Token t);
		// define our ignore function so it can be found by the program
		void ignore(char c);
	private:
		// a check to see if the token buffer is full
		bool full;
		// the buffer that houses all of the tokens
		Token buffer;
};

// Create a constructor for the tokens input stream
Token_stream::Token_stream()
    :full(false), buffer(0) { }

const char let = 'L';
// new constant key
const char constant = 'C';
const char power = 'P';
const char squarert = 'S';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';

// Used for checking if user is trying to use a keyword
const string let_word = "let";
const string const_word = "const";
const string quit_word = "exit";
const string power_word = "pow";
const string sqrt_word = "sqrt";


// read characters from cin and compose a Token
Token Token_stream::get()
{
	if (full) { full = false; return buffer; }

	char ch;
    cin.get(ch);
    while (isspace(ch)) 
	{
        if (ch == '\n') return Token(print);
        cin.get(ch);
    }

	switch (ch) 
    {
    case print:
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
    case '%':
    case '=':
    case ',':
		return Token(ch);
	case '.':
	case '0': 
	case '1': 
	case '2': 
	case '3': 
	case '4':
	case '5': 
	case '6': 
	case '7': 
	case '8': 
	case '9':
	{	
		cin.putback(ch);
		double val;
		cin >> val;
		return Token(number,val);
	}
	default:
		if (isalpha(ch)) 
		{
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch=='_')) s+=ch;
			cin.putback(ch);
			// new check to see if user is defining a variable that can be changed
            if (s == let_word) return Token(let);
			// new check to see if user is defining a constant variable
            if (s == const_word) return Token(constant);
			// new check to see if user wants to find the power of a number
			if (s == power_word) return Token(power);
			// new check to see if user wants to find the square root of a number
			if (s == sqrt_word) return Token(squarert);
			// new check to see if user wants to exit the program
			if (s == quit_word) return Token(quit);
			return Token(name,s);
		}
		error("Bad token");
	}
}

// a function to replace unget
void Token_stream::push(Token t)
{
    if (full) error("the buffer is full, cannot put back");
    buffer = t;
    full = true;
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) 
	{
		full = false;
		return;
	}

	full = false;

	char ch;
	while (cin >> ch)
		if (ch == c) return;
}

// change variable from vector to class to accomidate new code
class Variable 
{
	public:
		string name;
		double value;
		bool is_const;
		Variable(string n, double v, bool b) :name(n), value(v), is_const(b) { }
};

// All Symbol_table code is used to create functionality described in excercise 2 and 3 by implementing excercise 4
// type for variables and associated functions
class Symbol_table 
{
	// Defining all functions below
	public:
    	double get(string s);
    	void set(string s, double d);
    	bool is_declared(string var);
    	double declare(string var,double val,bool b);
	// vector of Variables
	private:
    	vector<Variable> variables;
};

// check for a variable with a name equivalent to the string parameter s, if it exists, return it
double Symbol_table::get(string s)
{
	for (int i = 0; i < variables.size(); ++i)
	{
		if (variables[i].name == s) return variables[i].value;
	}
	error("get: undefined variable ", s);
}

// converts a variable string s into a double value d
void Symbol_table::set(string s, double d)
{
	for (int i = 0; i < variables.size(); ++i)
	{
		if (variables[i].name == s) 
		{
			// cannot use a variable name that is already defined as a constant in the program
            if (variables[i].is_const) error(s," is a constant");
			variables[i].value = d;
			return;
		}
	}
	error("set: undefined variable ",s);
}

// check to see if the variable var is already defined in the program
bool Symbol_table::is_declared(string var)
{
	for (int i = 0; i<variables.size(); ++i)
	{
		if (variables[i].name == var) return true;
	}
	return false;
}

// create a new entry in user variables the the variable name equal to var and the value equal to val
double Symbol_table::declare(string var, double val, bool b)
{
    if (is_declared(var)) error(var," this variable has already been defined");
    variables.push_back(Variable(var,val,b));
    return val;
}

Token_stream ts;
// A way to store new user variables and their assignments
Symbol_table st;
// Make sure primary() can call expression()
double expression();


// functionality for allowing user to raise 
double power_of(double base, int expo)
{
	// anything raised to 0 equals 1
    if (expo == 0) {
        if (base == 0) return 0;
        return 1;
    }
    double res = base;

	// multiply the base i times
    for (int i = 2; i <= expo; ++i)
        res *= base;
    return res;
}

// updated with new logic for taking in a user defined variable
double primary()
{
	Token t = ts.get();
	switch (t.kind) 
	{
		case '(':
		{	
			double d = expression();
			t = ts.get();
			if (t.kind != ')') error("'(' expected");
		}
		case '-':
			return -primary();
		case number:
			return t.value;
		case name:
		{
			Token t2 = ts.get();
			// if token kind is an equal sign, the user is trying to define a varialbe
			if (t2.kind == '=') 
			{
				double d = expression();
				st.set(t.name,d);
				return d;
			}
			// user is not trying to define a new variable
			else 
			{
				ts.push(t2);
				return st.get(t.name);
			}
		}
		case squarert:
		{   t = ts.get();
			// check to make sure user is using the correct squar root syntax
			if (t.kind != '(') error("'(' expected");
			double d = expression();
			if (d < 0) error("Square roots of negative numbers... nope!");
			t = ts.get();
			if (t.kind != ')') error("')' expected");

			// using built in sqrt function as requested
			return sqrt(d);
		}
		case power:
		{   t = ts.get();

			// check to make sure user is using the correct squar root syntax
			if (t.kind != '(') error("'(' expected");
			double d = expression();
			t = ts.get();
			if (t.kind != ',') error("',' expected");
			t = ts.get();
			if (t.kind != number) error("second argument of 'pow' is not a number");
			int i = int(t.value);
			if (i != t.value) error("second argument of 'pow' is not an integer");
			t = ts.get();
			if (t.kind != ')') error("')' expected");

			// call power function
			return power_of(d,i);
		}
		default:
			error("primary expected");
	}
}

double term()
{
	double left = primary();
	// moved token to outside of while loop so it doesn't get overwritten
	Token t = ts.get();

	while (true) 
    {
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	
			double d = primary();
			if (d == 0) error("divide by zero");
			left /= d;
			break;
		}
		default:
			ts.push(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	// moved token to outside of while loop so it doesn't get overwritten
    Token t = ts.get();

	while (true) 
    {
		switch(t.kind) 
        {
		case '+':
			left += term();
            t = ts.get();
			break;
		case '-':
			left -= term();
            t = ts.get();
			break;
		// excercise 2: added new case for = sign so user can define a variable
        case '=':
            error("use of '=' outside of a declaration");
		default:
			// new function created to put token into the newly created table for user variables
			ts.push(t);
			return left; 
		}
	}
}

// added boolean parameter so it can be 
double declaration(bool b)
{
	Token t = ts.get();
	// check to see if user named their variable
	if (t.kind != name) error ("name expected in declaration");
	string var_name = t.name;

	// check to make sure user is using the euqal operator for the definition of a variable
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ",var_name);

	double d = expression();
    st.declare(var_name,d,b);
	return d;
}

double statement()
{
	Token t = ts.get();
	switch(t.kind) 
    {
	case let:
		return declaration(false);
	// added a new case for when user wants to define a constant
    case constant:
        return declaration(true);
	default:
		// new function created to put token into the newly created table for user variables
		ts.push(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	// Changed true to cin so we only calculate when the user has input something
	while (cin)
	{
		try 
        {
			cout << prompt;
			Token t = ts.get();
			while (t.kind == print) t=ts.get();
			if (t.kind == quit) return;

			// new function created to put token into the newly created table for user variables
			ts.push(t);
			cout << result << statement() << endl;    
		}
		catch(exception& e) 
        {
			cerr << e.what() << endl;
			clean_up_mess();
		}
	}
}

// The start of the program
int main()

try {
    st.declare("k", 1000, true);

	// Do the actual calculations
	calculate();

	// Function provided by header file to clear cin and take new user input
	keep_window_open();
	return 0;
}
catch (exception& e) 
{
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...)
{
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}