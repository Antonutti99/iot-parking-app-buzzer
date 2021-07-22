#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "EasyBuzzer.h"

unsigned int frequency = 1000;  
unsigned int onDuration = 50;
unsigned int offDuration = 100;
unsigned int beeps = 1;
unsigned int pauseDuration = 500;
unsigned int cycles = 1;

long dist;

int pin = 2;

const char* topic_name = "ultrasonic/+";

const char* ssid = "TIM-20917905";
const char* password = "antonutti_lollipop5";
const char* mqtt_server = "192.168.1.133";

WiFiClient espClient;
PubSubClient client(espClient);


void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void done() {
	Serial.println("Cambio frequenza!");
}

void setBuzzer(){
  EasyBuzzer.beep(
		frequency,		// Frequency in hertz(HZ). 
		onDuration, 	// On Duration in milliseconds(ms).
		offDuration, 	// Off Duration in milliseconds(ms).
		beeps, 			// The number of beeps per cycle.
		pauseDuration, 	// Pause duration.
		cycles, 		// The number of cycle.
		done			// [Optional] Callback. A function to call when the sequence ends.
	);
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);
  long distPart = messageTemp.toInt();

  if (topic=="ultrasonic/0"){
    dist = distPart;
  }
  else if (distPart<dist){
    dist = distPart;
  }
  if (topic=="ultrasonic/2"){
    if (dist < 5){
      EasyBuzzer.beep(1000);
    }
    else if (dist < 20){
      offDuration = 100;
      onDuration = 50;
      setBuzzer();
    }
    else if (dist < 40){
      offDuration = 150;
      onDuration = 50;
      setBuzzer();
    }else{
      onDuration = 0;
      setBuzzer();
    }
    Serial.println(dist);
  }
Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    /*
     YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a new name to the ESP8266.
     Here's how it looks:
       if (client.connect("ESP8266Client")) {
     You can do it like this:
       if (client.connect("ESP1_Office")) {
     Then, for the other ESP:
       if (client.connect("ESP2_Garage")) {
      That should solve your MQTT multiple connections problem
    */
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe(topic_name);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  Serial.begin(115200);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


  EasyBuzzer.setPin(pin);
  setBuzzer();
  beeps = 3;
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop()){
    client.connect("ESP8266Client");
  }

  EasyBuzzer.update();
}