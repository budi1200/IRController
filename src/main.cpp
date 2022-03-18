#include <Arduino.h>
#include <Dictionary.h>
#include <IRremote.hpp>
#include <DFRobot_WiFi_IoT_Module.h>
#include <variables.h>

IRsend irsend;

// I2C mode
DFRobot_WiFi_IoT_Module_I2C IoT;

// MQTT message storage
char *data = (char *) "0";
// Message Prefix
const char *ir = "IR_";

// Callback function for MQTT messages
void callback(const char *topic, const char *message) {
    Serial.println("Receive [ " + (String) topic + " ], " + "Raw Message : " + (String) message);

    // Cut message at specified char to avoid garbage
    char *tmp = strdup(message);
    char *pch;
    pch = strchr(tmp, (char) 3);

    if (pch != nullptr) {
        int pos = (int) (pch - tmp);
        tmp[pos] = '\0';
        // Serial.println("New message: " + (String) tmp);
    }

    strcpy(data, tmp);
}

// WIFI Info
char *WIFI_SSID = (char *) ENV_WIFI_SSID
char *WIFI_PASSWORD = (char *) ENV_WIFI_PASSWORD

// MQTT configuration
char *MQTT_SERVER = (char *) ENV_MQTT_SERVER
char *MQTT_PORT = (char *) ENV_MQTT_PORT
char *MQTT_USERNAME = (char *) ENV_MQTT_USERNAME
char *MQTT_PASSWORD = (char *) ENV_MQTT_PASSWORD
char *MQTT_SUBSCRIBE_TOPIC = (char *) ENV_MQTT_SUBSCRIBE_TOPIC

// Dictionary for IR codes
Dictionary &dict = *(new Dictionary(44));

void setup() {
    // Setup IR Codes
    dict("IR_BPlus", 0xFF3AC5);
    dict("IR_BMinus", 0xFFBA45);
    dict("IR_ON", 0xFF827D);
    dict("IR_OFF", 0xFF02FD);
    dict("IR_R", 0xFF1AE5);
    dict("IR_G", 0xFF9A65);
    dict("IR_B", 0xFFA25D);
    dict("IR_W", 0xFF22DD);
    dict("IR_B1", 0xFF2AD5);
    dict("IR_B2", 0xFFAA55);
    dict("IR_B3", 0xFF926D);
    dict("IR_B4", 0xFF12ED);
    dict("IR_B5", 0xFF0AF5);
    dict("IR_B6", 0xFF8A75);
    dict("IR_B7", 0xFFB24D);
    dict("IR_B8", 0xFF32CD);
    dict("IR_B9", 0xFF38C7);
    dict("IR_B10", 0xFFB847);
    dict("IR_B11", 0xFF7887);
    dict("IR_B12", 0xFFF807);
    dict("IR_B13", 0xFF18E7);
    dict("IR_B14", 0xFF9867);
    dict("IR_B15", 0xFF58A7);
    dict("IR_B16", 0xFFD827);
    dict("IR_UPR", 0xFF28D7);
    dict("IR_UPG", 0xFFA857);
    dict("IR_UPB", 0xFF6897);
    dict("IR_QUICK", 0xFFE817);
    dict("IR_DOWNR", 0xFF08F7);
    dict("IR_DOWNG", 0xFF8877);
    dict("IR_DOWNB", 0xFF48B7);
    dict("IR_SLOW", 0xFFC837);
    dict("IR_DIY1", 0xFF30CF);
    dict("IR_DIY2", 0xFFB04F);
    dict("IR_DIY3", 0xFF708F);
    dict("IR_AUTO", 0xFFF00F);
    dict("IR_DIY4", 0xFF10EF);
    dict("IR_DIY5", 0xFF906F);
    dict("IR_DIY6", 0xFF50AF);
    dict("IR_FLASH", 0xFFD02F);
    dict("IR_JUMP3", 0xFF20DF);
    dict("IR_JUMP7", 0xFFA05F);
    dict("IR_FADE3", 0xFF609F);
    dict("IR_FADE7", 0xFFE01F);

    irsend.begin(3);
    Serial.begin(115200);

    // Init communication port
    while (IoT.begin() != 0) {
        Serial.println("IoT Init Error!");
        delay(100);
    }
    Serial.println("\nIoT Init Success");

    // Connect to WiFi
    while (IoT.connectWifi(WIFI_SSID, WIFI_PASSWORD) != 0) {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\nWiFi Connect Success");

    // Init MQTT
    while (IoT.MQTTBegin(MQTT_SERVER, MQTT_PORT, MQTT_USERNAME, MQTT_PASSWORD) != 0) {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\nMQTT Connect Success");

    // Setup callback function
    IoT.setCallBack(callback);

    // Subscribe to MQTT_SUBSCRIBE_TOPIC
    while (IoT.subscribe(MQTT_SUBSCRIBE_TOPIC) != 0) {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\nSubscribe Topic Success");
}

void loop() {
    IoT.loop();

    char buf[1000];
    strcpy(buf, ir);
    strcat(buf, data);

    if (dict(buf)) {
        Serial.println("Sending...");
//        irsend.sendNEC(dict[buf].toInt(), 32);
        irsend.sendNECMSB(dict[buf].toInt(), 32);
        strcpy(data, "null");
    }

    delay(200);
}