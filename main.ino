#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// WiFi ağının SSID ve şifresi
const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

// MQTT broker adresi ve bağlantı noktası
const char* mqttServer = "MQTT_SERVER_IP";
const int mqttPort = 1883;

// MQTT konu adı ve röle pin numarası
const char* mqttTopic = "relay";
const int relayPin = D1;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(relayPin, OUTPUT);
  
  // Seri bağlantıyı başlat
  Serial.begin(115200);

  // WiFi ağına bağlan
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("WiFi bağlantısı kuruluyor...");
  }
  
  Serial.println("WiFi bağlantısı başarıyla kuruldu");

  // MQTT broker'a bağlan
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      Serial.println("MQTT broker'a bağlandı");
      client.subscribe(mqttTopic);
    } else {
      Serial.print("MQTT broker'a bağlantı başarısız, hata kodu: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  
  Serial.print("Konu: ");
  Serial.print(topic);
  Serial.print(", Mesaj: ");
  Serial.println(message);

  if (String(topic) == mqttTopic) {
    if (message == "on") {
      digitalWrite(relayPin, HIGH);
    } else if (message == "off") {
      digitalWrite(relayPin, LOW);
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ESP8266Client")) {
      Serial.println("MQTT broker'a bağlandı");
      client.subscribe(mqttTopic);
    } else {
      Serial.print("MQTT broker'a bağlantı başarısız, hata kodu: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}
