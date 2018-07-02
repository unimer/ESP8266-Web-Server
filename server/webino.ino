
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>

#define SDA 2
#define SCL 14

#define EEPROM0_I2C_ADDRESS 0x50
#define EEPROM1_I2C_ADDRESS 0x51
#define EEPROM_WRITE_DELAY 0x06
#define EEPROM_SIZE 8192

void(* resetFunc) (void) = 0; //declare reset function @ address 0

const char* ssid = "rade021";
const char* password = "hjk567sdf";

ESP8266WebServer server(80);


char *form = NULL;
int len = 0;
int ln = 0;

String t;

bool webpage_load(void) {
  if(len) { free(form); }
  char* data = (char*)malloc(32);
  if(data == NULL) { return false; }
  eeprom_read(EEPROM1_I2C_ADDRESS, EEPROM_SIZE-2, &data, 2);
  len = (data[0] << 8) | data[1];
  Serial.println(len, DEC);
  form = (char*)malloc(len+1);
  Serial.println("Malloc");
  if(form == NULL) { free(data); return false; }
  for(int i=0; i<len; i+=4) {
      if (i < EEPROM_SIZE){
        eeprom_read(EEPROM0_I2C_ADDRESS, i, &data, 4);
        strncpy(&form[i], &data[0], 4);  
      }
      else{
        eeprom_read(EEPROM1_I2C_ADDRESS, i-EEPROM_SIZE, &data, 4);
        strncpy(&form[i], &data[0], 4);
      }
  /*for(int i=0; i<len; i++) {
      if (i < EEPROM_SIZE){
        eeprom_read(EEPROM0_I2C_ADDRESS, i, &data, 1);
        strncpy(&form[i], &data[0], 1);  
      }
      else{
        eeprom_read(EEPROM1_I2C_ADDRESS, i-EEPROM_SIZE, &data, 1);
        strncpy(&form[i], &data[0], 1);
      }*/
  }
  Serial.println("read done");
  /*
  for(int i=0; i<len; i+=32) {
    Serial.println(i, DEC);
    if (i + 32 < len){
      if (i < EEPROM_SIZE){
        eeprom_read(EEPROM0_I2C_ADDRESS, i, &data, 32);
        strncpy(&form[i], &data[0], 32);  
      }
      else{
        
        eeprom_read(EEPROM1_I2C_ADDRESS, i-EEPROM_SIZE, &data, 32);
        strncpy(&form[i], &data[0], 32);
      }
      
    }
    else{
      if (i < EEPROM_SIZE)
      {
        eeprom_read(EEPROM0_I2C_ADDRESS, i, &data, len-i);
        strncpy(&form[i], &data[0], len-i);
      }
      else{
        eeprom_read(EEPROM1_I2C_ADDRESS, i-EEPROM_SIZE, &data, len-i);
        strncpy(&form[i], &data[0], len-i);
      }
    }
  }
   */
  form[len] = '\0';
  free(data);
  return true;
}

void webpage_save() {
  char length[2] = {len >> 8, len};
  eeprom_write(EEPROM1_I2C_ADDRESS, EEPROM_SIZE-2, &length[0], 2);
  for(unsigned int i=0; i<len; i+=4) {
      if (i < EEPROM_SIZE) {
        eeprom_write(EEPROM0_I2C_ADDRESS, i, &form[i], 4);
      } else {
        eeprom_write(EEPROM1_I2C_ADDRESS, i-EEPROM_SIZE, &form[i], 4);
      }
  /*for(unsigned int i=0; i<len; i++) {
      if (i < EEPROM_SIZE) {
        eeprom_write(EEPROM0_I2C_ADDRESS, i, &form[i], 1);
      } else {
        eeprom_write(EEPROM1_I2C_ADDRESS, i-EEPROM_SIZE, &form[i], 1);
      }*/
  }
  Serial.println("store done");
  /*
  for(unsigned int i=0; i<len; i+=32) {
    Serial.println(i);
    if(i + 32 < len) {
      if (i < EEPROM_SIZE) {
        eeprom_write(EEPROM0_I2C_ADDRESS, i, &form[i], 32);
      } else {
        eeprom_write(EEPROM1_I2C_ADDRESS, i-EEPROM_SIZE, &form[i], 32);
      }
    } else {
      if (i < EEPROM_SIZE) {
        eeprom_write(EEPROM0_I2C_ADDRESS, i, &form[i], len-i);
      } else {
        eeprom_write(EEPROM1_I2C_ADDRESS, i-EEPROM_SIZE, &form[i], len-i);
      }
    }    
  }
   */
}

