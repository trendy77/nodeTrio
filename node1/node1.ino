/**
* The MIT License (MIT)
* Copyright (c) 2018 by ThingPulse, Daniel Eichhorn
* Copyright (c) 2018 by Fabrice Weinberg
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* ThingPulse invests considerable time and money to develop these open source libraries.
* Please support us by buying our products (and not the clones) from
* https://thingpulse.com
*
*/

// WiFi includes
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>

String local_ip = "";
char* device_id = "nano1";
IPAddress subnet(255, 255, 255, 0);
IPAddress ip(10, 0, 77, 151);
IPAddress gateway(10, 0, 77, 100);
const char ssid[] = "Northern Frontier Interwebs";
const char pass[] = "num4jkha8nnk";


#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h" // legacy include: `#include "SSD1306.h"`

// Initialize the OLED display using Wire library
SSD1306Wire  display(0x3c, 5,4);

int scrNum = 0;
long theTime, lastTime = 0;



void drawLines() {
	for (int16_t i = 0; i<display.getWidth(); i += 4) {
		display.drawLine(0, 0, i, display.getHeight() - 1);
		display.display();
		delay(10);
	}
	for (int16_t i = 0; i < display.getHeight(); i += 4) {
		display.drawLine(0, 0, display.getWidth() - 1, i);
		display.display();
		delay(10);
	}
	delay(250);

	display.clear();
	for (int16_t i = 0; i < display.getWidth(); i += 4) {
		display.drawLine(0, display.getHeight() - 1, i, 0);
		display.display();
		delay(10);
	}
	for (int16_t i = display.getHeight() - 1; i >= 0; i -= 4) {
		display.drawLine(0, display.getHeight() - 1, display.getWidth() - 1, i);
		display.display();
		delay(10);
	}
	delay(250);
	display.clear();
	for (int16_t i = display.getWidth() - 1; i >= 0; i -= 4) {
		display.drawLine(display.getWidth() - 1, display.getHeight() - 1, i, 0);
		display.display();
		delay(10);
	}
	for (int16_t i = display.getHeight() - 1; i >= 0; i -= 4) {
		display.drawLine(display.getWidth() - 1, display.getHeight() - 1, 0, i);
		display.display();
		delay(10);
	}
	delay(250);
	display.clear();
	for (int16_t i = 0; i < display.getHeight(); i += 4) {
		display.drawLine(display.getWidth() - 1, 0, 0, i);
		display.display();
		delay(10);
	}
	for (int16_t i = 0; i < display.getWidth(); i += 4) {
		display.drawLine(display.getWidth() - 1, 0, i, display.getHeight() - 1);
		display.display();
		delay(10);
	}
	delay(250);
}

void drawRect(void) {
	for (int16_t i = 0; i < display.getHeight() / 2; i += 2) {
		display.drawRect(i, i, display.getWidth() - 2 * i, display.getHeight() - 2 * i);
		display.display();
		delay(10);
	}
}

void fillRect(void) {
	uint8_t color = 1;
	for (int16_t i = 0; i < display.getHeight() / 2; i += 3) {
		display.setColor((color % 2 == 0) ? BLACK : WHITE); // alternate colors
		display.fillRect(i, i, display.getWidth() - i * 2, display.getHeight() - i * 2);
		display.display();
		delay(10);
		color++;
	}
	// Reset back to WHITE
	display.setColor(WHITE);
}

void drawCircle(void) {
	for (int16_t i = 0; i < display.getHeight(); i += 2) {
		display.drawCircle(display.getWidth() / 2, display.getHeight() / 2, i);
		display.display();
		delay(10);
	}
	delay(1000);
	display.clear();

	// This will draw the part of the circel in quadrant 1
	// Quadrants are numberd like this:
	//   0010 | 0001
	//  ------|-----
	//   0100 | 1000
	//
	display.drawCircleQuads(display.getWidth() / 2, display.getHeight() / 2, display.getHeight() / 4, 0b00000001);
	display.display();
	delay(200);
	display.drawCircleQuads(display.getWidth() / 2, display.getHeight() / 2, display.getHeight() / 4, 0b00000011);
	display.display();
	delay(200);
	display.drawCircleQuads(display.getWidth() / 2, display.getHeight() / 2, display.getHeight() / 4, 0b00000111);
	display.display();
	delay(200);
	display.drawCircleQuads(display.getWidth() / 2, display.getHeight() / 2, display.getHeight() / 4, 0b00001111);
	display.display();
}

String showScreen() {
	switch (scrNum){
	case '1': drawCircle();			 break;
	case '2':
		drawLines();
		break;
	case '3':
		drawRect(); break;
	case '4':
		fillRect();	delay(1000);	display.clear(); typeDis("IP is " + ipToString(WiFi.localIP())); break;
	case '5':
		printBuffer(); break;


	}
}

void printBuffer(void) {
	// Initialize the log buffer
	// allocate memory to store 8 lines of text and 30 chars per line.
	String toPrint = "UPTIME: ";
	toPrint += String(millis());
	toPrint += "millis.";
	int textch = toPrint.length();
	int t2 = local_ip.length();
	textch += t2;
	display.setLogBuffer(5, textch);
	const char * top = toPrint.c_str();
	const char * top2 = local_ip.c_str();
	const char* test[] = {
		top,
		top2,
		"the log buffer",
		"is",
		"working."
	};
	for (uint8_t i = 0; i < 11; i++) {
		display.clear();
		// Print to the screen
		display.println(test[i]);
		// Draw it to the internal screen buffer
		display.drawLogBuffer(0, 0);
		// Display it on the screen
		display.display();
	}
}
void typeDis(String dis) {
	display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
	display.setFont(ArialMT_Plain_16);
	display.drawString(display.getWidth() / 2, display.getHeight() / 2, dis);
	display.display();
}

String ipToString(IPAddress address){
	return String(address[0]) + "." +
		String(address[1]) + "." +
		String(address[2]) + "." +
		String(address[3]);
}

void setup() {
	Serial.begin(115200); 
	Serial.print("Starting WIFI");
	display.init();
	display.flipScreenVertically();
	display.setContrast(255);

	typeDis("Starting WiFi");
	
	WiFi.config(ip, gateway, subnet);
	WiFi.begin(ssid, pass);
	while (WiFi.status() != WL_CONNECTED) {
		delay(10);
	}
	typeDis("Connected to " + Wi);
	local_ip = ipToString(WiFi.localIP());

	ArduinoOTA.begin();
	ArduinoOTA.onStart([]() {
		display.clear();
		display.setFont(ArialMT_Plain_10);
		display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
		display.drawString(display.getWidth() / 2, display.getHeight() / 2 - 10, "OTA Update");
		display.display();
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		display.drawProgressBar(4, 32, 120, 8, progress / (total / 100));
		display.display();
	});
	ArduinoOTA.onEnd([]() {
		display.clear();
		display.setFont(ArialMT_Plain_10);
		display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
		display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Restart");
		display.display();
	});
	// Align text vertical/horizontal center
	display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
	display.setFont(ArialMT_Plain_10);
	display.drawString(display.getWidth() / 2, display.getHeight() / 2, "Ready for OTA:\n" + WiFi.localIP().toString());
	display.display();
	
	scrNum = 1;
	showScreen();
}

void loop() {
	ArduinoOTA.handle();
	theTime = millis();

	if (theTime >= (lastTime + 10000)) {
		display.clear();
		showScreen();
		scrNum++;
	}
}
