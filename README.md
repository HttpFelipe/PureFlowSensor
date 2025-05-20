# 💧 PureFlow Sensor

Sistema IoT para monitoramento da qualidade da água utilizando sensores de pH, oxigênio dissolvido e temperatura, com envio dos dados via protocolo MQTT. O sistema permite controle remoto de uma válvula com base na análise dos dados coletados, visando garantir que a água esteja dentro dos padrões seguros para consumo humano.

---

## 📌 1) Descrição do Funcionamento e Uso

O projeto consiste em duas partes:

- Arduino Uno: coleta os dados dos sensores e envia via comunicação serial.
- ESP32: recebe os dados via Serial2 e os publica em um broker MQTT público (test.mosquitto.org).

O sistema interpreta os dados e, caso estejam fora dos limites seguros, aciona um atuador (válvula simulada com LED) para impedir o fluxo da água. Um display LCD I2C exibe os valores lidos e o status da água (OK/NOK). A comunicação MQTT permite o monitoramento remoto dos dados por uma plataforma na internet.

Para reproduzir:

1. Conecte os sensores simulados (potenciômetros) aos pinos analógicos do Arduino.
2. Conecte o Arduino ao ESP32 via Serial (pinos 16 e 17 do ESP32).
3. Configure o WiFi no ESP32.
4. Observe os dados no monitor serial e em um cliente MQTT.

---

## 📄 2) Documentação do Código

### Arduino (Sensores + Atuador)

- Lê sensores analógicos: pH (A0), oxigênio (A1), temperatura (A2).
- Faz mapeamento dos valores simulados para escalas reais.
- Verifica se os valores estão dentro dos limites seguros.
- Aciona válvula (LED no pino 8) se a água for adequada.
- Exibe os dados no display LCD 16x2 (I2C).
- Envia as leituras via serial no formato CSV: `pH,oxygen,temperature`.

### ESP32 (MQTT + Comunicação Serial)

- Conecta-se à rede WiFi (Wokwi-GUEST).
- Recebe os dados do Arduino via Serial2 (pinos 16 e 17).
- Faz o parsing dos dados e publica no tópico MQTT `pureflow/sensor` no broker test.mosquitto.org.
- Também imprime os dados no Serial Monitor para debug.

---

## 🔧 3) Hardware Utilizado

| Componente                  | Descrição                                        |
|----------------------------|--------------------------------------------------|
| Arduino Uno                | Leitura dos sensores e controle da válvula       |
| ESP32                      | Conectividade WiFi e envio MQTT                  |
| Potenciômetros             | Simulam sensores analógicos (pH, O2, temperatura)|
| Display LCD 16x2 (I2C)     | Exibição local dos dados                         |
| LED                        | Simula a válvula de água                         |
| Conexão Serial (TX/RX)     | Comunicação Arduino ↔ ESP32                      |
| Protoboard + Jumpers       | Montagem e interligação                          |

Obs: Não foram utilizadas peças impressas em 3D. A montagem é feita em protoboard, mas pode ser adaptada para caixas físicas (ex: caixa 10x10x5 cm para comportar Arduino, ESP32, sensores e display).

---

## 🌐 4) Interfaces, Protocolos e Módulos de Comunicação

| Recurso               | Descrição                                              |
|-----------------------|--------------------------------------------------------|
| UART Serial           | Comunicação entre Arduino e ESP32                      |
| I2C                   | Comunicação com o display LCD                          |
| MQTT                  | Protocolo para envio de dados sensor → broker MQTT     |
| Wi-Fi (TCP/IP)        | Conexão do ESP32 à internet para envio MQTT            |

O ESP32 utiliza a biblioteca PubSubClient para realizar a conexão com o broker MQTT via TCP/IP. Os dados são enviados no formato JSON, o que permite integração fácil com plataformas como Node-RED, Home Assistant ou dashboards personalizados.

---

## ☁️ 5) Comunicação/Controle via Internet (TCP/IP + MQTT)

O sistema utiliza a rede Wi-Fi para conectar o ESP32 à internet. As mensagens dos sensores são publicadas no tópico MQTT:

- Broker: `test.mosquitto.org`
- Tópico: `pureflow/sensor`
- Payload:  
  ```json
  {
    "pH": 7.25,
    "oxygen": 8.3,
    "temperature": 22.1
  }
  ```

Isso permite que qualquer plataforma que suporte MQTT (como um cliente MQTT, Node-RED ou app móvel) possa receber os dados em tempo real, permitindo ações remotas e geração de relatórios.
