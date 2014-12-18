#ifndef SIMPLE_THERMINAL_h
#define SIMPLE_THERMINAL_h

#include <Arduino.h>

#ifndef SIMPLE_THERMINAL_MAX_VAR
#define SIMPLE_THERMINAL_MAX_VAR 10
#endif

//#define SIMPLE_THERMINAL_DEBUG

String VarTypePretty[] = {"BOOL", "INT", "LONG", "FLOAT", "DOUBLE"};

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

	SimpleTerminal(Stream &stream) {
		this->stream = &stream;
	};

	bool addVar(String name, VarType type, void *ptr = 0, const void *fnPtr = 0) {
		if (regVarIndex == SIMPLE_THERMINAL_MAX_VAR) {
			return false;
		} else {
			RegVar *reg = &vars[regVarIndex++];
			reg->name = name;
			reg->type = type;
			reg->ptr = ptr;
			reg->ptrFunc = *((FN_CALLBACK*) (&fnPtr));
		}
		return true;
	}

	bool addCommand(String name, const void *fnPtr, String description = "") {
		if (regCmdIndex == SIMPLE_THERMINAL_MAX_VAR) {
			return false;
		} else {
			RegCmd *reg = &cmds[regCmdIndex++];
			reg->name = name;
			reg->fn = *((FN_CALLBACK*) (&fnPtr));
			reg->description = description;
		}
		return true;
	}

	void run() {
		while (stream->available()) {
			char c = stream->read();
			if (c == '\r') {
				//ignore
			} else if (c == '\n') {
				anaylseLine(buffer);
				buffer = String();
			} else {
				buffer += c;
			}
		}
	}

	void printHelp() {
		this->commandHelp();
	}

private:

	int emptyLine = 0;

	void anaylseLine(String &line) {
		if (line.length()) {
			String cmd = getFirstWord(line);
#ifdef SIMPLE_THERMINAL_DEBUG
			Serial.print("new command : ");
			Serial.println(cmd);
#endif

			if (cmd.equals("set")) {
				this->commandSet(line);
			} else if (cmd.equals("list")) {
				this->commandList();
			} else if (cmd.equals("get")) {
				this->commandGet(line);
			} else if (cmd.equals("help")) {
				this->commandHelp();
			} else {
				bool found = false;
				for (int i = 0; i < regCmdIndex; i++) {
					RegCmd* c = &cmds[i];
					Serial.print("test name :");
					Serial.println(c->name);
					if (cmd.equals(c->name)) {
						c->fn(cmd, line);
						found = true;
						break;
					}
				}

				if (!found) {
					stream->print("Unknown command : ");
					stream->println(cmd);
				}
			}
		}
	}

	String getFirstWord(String &line) {
		int firstSpace = line.indexOf(' ');
		if (firstSpace == -1) {
			firstSpace = line.length();
		}
		return line.substring(0, firstSpace);
	}

	void commandHelp() {
		stream->println("==================================");
		stream->println("          SimpleTerminal");
		stream->println("==================================");
		stream->println("command available : ");
		stream->println(" - help");
		stream->println(" - list");
		stream->println(" - set <varName> <newValue>");
		stream->println(" - get <varName>");
		for (int i = 0; i < regCmdIndex; i++) {
			RegCmd* c = &cmds[i];
			stream->print(" - ");
			stream->print(c->name);
			stream->print(" ");
			stream->println(c->description);
		}
		stream->println("==================================");
	}

	void commandList() {
		stream->println("List of variabless ");
		stream->println("--------------------");
		for (int i = 0; i < regVarIndex; i++) {
			printVar(&vars[i]);
		}
	}

	void commandSet(String &line) {
		int endName = line.indexOf(' ', 4);
		if (endName == -1 || endName == line.length()) {
			stream->println("wrong parameter for set command");
		} else {
			String name = line.substring(4, endName);
			RegVar* var = 0;

			for (int i = 0; i < regVarIndex; i++) {
				if (name.equals(vars[i].name)) {
					var = &vars[i];
					break;
				}
			}
			if (var) {
				String value = line.substring(endName + 1, line.length());
				if (var->ptr) {
					switch (var->type) {
						case FLOAT:
						case DOUBLE:
							*(float*) var->ptr = value.toFloat();
							break;
						case BOOL:
							if (value.equals("true")) {
								*(bool*)var->ptr = true;
							} else {
								*(bool*)var->ptr = value.toInt() != 0;
							}
							break;
						case LONG:
						case INT:
						default:
							*((long*) var->ptr) = value.toInt();
							break;
					}
				}
				printVar(var);
				if (var->ptrFunc) {
					var->ptrFunc(name, value);
				}
			} else {
				stream->print("Var not found : ");
				stream->println(name);
			}
		}
	}

	void commandGet(String &line) {
		String name = line.substring(4, line.length());
		bool found = false;
		for (int i = 0; i < regVarIndex; i++) {
			if (name.equals(vars[i].name)) {
				printVar(&vars[i]);
				found = true;
				break;
			}
		}
		if (!found) {
			stream->print("Var not found : ");
			stream->println(name);
		}
	}

	void printVar(RegVar *var) {
		stream->print(var->name);
		stream->print('\t');
		stream->print(VarTypePretty[var->type]);
		stream->print('\t');
		if (var->ptr) {
			switch (var->type) {
				case FLOAT:
					stream->print(*(float*) var->ptr);
					break;
				case DOUBLE:
					stream->print(*(double*) var->ptr);
					break;
				case BOOL:
					stream->print((*(bool*) var->ptr) == 0 ? "false" : "true");
					break;
				case LONG:
					stream->print(*(long*) var->ptr);
					break;
				case INT:
					stream->print(*(int*) var->ptr);
					break;
				default:
					stream->print("-");
			}
		} else {
			stream->print("-");
		}
		stream->println();
	}

	int regVarIndex = 0;
	RegVar vars[SIMPLE_THERMINAL_MAX_VAR];
	int regCmdIndex = 0;
	RegCmd cmds[SIMPLE_THERMINAL_MAX_VAR];

	Stream* stream;

	String buffer;

};

#endif