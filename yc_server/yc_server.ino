#include <SPI.h>
#include <Dhcp.h>
#include <Dns.h>
#include <WiFiEsp.h>
#include <WiFiEspUdp.h>
#include <coap.h>
#include "SoftwareSerial.h"



char ssid[] = "AndroidAP";            // your network SSID (name)
char pass[] = "qswp2014";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status
char TepTemp[20];
char record='\0';
int Realdat;
int cnt=0;

// CoAP client response callback
void callback_response(CoapPacket &packet, IPAddress ip, int port);

// CoAP server endpoint url callback
void callback_light(CoapPacket &packet, IPAddress ip, int port);

// UDP and CoAP class
WiFiEspUDP Udp;
Coap coap(Udp);

// LED STATE

SoftwareSerial Serial1(4, 5); // RX, TX

// CoAP server endpoint URL ge
CoapPacket packetTmp;

IPAddress ipTmp;
int portTmp;
void callback_light(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Light] ON/OFF");
  
  // send response
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  
  String message(p);

  if (message.equals("0"))
  {
  digitalWrite(11, LOW);
  coap.sendResponse(ipTmp, portTmp, packetTmp.messageid,TepTemp);
  record='0';
  }
    
  else if(message.equals("1"))
  {

  digitalWrite(11, HIGH);
  coap.sendResponse(ipTmp, portTmp, packetTmp.messageid,TepTemp);
  record='1';
  }

  Serial.print(packet.messageid);
   
  packetTmp=packet;
  ipTmp=ip;
  portTmp=port;
}

// CoAP client response callback
void callback_response(CoapPacket &packet, IPAddress ip, int port) {
  Serial.println("[Coap Response got]");
  
  char p[packet.payloadlen + 1];
  memcpy(p, packet.payload, packet.payloadlen);
  p[packet.payloadlen] = NULL;
  
  
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  WiFi.init(&Serial1);

  Serial.println(ssid);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue:
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }
 
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  
  // LED State
  pinMode(11, OUTPUT);
  digitalWrite(11, HIGH);
 //Buzzer State
 pinMode(6,OUTPUT);   
 digitalWrite(6,LOW); 
 
  Serial.println("Setup Callback Light");
  coap.server(callback_light, "light");

  // client response callback.
  
  Serial.println("Setup Response Callback");
  coap.response(callback_response);

  // start coap server/client
  coap.start();
}

void loop() {
  // send GET or PUT coap request to CoAP server.
  // To test, use libcoap, microcoap server...etc
  
 
  
  uint16_t val;
    double dat;
    val=analogRead(A0);//Connect LM35 on Analog 0
    dat = (double) val * (5/10.24); 
    Serial.print("Tep:"); //Display the temperature on Serial monitor
    Serial.print(dat);
    Serial.println("C");
    Realdat = (int) (dat*100);
    sprintf(TepTemp,"%d\n",Realdat);

    

    if(dat>=40)
    {
       if(record=='1')
       {
         digitalWrite(11, HIGH);
         digitalWrite(6, LOW);
       }
        else if(record=='0')
        {
           cnt++;
           Serial.print("count=");
           Serial.print(cnt);
           Serial.print("\n");
    
          digitalWrite(11, LOW);
          if(cnt==6)
          {
     
            cnt=0;
            record='\0';
          }
            
        }         
       else 
       {
         digitalWrite(6, HIGH);
         digitalWrite(11, HIGH);   //Turn on led
         delay(200);
         digitalWrite(11, LOW);    //Turn off led 
       }
    }

  delay(1000);    
  coap.loop();
}

