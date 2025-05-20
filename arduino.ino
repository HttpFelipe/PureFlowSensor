#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Pinos dos sensores
const int pHSensorPin = A0;      // pH sensor (potenciômetro simulado)
const int oxygenSensorPin = A1;  // Oxigênio dissolvido (potenciômetro simulado)
const int tempSensorPin = A2;    // Temperatura (potenciômetro simulado)

const int valvePin = 8;          // Controle da válvula (LED simulado)
const int wifiLedPin = 9;        // LED indicador WiFi (simulado)

// Limites seguros para consumo humano
const float pH_min = 6.5;
const float pH_max = 8.0;
const float oxygen_min = 6.0;
const float oxygen_max = 14.0;  

const float temp_min = 0.0;
const float temp_max = 25.0;  // Limite seguro real (25°C)

// Limites máximos para simulação (potenciômetros simulam até esses valores)
const float pH_max_map = 10.0;       // Pode simular pH até 10
const float oxygen_max_map = 20.0;   // Pode simular O2 até 20 mg/L
const float temp_max_map = 40.0;     // Pode simular temperatura até 40°C

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  pinMode(valvePin, OUTPUT);
  pinMode(wifiLedPin, OUTPUT);
  
  digitalWrite(wifiLedPin, HIGH);  // LED WiFi ligado

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("PureFlow Sensor");
  delay(1500);
  lcd.clear();
}

float readSensor(int pin, float minVal, float maxVal) {
  int raw = analogRead(pin);
  float val = map(raw, 0, 1023, int(minVal * 10), int(maxVal * 10)) / 10.0;
  return val;
}

void displayMessage(bool waterGood, float pH, float oxygen, float temperature, const String &alertMsg) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("pH:");
  lcd.print(pH, 2);
  lcd.print(" O2:");
  lcd.print(oxygen, 1);

  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(temperature, 1);
  lcd.print("C ");
  
  if (waterGood) {
    lcd.print("OK");
  } else {
    lcd.print("NOK");
  }
  
  delay(1500);  // Tempo para ler essa tela

  if (alertMsg.length() > 0) {
    lcd.clear();
    lcd.setCursor(0,0);
    if (alertMsg.length() > 16) {
      lcd.print(alertMsg.substring(0,16));
      lcd.setCursor(0,1);
      lcd.print(alertMsg.substring(16));
    } else {
      lcd.print(alertMsg);
    }
    delay(2500);
  }
}

void loop() {
  float pH = readSensor(pHSensorPin, pH_min, pH_max_map);
  float oxygen = readSensor(oxygenSensorPin, oxygen_min, oxygen_max_map);
  float temperature = readSensor(tempSensorPin, temp_min, temp_max_map);

  Serial.print(pH, 2);
  Serial.print(",");
  Serial.print(oxygen, 2);
  Serial.print(",");
  Serial.println(temperature, 2);

  bool waterGood = true;
  String alertMsg = "";

  if (pH < pH_min || pH > pH_max) {
    Serial.println("pH fora do intervalo seguro (6.5-8.0)!");
    alertMsg += "pH fora limite ";
    waterGood = false;
  }
  if (oxygen < oxygen_min) {
    Serial.println("Oxigênio dissolvido abaixo do mínimo seguro (6.0 mg/L)!");
    if (alertMsg.length() > 0) alertMsg += " ";
    alertMsg += "O2 baixo";
    waterGood = false;
  }
  if (temperature > temp_max) {
    Serial.println("Temperatura acima do máximo seguro (25 °C)!");
    if (alertMsg.length() > 0) alertMsg += " ";
    alertMsg += "Temp alta";
    waterGood = false;
  }

  if (waterGood) {
    Serial.println("Água adequada para consumo humano.");
    digitalWrite(valvePin, HIGH);  // Abre válvula (liga LED)
    alertMsg = "Agua OK";
  } else {
    Serial.println("Água NÃO adequada, válvula fechada.");
    digitalWrite(valvePin, LOW);   // Fecha válvula (desliga LED)
  }

  displayMessage(waterGood, pH, oxygen, temperature, alertMsg);

  Serial.println("-----------------------");
  delay(2000);
}
