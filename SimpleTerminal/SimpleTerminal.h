#ifndef SIMPLE_THERMINAL_h
#define SIMPLE_THERMINAL_h

#include <Arduino.h>

#ifndef SIMPLE_THERMINAL_MAX_VAR
#define SIMPLE_THERMINAL_MAX_VAR 10
#endif

//#define SIMPLE_THERMINAL_DEBUG

enum VarType {
	BOOL,
	INT,
	LONG,
	FLOAT,
	DOUBLE
};

typedef void (*FN_CALLBACK)(String, String);

typedef struct {
	String name;
	VarType type;
	void *ptr;
	FN_CALLBACK ptrFunc;
} RegVar;

typedef struct {
	String name;
	String description;
	FN_CALLBACK fn;
} RegCmd;

class SimpleTerminal {
public:

	SimpleTerminal(Stream &stream);
	bool addVar(String name, VarType type, void *ptr = 0, const void *fnPtr = 0);
	bool addCommand(String name, const void *fnPtr, String description = "");
	void run();
	void printHelp();

private:

	void anaylseLine(String &line);
	String getFirstWord(String &line);
	void commandHelp();
	void commandPrintEnv();
	void commandSet(String &line);
	void commandGet(String &line);
	void printVar(RegVar *var);

	int regVarIndex = 0;
	RegVar vars[SIMPLE_THERMINAL_MAX_VAR];
	int regCmdIndex = 0;
	RegCmd cmds[SIMPLE_THERMINAL_MAX_VAR];

	Stream* stream;

	String buffer;

	String VarTypePretty[5] = {"BOOL", "INT", "LONG", "FLOAT", "DOUBLE"};

};

#endif