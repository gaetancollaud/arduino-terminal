#include <SimpleTerminal.h>

SimpleTerminal terminal(Serial);

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

