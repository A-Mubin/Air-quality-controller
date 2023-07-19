#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <MQUnifiedsensor.h>

// global variables
// Replace with your network credentials
const char *ssid = "DESKTOP-hotspot";
const char *password = "mubinwifi";

// Set your Static IP address
IPAddress local_IP(192, 168, 137, 98);
// Set your Gateway IP address
IPAddress gateway(192, 168, 137, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

#define DHTPIN 23 // Digital pin connected to the DHT sensor
#define fanPin 22
// Uncomment the type of sensor in use:
#define DHTTYPE DHT11 // DHT 11
// #define DHTTYPE DHT22 // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)


DHT dht(DHTPIN, DHTTYPE);

// MQ135 definitions
#define placa "ESP-32"
#define Voltage_Resolution 3.3
#define pin 34                 // Analog input 0 of your arduino
#define type "MQ-135"          // MQ135
#define ADC_Bit_Resolution 12  // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6 // RS / R0 = 3.6 ppm
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, pin, type);

// Variables
float CO2 = 0, t = 0, h = 0, threshT = 30, threshH = 85, threshCO2 = 415;
long int currentMillis;
String fanStatus = "OFF";
int interval = 1000;
// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Webpage html code
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css"
		integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous" />
	<link rel="stylesheet" href="https://github.com/A-Mubin/Air-quality-controller/blob/main/src/style.css" crossorigin="anonymous" />
	<title>Esp Server</title>
</head>
<body>
	Loading
</body>
<script src="https://github.com/A-Mubin/Air-quality-controller/blob/main/src/script.js" crossorigin="anonymous">
</script>
</html>
)rawliteral";

String readSensorData()
{
  String sensorData = String(t) + " " + String(h) + " " + String(CO2) + " " + fanStatus + " " + String(threshT) + " " + String(threshH) + " " + String(threshCO2);
  Serial.println("sending sensor data");
  return sensorData;
}

// read DHT senssor
void readSensors()
{
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  // float t = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  h = dht.readHumidity();

  MQ135.update();                 // Update data, the arduino will read the voltage from the analog pin
  CO2 = MQ135.readSensor() + 400; // Sensor will read PPM concentration using the model, a and b values set previously or from the setup
  if (isnan(t) || isnan(h))
  {
    Serial.println("Failed to read from DHT sensor!");
  }
  else
  {
    Serial.print("Humidity: ");
    Serial.print(h);
    Serial.print("\tTemperature: ");
    Serial.print(t);
  }
  if (isnan(CO2))
  {
    Serial.println("Failed to read from MQ135 sensor!");
  }
  else
  {
    Serial.print("\tAir Quality(CO2): ");
    Serial.print(CO2);
    Serial.println("ppm");
  }
}

// Replaces placeholder with DHT values
// String processor(const String &var)
// {
//   // Serial.println(var);
//   if (var == "TEMPERATURE")
//   {
//     return String(t);
//   }
//   else if (var == "HUMIDITY")
//   {
//     return String(h);
//   }
//   else if (var == "AQI")
//   {
//     return String(CO2);
//   }
//   else if (var == "FANSTATUS")
//   {
//     return fanStatus;
//   }
//   else if (var == "THRESHTEMP")
//   {
//     return String(threshT);
//   }
//   else if (var == "THRESHHUMIDITY")
//   {
//     return String(threshH);
//   }
//   else if (var == "THRESHCO")
//   {
//     return String(threshCO2);
//   }
//   Serial.println("Processed template");
//   return String();
// }

void setup()
{
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(fanPin, OUTPUT);
  dht.begin();

  MQ135.setRegressionMethod(1); //_PPM =  a*ratio^b
  MQ135.init();
  MQ135.setRL(1);
  MQ135.setA(110.47);
  MQ135.setB(-2.862); // Configure the equation to calculate CO2 concentration value
 
 

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
 

  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();


  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); }); //, processor
  server.on("/Data", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/plain", readSensorData().c_str());
            Serial.println("Sending sensor Data"); });
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
              Serial.print("Processing /get request");
              if (request->hasParam("threshT") && request->getParam("threshT")->value()!="")
              {
                Serial.print("\thas threshT");
                threshT = request->getParam("threshT")->value().toFloat();
              }
              // GET input2 value on <ESP_IP>/get?input2=<inputMessage>
              if (request->hasParam("threshH") && request->getParam("threshH")->value() != "")
              {
                Serial.print("\thas threshH");
                threshH = request->getParam("threshH")->value().toFloat();
              }
              // GET input3 value on <ESP_IP>/get?input3=<inputMessage>
              if (request->hasParam("threshCO2") && request->getParam("threshCO2")->value() != "")
              {
                Serial.print("\thas threshCO2");
                threshCO2 = request->getParam("threshCO2")->value().toFloat();
              }
              Serial.print("\tNew threshT : " + String(threshT) + "\tthreshH : " + String(threshH) + "\tthreshCO2 : " + String(threshCO2) + "\n");
              request->send(200, "text/html", "Succesfully set threshhold values"); });

  // Start server
  server.begin();
  currentMillis = millis();

  // MQ135 calibrate
  Serial.print("Calibrating please wait.");
  float calcR0 = 0;
  for (int i = 1; i <= 10; i++)
  {
    MQ135.update(); // Update data, the arduino will read the voltage from the analog pin
    calcR0 += MQ135.calibrate(RatioMQ135CleanAir);
    Serial.print(".");
  }
  MQ135.setR0(calcR0 / 10);
  Serial.println("  done!.");

  if (isinf(calcR0))
  {
    Serial.println("Warning: Conection issue, R0 is infinite (Open circuit detected) please check your wiring and supply");
    while (1)
      ;
  }
  if (calcR0 == 0)
  {
    Serial.println("Warning: Conection issue found, R0 is zero (Analog pin shorts to ground) please check your wiring and supply");
    while (1)
      ;
  }
}

void loop()
{
  if (millis() - currentMillis > interval)
  {
    readSensors();

    if (threshT > t && threshH > h && threshCO2 > CO2)
    {
      if (fanStatus == "ON")
      {
        digitalWrite(fanPin, LOW);
        fanStatus = "OFF";
      }
    }
    else
    {
      if (fanStatus == "OFF")
      {
        digitalWrite(fanPin, HIGH);
        fanStatus = "ON";
      }
    }

    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());
    currentMillis = millis();
  }
}
