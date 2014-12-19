#ifndef SIMPLE_THERMINAL_h
#define SIMPLE_THERMINAL_h

#include <Arduino.h>

#define CMD_NAME_HELP "help"
#define CMD_NAME_PRINTENV "printenv"
#define CMD_NAME_SET_VAR "set"
#define CMD_NAME_GET_VAR "get"

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

	/**
	 * Constructor.
	 * Create the arrays for the variables and the commands.
	 * 
     * @param stream stream to listen to
     * @param maxCmds size of the commands array
     * @param maxVars size of the variables array
     */
	SimpleTerminal(Stream *stream, int maxCmds=5, int maxVars=5);
	~SimpleTerminal();
	
	bool addVar(String name, VarType type, void *ptr = 0, const void *fnPtr = 0);
	bool addCommand(String name, const void *fnPtr, String description = "");
	void setVarCmdEnabled(bool newValue);
	void setConfirmPrintVar(bool newValue);
	void run();
	void printHelp();
	void setStream(Stream *stream);

private:

	void anaylseLine(String &line);
	String getFirstWord(String &line);
	void commandHelp();
	void commandPrintEnv();
	void commandSet(String &line);
	void commandGet(String &line);
	void printVar(RegVar *var);
	
	bool varCmdEnabled = true;
	bool confirmPrintVar = true;

	int maxVars;
	int maxCmds;
	int varIndex = 0;
	int cmdIndex = 0;
	RegVar* vars;
	RegCmd* cmds;

	Stream* stream;

	String buffer;

	String VarTypePretty[5] = {"BOOL", "INT", "LONG", "FLOAT", "DOUBLE"};

};

#endif