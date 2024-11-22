#include <Arduino.h>
#include <Wifi.h>
#include <PubSubClient.h>
#include <string>


// WiFi
const char *ssid = "home"; // Enter your WiFi name
const char *password = "home2014";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "192.168.0.100";
const char *topic = "home/bedroom/fan";
const char *mqtt_username = "mqtt_client";
const char *mqtt_password = "mqtt";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// pins definition
#define LED1 1
#define LED2 0

#define VoltagePin 3

// CFG1 CFG2 CFG3
// 1    x    x    5V
// 0    0    0    9V
// 0    0    1    12V
// 0    1    1    15V
// 0    1    0    20V
#define CFG1 6
#define CFG2 5
#define CFG3 4
#define PWR_EN 7
#define outputOn HIGH
#define outputOff LOW
#define PG 10

// Variables
enum voltage {V5, V9, V12, V15, V20};
uint8_t pwm_fan = 0;

// Functions prototypes
void monitorVoltage(void *pvParameters);
void configVoltage(voltage v);
float readVoltage(void);
bool checkVoltage(voltage v);
void callback(char *topic, byte *payload, unsigned int length);
void connectToClient(void);

void setup() {
  // put your setup code here, to run once:
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(CFG1, OUTPUT);
  pinMode(CFG2, OUTPUT);
  pinMode(CFG3, OUTPUT);
  pinMode(PWR_EN, OUTPUT);
  analogWriteFrequency(10000);
  analogReadResolution(8);
  analogWrite(PWR_EN, 0);
  pinMode(PG, INPUT_PULLUP);

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);

  Serial.begin(115200);
  // connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  //connecting to a mqtt broker
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  connectToClient();

  // Config the voltage to 12V
  voltage v = V12;
  configVoltage(v);
  delay(500);

  // if(checkVoltage(v))
  // {
  //   delay(2000);
  //   Serial.println("Voltage is 12V");
  //   digitalWrite(LED1, LOW);
  //   digitalWrite(PWR_EN, outputOn);
  // }
  
  // Create task to flash LED1
  xTaskCreate(monitorVoltage,"LED1_task",10000,NULL,1,NULL);
  
}

void loop() {
  static uint8_t last_pwm_fan = 0;
  // put your main code here, to run repeatedly:
  // if(digitalRead(PG) == HIGH)
  // {
  //   Serial.print("* Power bad * ");
  //   digitalWrite(LED2, LOW);
  // }
  // else
  // {
  //   // Serial.print("* Power good * ");
  //   digitalWrite(LED2, HIGH);
  // }
  if(client.loop() == false)
  {
    connectToClient();
  }

  if(pwm_fan != last_pwm_fan)
  {
    last_pwm_fan = pwm_fan;
    analogWrite(PWR_EN, pwm_fan);
    Serial.println("PWM: " + String(pwm_fan));
    // client.publish(topic, String(pwm_fan).c_str());
  }
  // analogWrite(PWR_EN, pwm_fan);
  // if(checkVoltage(V12))
  // {
  //   if(pwm_fan != last_pwm_fan)
  //   {
  //     last_pwm_fan = pwm_fan;
  //     analogWrite(PWR_EN, pwm_fan);
  //     Serial.println("PWM: " + String(pwm_fan));
  //     // client.publish(topic, String(pwm_fan).c_str());
  //   }
  //   // analogWrite(PWR_EN, pwm_fan);
  // }
  // else
  // {
  //   analogWrite(PWR_EN, 0);
  // }

  // float voltage = readVoltage();
  // Serial.print(" Voltage: ");
  // Serial.println(voltage);
  // delay(1000);
}

void monitorVoltage(void *pvParameters)  // This is a task.
{
  (void) pvParameters;
  long last = millis();
  bool flash = false;

  while (1) {
    if(millis() - last > 1000)
    {
      last = millis();
      if(flash)
      {
        digitalWrite(LED2, !digitalRead(LED2));
      }
    }

    if(digitalRead(PG) == HIGH) // bad voltage
    {
      flash = 1;
    }
    else
    {
      flash = 0;
    }

  }
}

