
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#define REED_SWITCH_D5 14 //D5 GPIO PIN 14
#define REED_SWITCH_D6 12 //D6 GPIO PIN 12
#define REED_SWITCH_D7 13 //D6 GPIO PIN 13
#define DOOR_OUTPUT 16 //Light Sensor Red

const char* ssid = "name";
const char* ssid2 = "name2";
const char* password = "password";
const char* password2 = "password";
const char* mqtt_server = "0.0.0.0";
const int mqtt_port = 1883;
const char* mqtt_user = "user";
const char* mqtt_pass = "userpassword";
const char* mqtt_client_name = "sonoff"; // Client connections cant have the same connection name
const char* mqtt_topic= "home/doors/stat";
const char* mqtt_topic_basement = "home/doors/basement/stat";
const char* mqtt_topic_basement_stat_on = "Open";
const char* mqtt_topic_basement_stat_off = "Closed";
const char* mqtt_topic_kitchen = "home/doors/kitchen/stat";
const char* mqtt_topic_kitchen_stat_on = "Open";
const char* mqtt_topic_kitchen_stat_off = "Closed";
const char* mqtt_topic_front_door = "home/doors/front/stat";
const char* mqtt_topic_front_door_stat_on = "Open";
const char* mqtt_topic_front_door_stat_off = "Closed";

WiFiClient espClient;
PubSubClient client(espClient);
// Variables
int doorClosed_d5 = 1;
int doorClosed_d6 = 1;
int doorClosed_d7 = 1;
bool boot = true;
long lastMsg = 0;
char msg[50];
int value = 0;

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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == 'O') {
    // digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    Serial.println("DOOR Open- callback ");
    //Serial.println((char)payload[0]);
    Serial.println(doorClosed_d5);
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else{
    // digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
    Serial.println("DOOR Closed- callback ");
    //Serial.println((char)payload[0]);
    Serial.println(doorClosed_d5);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    // String clientId = "kitchendoor-";
    // clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(mqtt_client_name, mqtt_user, mqtt_pass, mqtt_topic, 0, 0, "closed")) {
      Serial.print("connected: ");
      // Once connected, publish an announcement...
      client.publish("home/doors/stat", "Connected");
      // ... and resubscribe
      client.subscribe(mqtt_topic);
      Serial.println(WiFi.localIP());
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Code here only runs once at startup
void setup() {
  pinMode(DOOR_OUTPUT, OUTPUT); //Built in LED HIGH = OFF, LOW = ON
  pinMode(REED_SWITCH_D5, INPUT_PULLUP); // Switch open = HIGH, switch closed = LOW  | BASEMENT
  pinMode(REED_SWITCH_D6, INPUT_PULLUP); // Switch open = HIGH, switch closed = LOW  | FRONT DOOR
  pinMode(REED_SWITCH_D7, INPUT_PULLUP); // Switch open = HIGH, switch closed = LOW  | Kitchen DOOR
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  long now = millis();

  // Basement Back Door Reed Switch OPENED
  if ((digitalRead(REED_SWITCH_D5) == HIGH) && (doorClosed_d5 == 1))
  {
    Serial.print("Basement DOOR is: ");
    Serial.println(mqtt_topic_basement_stat_on);
    digitalWrite(DOOR_OUTPUT, LOW);
    client.publish(mqtt_topic_basement, mqtt_topic_basement_stat_on, true); 
    doorClosed_d5 = 0;
    
  }
  // Basement Back Door Reed Switch CLOSED
  else if ((digitalRead(REED_SWITCH_D5) == LOW) && (doorClosed_d5 == 0))
  {
    Serial.print("Basement DOOR is: ");
    Serial.println(mqtt_topic_basement_stat_off);
    digitalWrite(DOOR_OUTPUT, HIGH);
    client.publish(mqtt_topic_basement, mqtt_topic_basement_stat_off, true);
    doorClosed_d5 = 1;
  }

  // Front Door Reed Switch OPENED
  if ((digitalRead(REED_SWITCH_D6) == HIGH) && (doorClosed_d6 == 1))
  {
    Serial.print("Front Door DOOR is: ");
    Serial.println(mqtt_topic_front_door_stat_on);
    digitalWrite(DOOR_OUTPUT, LOW);
    client.publish(mqtt_topic_front_door, mqtt_topic_front_door_stat_on, true); 
    doorClosed_d6 = 0;
    
  }
  // Front Door Reed Switch CLOSED
  else if ((digitalRead(REED_SWITCH_D6) == LOW) && (doorClosed_d6 == 0))
  {
    Serial.print("Front Door is: ");
    Serial.println(mqtt_topic_front_door_stat_off);
    digitalWrite(DOOR_OUTPUT, HIGH);
    client.publish(mqtt_topic_front_door, mqtt_topic_front_door_stat_off, true);
    doorClosed_d6 = 1;
  }

  // Kitchen Reed Switch OPENED
  if ((digitalRead(REED_SWITCH_D7) == HIGH) && (doorClosed_d7 == 1))
  {
    Serial.print("Kitchen DOOR is: ");
    Serial.println(mqtt_topic_kitchen_stat_on);
    digitalWrite(DOOR_OUTPUT, LOW);
    client.publish(mqtt_topic_kitchen, mqtt_topic_kitchen_stat_on, true); 
    doorClosed_d7 = 0;
    
  }
  // Kitchen Door Reed Switch CLOSED
  else if ((digitalRead(REED_SWITCH_D7) == LOW) && (doorClosed_d7 == 0))
  {
    Serial.print("Kitchen DOOR is: ");
    Serial.println(mqtt_topic_kitchen_stat_off);
    digitalWrite(DOOR_OUTPUT, HIGH);
    client.publish(mqtt_topic_kitchen, mqtt_topic_kitchen_stat_off, true);
    doorClosed_d7 = 1;
  }
  delay(10);
}
