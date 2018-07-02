#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <pgmspace.h>


const char* ssid = "YOUR SSID";
const char* password = "YOUR PASSWORD";

ESP8266WebServer server(80);

String form =                                             // String form to sent to the client-browser
"<!DOCTYPE html><html><body><center> <h2>Monitored site</h2><img src=\"https://images.duckduckgo.com/iu/?u=http%3A%2F%2Ftheramblernews.com%2Fwp-content%2Fuploads%2F2013%2F09%2FNSA.jpg&f=1\" alt=\"NSA\" align=\"center\" width=350 height=350></center></body></html>";

void handleRoot() {
  server.send(200, "text/html", form);
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("Wi-Fi unavailable\n");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();

  
}
