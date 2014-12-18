#include "SimpleTerminal.h"

SimpleTerminal terminal(Serial);


void setup(){
	Serial.begin(115200);
	terminal.addCommand("test", (void*)test, "ma super description");
	terminal.printHelp();
}

void test(String &name, String &line){
	Serial.print(name);
	Serial.print(" received, line => ");
	Serial.println(line);
}

void loop(){
	terminal.run();
}

