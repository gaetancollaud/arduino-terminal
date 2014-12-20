SimpleTerminal
================

The SimpleTerminal library allow you to easily create a terminal for your arduino projects. It supports all kind of streams such as Serial communication and ethernet communication. I wrote some examples.

You can register a command and set the callback function or you can directly register a variable that can later be edited in the terminal.

##Commands

To register a command simply use _addCommand("commandNameString", (void*)function, "nonMandatoryCommandDescriptionString")_.

Example ([sketch](https://github.com/gaetancollaud/arduino-terminal/tree/master/SimpleTerminal/examples/commands)): 

```cpp
#include <SimpleTerminal.h>

SimpleTerminal terminal(&Serial);

void setup(){
	Serial.begin(115200);
	terminal.addCommand("test", (void*)test, "my pretty description");
}

void test(String &name, String &line){
	Serial.print(name);
	Serial.print(" received, line => ");
	Serial.println(line);
}

void loop(){
	terminal.run();
}
```
This way, when the command _test_ is received, the function _test(String, String)_ will be called. You must always have to String parameters for your callback functions.

##Variables

If you just want to implement a terminal to see and modify variables, you don't need to register any command. Everything is build in. Just use _addVar("varNameString", varType, varPointer, nonMandatoryVarChangedFunction)_. The type supported are :
* BOOL
* INT
* LONG
*	FLOAT
*	DOUBLE

You don't need to set a _varChangedFunction_, but this can be usefull if you want to call something once the var has changed. Example ([sketch](https://github.com/gaetancollaud/arduino-terminal/tree/master/SimpleTerminal/examples/variables)): 
```CPP
#include <SimpleTerminal.h>

SimpleTerminal terminal(&Serial);

int v1 = 10;
double v2 = 2.1588;
bool v3 = true;
int v4 = 0;

void setup(){
	Serial.begin(115200);
	terminal.addVar("v1", INT, &v1);
	terminal.addVar("v2", DOUBLE, &v2);
	terminal.addVar("v3", BOOL, &v3, (void*)vChanged);
	terminal.addVar("v4", INT, 0, (void*)vChanged);
	terminal.printHelp();
}

void vChanged(String &name, String &newValue){
	Serial.print(name);
	Serial.print(" changed to ");
	Serial.println(newValue);
}

void loop(){
	terminal.run();
}
```

### Usage example
**printenv**
```
List of variabless 
--------------------
v1	INT	10
v2	DOUBLE	2.16
v3	BOOL	true
v4	INT	-
```
**set v3 false**
```
v3	BOOL	false
v3 changed to false
```
