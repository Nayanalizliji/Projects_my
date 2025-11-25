#include <WiFi.h>
#include <esp_now.h>

// Define global state
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};  // Broadcast to all ESP-NOW devices

// Function to format MAC Address
void formatMacAddress(const uint8_t *macAddr, char *buffer, int maxLength) {
  snprintf(buffer, maxLength, "%02x:%02x:%02x:%02x:%02x:%02x", 
           macAddr[0], macAddr[1], macAddr[2], macAddr[3], macAddr[4], macAddr[5]);
}

// Function to randomly choose "Rock", "Paper", or "Scissors"
String chooseRandomOption() {
  int index = random(0, 3);
  if (index == 0) return "Rock";
  else if (index == 1) return "Paper";
  else return "Scissors";
}

// Callback: When Data is Received
void receiveCallback(const esp_now_recv_info_t *recv_info, const uint8_t *data, int dataLen) {
  char buffer[ESP_NOW_MAX_DATA_LEN + 1];
  int msgLen = min(ESP_NOW_MAX_DATA_LEN, dataLen);
  strncpy(buffer, (const char *)data, msgLen);
  buffer[msgLen] = 0;  // Null terminate

  // Format MAC Address
  char macStr[18];
  formatMacAddress(recv_info->src_addr, macStr, sizeof(macStr));

  Serial.printf("Received from %s: %s\n", macStr, buffer);

  if (String(buffer) == "START") {
    String choice = chooseRandomOption();
    Serial.printf("Chosen: %s\n", choice.c_str());
  }
}

// Callback: When Data is Sent
void sentCallback(const uint8_t *macAddr, esp_now_send_status_t status) {
  char macStr[18];
  formatMacAddress(macAddr, macStr, sizeof(macStr));
  Serial.printf("Last Packet Sent to: %s - Status: %s\n", macStr, status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// Function to Broadcast a Message
void broadcast(const String &message) {
  esp_err_t result = esp_now_send(broadcastAddress, (const uint8_t *)message.c_str(), message.length());

  if (result == ESP_OK) {
    Serial.printf("Broadcast Sent: %s\n", message.c_str());
  } else {
    Serial.printf("Broadcast Failed (Error Code: %d)\n", result);
  }
}

void setup() {
  Serial.begin(57600);
  WiFi.mode(WIFI_STA);
  Serial.println("ESP-NOW Broadcast with Serial & Button");

  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  WiFi.disconnect();
  randomSeed(analogRead(0));

  if (esp_now_init() == ESP_OK) {
    Serial.println("ESP_NOW Init Success");
    esp_now_register_recv_cb(receiveCallback);
    esp_now_register_send_cb(sentCallback);

    // Register Broadcast Peer
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if (esp_now_add_peer(&peerInfo) == ESP_OK) {
      Serial.println("Broadcast Peer Added");
    } else {
      Serial.println("Failed to Add Broadcast Peer");
    }
  } else {
    Serial.println("ESP-NOW Init Failed");
    delay(3000);
    ESP.restart();
  }
}

void loop() {
  if (Serial.available()) {
    String lastMessage = Serial.readStringUntil('\n');
    lastMessage.trim();

    if (lastMessage.length() > 0) {
      broadcast(lastMessage);
    }
  }
}
