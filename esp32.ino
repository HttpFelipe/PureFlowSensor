#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqtt_server = "test.mosquitto.org";

WiFiClient espClient;
PubSubClient client(espClient);

String inputString = "";
bool stringComplete = false;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando ao WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println();
  Serial.println("WiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado!");
      client.subscribe("pureflow/sensor");
    } else {
      Serial.print("Falha, rc=");
      Serial.print(client.state());
      Serial.println(" tentando novamente em 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial2.begin(9600, SERIAL_8N1, 16, 17);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  while (Serial2.available()) {
    char inChar = (char)Serial2.read();
    if (inChar == '\r') continue;
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }

  if (stringComplete) {
    Serial.println("Recebido: [" + inputString + "]");

    int firstComma = inputString.indexOf(',');
    int secondComma = inputString.indexOf(',', firstComma + 1);

    if (firstComma > 0 && secondComma > firstComma) {
      String pH = inputString.substring(0, firstComma);
      String oxygen = inputString.substring(firstComma + 1, secondComma);
      String temp = inputString.substring(secondComma + 1);

      String payload = "{";
      payload += "\"pH\":" + pH + ",";
      payload += "\"oxygen\":" + oxygen + ",";
      payload += "\"temperature\":" + temp;
      payload += "}";

      if (client.publish("pureflow/sensor", payload.c_str())) {
        Serial.println("Publicado no MQTT: " + payload);
      } else {
        Serial.println("❌ Falha ao publicar no MQTT");
        if (!client.connected()) {
          Serial.println("MQTT não está conectado.");
        }
      }
    } else {
      Serial.println("⚠️ Formato inválido da string recebida");
    }

    inputString = "";
    stringComplete = false;
  }
}
