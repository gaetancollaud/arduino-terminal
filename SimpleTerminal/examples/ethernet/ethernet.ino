#include <SimpleTerminal.h>
#include <SPI.h>
#include <Ethernet.h>

SimpleTerminal terminal(NULL);

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(172, 17, 10, 12);
EthernetServer server(80);

int var1;

void setup() {
	Serial.begin(115200);
	terminal.addCommand("test", (void*) test, "ma super description");
	terminal.addVar("var1", INT, &var1);

	Ethernet.begin(mac, ip);
	server.begin();
	Serial.print("server is at ");
	Serial.println(Ethernet.localIP());

}

void test(String &name, String &line) {
	Serial.print(name);
	Serial.print(" received, line => ");
	Serial.println(line);
}

void loop() {
	terminal.run();
	
	// listen for incoming clients
	EthernetClient client = server.available();
	if (client) {
		Serial.println("new client");
		
		//set stream to ethernet client
		terminal.setStream(&client);
		
		while (client.connected()) {
			terminal.run();
		}
		delay(1);
		client.stop();
		Serial.println("client disconnected");
		
		//set no stream
		terminal.setStream(NULL);
	}
}

