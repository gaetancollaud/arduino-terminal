#include "Arduino.h"
#include "SimpleTerminal.h"

SimpleTerminal::SimpleTerminal(Stream &stream) {
	this->stream = &stream;
};

bool SimpleTerminal::addVar(String name, VarType type, void *ptr, const void *fnPtr) {
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

bool SimpleTerminal::addCommand(String name, const void *fnPtr, String description) {
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

void SimpleTerminal::run() {
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

void SimpleTerminal::printHelp() {
	commandHelp();
}

//------------------------------
// PRIVATE
//------------------------------

void SimpleTerminal::anaylseLine(String &line) {
	if (line.length()) {
		String cmd = getFirstWord(line);
		if (cmd.equals("set")) {
			this->commandSet(line);
		} else if (cmd.equals("printenv")) {
			this->commandPrintEnv();
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

String SimpleTerminal::getFirstWord(String &line) {
	int firstSpace = line.indexOf(' ');
	if (firstSpace == -1) {
		firstSpace = line.length();
	}
	return line.substring(0, firstSpace);
}

void SimpleTerminal::commandHelp() {
	stream->println("==================================");
	stream->println("          SimpleTerminal");
	stream->println("==================================");
	stream->println("command available : ");
	stream->println(" - help");
	stream->println(" - printenv");
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

void SimpleTerminal::commandPrintEnv() {
	stream->println("List of variabless ");
	stream->println("--------------------");
	for (int i = 0; i < regVarIndex; i++) {
		printVar(&vars[i]);
	}
}

void SimpleTerminal::commandSet(String &line) {
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

void SimpleTerminal::commandGet(String &line) {
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

void SimpleTerminal::printVar(RegVar *var) {
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
