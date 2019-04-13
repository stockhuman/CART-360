#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <Hash.h>

// COM6

// Replace with your network credentials
const char *ssid = "GFi";
const char *password = "Y20";

WebSocketsServer webSocket = WebSocketsServer(81);
ESP8266WebServer server(80); // instantiate server at port 80 (http port)

String page = "";
int LEDPin = LED_BUILTIN;

#if LEDPin == BUILTIN_LED
  #define ON LOW
  #define OFF HIGH
#else
  #define ON HIGH
  #define OFF LOW
#endif

void setup(void)
{
  //the HTML of the web page
  page = renderPage(renderNormalPage());
  //make the LED pin output and initially turned off
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, OFF);

  delay(100);

  Serial.begin(115200);
  WiFi.begin(ssid, password); // begin WiFi connection
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("ESP READY");

  server.on("/", []() {
    server.send(200, "text/html", page);
  });

  server.on("/LEDOn", []() {
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, ON);
    Serial.println("ON");
    delay(10);
  });

  server.on("/LEDOff", []() {
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, OFF);
    Serial.println("OFF");
    delay(10);
  });

  server.on("/bitcrush", []() {
    server.send(200, "text/html", renderPage(renderBitcrusherPage()));
    Serial.println("b" + server.arg(0));
    delay(10);
  });

  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Serial.println("Web server started!");
}

void loop(void)
{
  webSocket.loop();
  server.handleClient();
  if (Serial.available() > 0)
  {
    char c[] = {(char)Serial.read()};
    webSocket.broadcastTXT(c, sizeof(c));
  }
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t
length)
{
  if (type == WStype_TEXT)
  {
    for (int i = 0; i < length; i++)
      Serial.print((char)payload[i]);
    Serial.println();
  }
}

String renderPage (String body) {
  String html =
    "<!DOCTYPE HTML>"
    "<html><head><meta charset='utf-8'>"
    "<meta http-equiv='X-UA-Compatible' content='IE=edge'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>" // uses lit 🔥 css framework
    "<link rel=\"stylesheet\"href=\"https://cdn.jsdelivr.net/npm/@ajusa/lit@latest/dist/lit.css\" />"
    "<style>body{background:#ff96ca;}</style>"
    "</head><body class='c'>";
    // end </head>
  html += "<h1>Air Guitar</h1>";
  html += body;

  "</body></html>"; // end body

  return html;
}

String renderBitcrusherPage() {
  String html = "<p>Bitcrusher</p>"
  "<p> USE the url bar to control the bitcrush effect. bits 1 through 16."
  "<p><pre>EX: http:url.com/bitcrush?b=4</pre><p>";
  return html;
}

String renderNormalPage() {
  String html = "<p>normal</p>";
  return html;
}
