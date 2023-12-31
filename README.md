# Air quality controller

This is a project to make a IOT based cheap air quality controller using esp32, mq135 sensor and dht sensor. The esp32 uses the httpserver library to send data and recive commands from user devices. The controller can read sensor data and sends it to user devices and get threshhold values from user devices. It can control fan or other equipment to suck in fresh air if needed and expel polluted air through exhoust fans. 

The code is modular in the sense that the esp only sends a skeleton page to the user devices and and the user agent fetches the complex ui code consisting of javascript and css fron the net. This lets the user interface code be as large as needed. So features like machine learning and data storage and data anylysis can be done on the users machine keep the limited  resources of the esp32 for more sensors and io.

The httpserver library is flexible. So many new features can be added incrementally.


### Code structure
![ Code structure](img/CodeStructureDiagram.png)

PlatformIO and VSCode is used for code compilation and flashing. 

Here the esp only continuously sends the sensor data. The page skeleton code is tranferred once from esp after that the user device connects to cloud for other ui elements and further data processing.

#### Code for sensor configuration
 DHT11 configuration
 ```
#define DHTPIN 23
#define fanPin 22
// type of sensor in use:
#define DHTTYPE DHT11
```
 
MQ135 configuration
```
// MQ135 definitions
#define placa "ESP-32"
#define Voltage_Resolution 3.3
#define pin 34                 // Analog input 0 of your arduino
#define type "MQ-135"          // MQ135
#define ADC_Bit_Resolution 12  // For arduino UNO/MEGA/NANO
#define RatioMQ135CleanAir 3.6 // RS / R0 = 3.6 ppm
MQUnifiedsensor MQ135(placa, Voltage_Resolution, ADC_Bit_Resolution, 	pin, type);
```


### UI

 ![UI](img/UI.png)
 
In the UI the %*% is updated by esp. The icons and other ui elements are fetched from cloud server using AJAX calls. The UI lets the user set treshhold values for the sensors and shows current sensor values.
 
 
 
 ### Todo
 
- Add graphs to show value over time 
- Add a custom page to configure wifi connection dynamically
- Add data storage and data anylysis capability using cloud services