void eeprom_write(int eeprom_address, unsigned int data_address, char *data, int len) {
  Wire.beginTransmission(eeprom_address);
  Wire.write((byte)(data_address >> 8 ));
  Wire.write((byte)(data_address & 0xFF));
  if(len > 32) { len = 32; }

  for(int i=0; i<len; i++) {
    Wire.write(data[i]);
  }
  Wire.endTransmission(true);
  delay(EEPROM_WRITE_DELAY);
}

void eeprom_read(int eeprom_address, unsigned int data_address, char **data, int len) {
  Wire.beginTransmission(eeprom_address);
  Wire.write((byte)(data_address >> 8));
  Wire.write((byte)(data_address & 0xFF));
  Wire.endTransmission(false); 
  if(len > 32) { len = 32; }
  Wire.requestFrom(eeprom_address, len);
  for(int i=0; i<len; i++) {
    data[0][i] =  Wire.read();
  }
  Wire.endTransmission(true);
  delay(EEPROM_WRITE_DELAY);
}

void handleGet() {

    server.send(200, "text/html",  form);

}

void handlePost() {                         // If a POST request is made to URI /login
  // Receved new web page code.
  if (server.hasArg("update")) {
    if(len) { free(form); }
    len = server.arg("update").toInt();
    form = (char*) malloc(len+1);
    if (form == NULL) {
      server.send(200, "text/plain", "1");
      resetFunc();
    }
    form[len] = '\0';
    server.send(200, "text/plain", "2");
  }
  else if (server.hasArg("address") && server.hasArg("value") && server.hasArg("size"))
  {
    if(server.arg("address").toInt() + server.arg("size").toInt() <= len) {
      //form[server.arg("address").toInt()] = server.arg("value").toInt();
      strncpy(&form[server.arg("address").toInt()], server.arg("value").c_str(), server.arg("size").toInt());
      server.send(200, "text/plain", "3");
    } else {
      server.send(200, "text/plain", "4");
    }
  } else if ( server.hasArg("store") && server.arg("store").toInt() == 1 ) {
    webpage_save();
    server.send(200, "text/plain", "5");
  } else if ( server.hasArg("restore") && server.arg("restore").toInt() == 1 ) {
    if ( webpage_load() == true ) {
      
      //for(int i=0; i<len; i++) Serial.print(form[i]);
      server.send(200, "text/plain", "6");
    } else {
      server.send(200, "text/plain", "7");
      resetFunc();
    }
  // Invalid arguments ignored.
  } else {
    server.send(200, "text/plain", "0");
  }

return;
  if (server.hasArg("update"))
  {
    Serial.print("  Updating server with file: ");
    Serial.println(server.arg("update"));
    server.send(200, "text/plain", "Update started");
  }

  if(server.hasArg("clear"))
  {
    free(form);
    form = NULL;
    len =0;
    server.send(200, "text/plain", "Memory cleared");
    Serial.println("  Memory cleared.");
  }

  if (server.hasArg("size"))
  {
    ln = server.arg("size").toInt();
    Serial.print("  File size: ");
    Serial.print(ln/1000);
    Serial.println(" kB");
      
  }
  if (server.hasArg("finished"))
  {
    Serial.print("  Server updated, page stored to RAM ");
    server.send(400, "text/plain", "Page stored to RAM ");    
    Serial.println("");    
    
  }
  if (server.hasArg("flash"))
  {
    webpage_save();
    Serial.print("Web page stored in eeprom.");
    server.send(400, "text/plain", "Web stored in eeprom");    
    Serial.println("");    
  }
  
  else if( ! server.hasArg("address") || ! server.hasArg("value") 
      || server.arg("address") == NULL || server.arg("value") == NULL )
  {
    server.send(400, "text/plain", "");
    return;
  }
  else if ( server.hasArg("address"))
  {
    int address = server.arg("address").toInt();
    if(len < address)
    {
      form = (char*) realloc(form, address+1);
      
      if(form == NULL)
      {
        server.send(400, "text/plain", "Cant allocate memory");
        len = 0;
      }
      else
      {
        form[address+1] = '\0';
        Serial.println("Allocated");
        form[address] = server.arg("value").toInt();
        server.send(400, "text/plain", "Allocated");
        len = address;
      }
    } 
    else
    {
      Serial.println("Value Stored");
      form[address] = server.arg("value").toInt();
      server.send(400, "text/plain", "");

    }
  }
   

}

void handleNotFound(){
  //digitalWrite(led, 1);
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
//  digitalWrite(led, 0);
}



void setup(void){
  Serial.begin(115200);
  Wire.begin(SDA, SCL);
  
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

  server.on("/", HTTP_GET, handleGet);
  server.on("/", HTTP_POST, handlePost);
  
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  
  //webpage_load();
  Serial.println("EEPROM loaded.");
}

void loop(void){
  server.handleClient();
}
