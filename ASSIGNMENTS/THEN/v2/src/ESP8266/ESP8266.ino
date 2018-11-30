#include <ESP8266WiFi.h>
// Base code via https://www.instructables.com/id/Quick-Start-to-Nodemcu-ESP8266-on-Arduino-IDE/

#define ledPin BUILTIN_LED

// Note builtin treats HIGH as off & reverse
#if ledPin == BUILTIN_LED
  #define ON  LOW
  #define OFF HIGH
#else
  #define ON  HIGH
  #define OFF LOW
#endif


const char* ssid = "GentriFi";
const char* password = "Yeezy2020";


WiFiServer server(80);

void setup() {
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
  while(!client.available()){
    delay(1);
  }

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // Match the request

  int value = OFF;
  if (request.indexOf("/LED=ON") != -1)  {
    digitalWrite(ledPin, ON);
    value = ON;
  }
  if (request.indexOf("/LED=OFF") != -1)  {
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

  client.println("<!DOCTYPE HTML>");
  client.println("<html><head><meta charset='utf-8'><meta http-equiv='X-UA-Compatible' content='IE=edge'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>"); // uses lit 🔥 css framework
  client.println("<link rel=\"stylesheet\" href=\"https://cdn.jsdelivr.net/npm/@ajusa/lit@latest/dist/lit.css\" />");
  client.println("</head><body class='c'>");
  // end </head>

  client.print("Led pin is now: ");

  if(value == ON) {
    client.print("On");
  } else {
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/LED=ON\"\"><button class='btn'>Turn On </button></a>");
  client.println("<a href=\"/LED=OFF\"\"><button class='btn'>Turn Off </button></a><br />");
  client.println("</body></html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}

// #include <ESP8266WiFi.h>
// #include <WiFiClient.h>
// #include <ESP8266WebServer.h>
// #include <WebSocketsServer.h>
// #include <Hash.h>

// // Replace with your network credentials
// const char *ssid = "ConcordiaUniversity";
// const char *password = "<YOUR WIFI PASSWORD>";

// WebSocketsServer webSocket = WebSocketsServer(81);
// ESP8266WebServer server(80); // instantiate server at port 80 (http port)

// String page = "";
// int LEDPin = LED_BUILTIN;

// void setup(void)
// {
//   //the HTML of the web page
//   page = "<h1>Simple NodeMCU Web Server</h1><p><a href=\"LEDOn\"><button>ON</button></a>&nbsp;<a href=\"LEDOff\"><button>OFF</button></a></p>";
//   //make the LED pin output and initially turned off
//   pinMode(LEDPin, OUTPUT);
//   digitalWrite(LEDPin, LOW);

//   delay(1000);

//   Serial.begin(115200);
//   WiFi.begin(ssid, password); // begin WiFi connection
//   Serial.println("");

//   // Wait for connection
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(500);
//     Serial.print(".");
//   }

//   Serial.println("");
//   Serial.print("Connected to ");
//   Serial.println(ssid);
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP());

//   server.on("/", []() {
//     server.send(200, "text/html", page);
//   });

//   server.on("/LEDOn", []() {
//     server.send(200, "text/html", page);
//     digitalWrite(LEDPin, HIGH);
//     delay(1000);
//   });

//   server.on("/LEDOff", []() {
//     server.send(200, "text/html", page);
//     digitalWrite(LEDPin, LOW);
//     delay(1000);
//   });

//   server.begin();
//   webSocket.begin();
//   webSocket.onEvent(webSocketEvent);

//   Serial.println("Web server started!");
// }

// void loop(void)
// {
//   webSocket.loop();
//   server.handleClient();
//   if (Serial.available() > 0)
//   {
//     char c[] = {(char)Serial.read()};
//     webSocket.broadcastTXT(c, sizeof(c));
//   }
// }

// void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
// {
//   if (type == WStype_TEXT)
//   {
//     for (int i = 0; i < length; i++)
//       Serial.print((char)payload[i]);
//     Serial.println();
//   }
// }
