/**
 * This code is to be uploaded to the NodeMCU module,
 * or adapted for use on the ESP directly.
 * --
 * For future note: connectivity should be done via
 * a) An app with direct communication to the MCU
 * b) automatic pairing via a phone hotspot (ask user to set up a predetermined hotspot,
 * negotiate broader network credentials from there)
 * c) local network (as is curretly configured, then input data so to connect to actual internet)
 * d) build a separate interface with knobs and buttons, and skip the cellphone altogether.
 *
 * Currently a Serial connection
 * Base code via
 * https://www.instructables.com/id/Quick-Start-to-Nodemcu-ESP8266-on-Arduino-IDE/
 */
#include <ESP8266WiFi.h>

#define ledPin BUILTIN_LED

// Note builtin treats HIGH as off & reverse
#if ledPin == BUILTIN_LED
  #define ON  LOW
  #define OFF HIGH
#else
  #define ON  HIGH
  #define OFF LOW
#endif


const char* ssid = "XXXXXXXXXXXXXXXXXX"; // WiFi Name
const char* password = "XXXXXXXXXXXXXX"; // WiFi Password

// Start wifi server (ESP is now an AP)
WiFiServer server(80);

void setup() {
  // Serial line to the Teensy
  // NOTE: Serial tx/rx pins mirror USB line
  Serial.begin(115200);
  delay(10);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, OFF);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial1.println("WIFI READY");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  /**
   * NOTE: in my rush to accomplish this interaction whilst sick,
   * the original code was lost.
   * The code below is NOT what was running during the final presentation,
   * although it illustrates the concept.
   */
  int value = OFF;
  if (request.indexOf("/LED=ON") != -1) {
    digitalWrite(ledPin, ON);
    value = ON;
  }
  if (request.indexOf("/LED=OFF") != -1) {
    digitalWrite(ledPin, OFF);
    value = OFF;
  }

  if (request.indexOf("/bitcrush") != -1) {
    Serial.println("b");
  }

  // Set ledPin according to the request
  // digitalWrite(ledPin, value);

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one

  client.println(renderHeader("index"));
  client.println(renderPage(0));

  client.print("Led pin is now: ");

  if (value == ON) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println(
      "<a href=\"/LED=ON\"\"><button class='btn'>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button class='btn'>Turn Off "
                 "</button></a><br />");
  client.println("</body></html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}

// I have recreated missing functions that I've unfortunately seemingly lost.
// these current commits may be omitted from grading, I am including them purely
// for completeness, and as I intend on developing the project further
// missing functions from final: renderHeader(), renderpage() and parseurl()

String renderHeader(String title) {
  String html = ""; // declare the response
  // NOTE: in the future, use PROGMEM to store this data.
  // see: https://github.com/esp8266/Arduino/blob/master/doc/PROGMEM.rst

  // for code clarity, I've utilised the implicit string literal
  // concatenation in the c++ compiler.
  html = "<!DOCTYPE HTML>"
         "<html><head><meta charset='utf-8'>"
         "<meta http-equiv='X-UA-Compatible' content='IE=edge'>"
         "<meta name='viewport' content='width=device-width, initial-scale=1'>"
         "<link rel='stylesheet' " // uses lit 🔥 css framework, chosen for its size
         "href='https://cdn.jsdelivr.net/npm/@ajusa/lit@latest/dist/lit.css' />";

  html += "<title>" + title + "</title>"; // pass in the page title
  html += "</head><body class='c'>"; // container class on the body, derived from Lit.

  return html;
}

String renderPage(int pageID) {
  String html = "";


  switch (pageID)
  {
    case 0:
      html += "index page";
      break;

    case 2:
      html += "<button class='btn'>bitcrush</bitcrush>"; // bitcrush explanations here
      break;

    default:
      break;
  }

  html += "</body></html>"; // close tags

  return html;
}