// Function that read the voltage with a voltage divider of 1.69k and 10k
float readVoltage() {
  double voltage = 0;
  // Read the voltage 10 times and get the average
  for (int i = 0; i < 10; i++) {
    voltage += analogReadMilliVolts(VoltagePin)/1000.0;
    delay(10);
  }
  voltage = voltage / 10.0;
  // Transform the voltage to the real voltage
  voltage = voltage / (1.67 / (1.69 + 10.0)); // tweak a little bit the voltage divider to get the ish correct voltage.
  // Serial.println(analogReadMilliVolts(VoltagePin) / (1.69 / (1.69 + 10)));
  return voltage;
}

// Function that config the voltage
void configVoltage(voltage v)
{
  switch(v)
  {
    case V5:
      digitalWrite(CFG1, HIGH);
      digitalWrite(CFG2, LOW);
      digitalWrite(CFG3, LOW);
      break;
    case V9:
      digitalWrite(CFG1, LOW);
      digitalWrite(CFG2, LOW);
      digitalWrite(CFG3, LOW);
      break;
    case V12:
      digitalWrite(CFG1, LOW);
      digitalWrite(CFG2, LOW);
      digitalWrite(CFG3, HIGH);
      break;
    case V15:
      digitalWrite(CFG1, LOW);
      digitalWrite(CFG2, HIGH);
      digitalWrite(CFG3, HIGH);
      break;
    case V20:
      digitalWrite(CFG1, LOW);
      digitalWrite(CFG2, HIGH);
      digitalWrite(CFG3, LOW);
      break;
  }
}

// Function that check if the voltage is the same as the voltage selected
bool checkVoltage(voltage v)
{
  float voltage = readVoltage();
  switch(v)
  {
    case V5:
      if(voltage > 4.5 && voltage < 5.5)
      {
        return true;
      }
      break;
    case V9:
      if(voltage > 8.5 && voltage < 9.5)
      {
        return true;
      }
      break;
    case V12:
      if(voltage > 11.5 && voltage < 12.5)
      {
        return true;
      }
      break;
    case V15:
      if(voltage > 14.5 && voltage < 15.5)
      {
        return true;
      }
      break;
    case V20:
      if(voltage > 19.0 && voltage < 20.5)
      {
        return true;
      }
      break;
  }
  return false;
}

void callback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");

  // parse 3 bytes into a number
  
  
  uint8_t number[3] = {0,0,0};
  if(payload[0] >= 0x30 && payload[0] <= 0x39 && payload[1] >= 0x30 && payload[1] <= 0x39 && payload[2] >= 0x30 && payload[2] <= 0x39)
  {
    number[0] = ((payload[0] - 0x30) * 100) + ((payload[1] - 0x30) * 10) + ((payload[2] - 0x30) * 1);
  }

  if(payload[3] >= 0x30 && payload[3] <= 0x39 && payload[4] >= 0x30 && payload[4] <= 0x39 && payload[5] >= 0x30 && payload[5] <= 0x39)
  {
    number[1] = ((payload[3] - 0x30) * 100) + ((payload[4] - 0x30) * 10) + ((payload[5] - 0x30) * 1);
  }

  if(payload[6] >= 0x30 && payload[6] <= 0x39 && payload[7] >= 0x30 && payload[7] <= 0x39 && payload[8] >= 0x30 && payload[8] <= 0x39)
  {
    number[2] = ((payload[6] - 0x30) * 100) + ((payload[7] - 0x30) * 10) + ((payload[8] - 0x30) * 1);
  }

  for (unsigned int i = 0; i < length; i++) {
      Serial.print((char) payload[i]);
  }
  
  Serial.println();
  Serial.println("-----------------------");
  Serial.println(number[0]);
  Serial.println(number[1]);
  Serial.println(number[2]);
  
  // convert  100 to 4096
  pwm_fan = map(number[0], 0, 100, 0, 255);
  Serial.println(pwm_fan);
}

void connectToClient(void)
{
  uint8_t retries = 50;
  while (!client.connected()) {
      String client_id = "USB-PD-FAN";
      client_id += String(WiFi.macAddress());
      Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
      if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
          Serial.println("MQTT broker connected");
      } else {
          Serial.print("failed with state ");
          Serial.print(client.state());
          delay(2000);
          retries--;
          Serial.print("  Retries left: ");
          Serial.println(retries);
          if(retries == 0)
          {
            ESP.restart();
          }
      }
  }
  client.subscribe(topic);
}
