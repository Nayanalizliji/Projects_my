#include <esp_now.h>
#include <WiFi.h>

int int_value;
float float_value;
bool bool_value = true;

//MAC Address of responder
uint8_t broadcastAddress[] = {0x7C, 0x2C, 0x67, 0x64, 0xA1, 0x5C};

//Define Data structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

//Create a structured object
struct_message myData;

//Peer info
esp_now_peer_info_t peerInfo;

//Callback function called when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  // serial monitor setup
  Serial.begin(115200);

  //Set ESP32 as a WiFi Station
  WiFi.mode(WIFI_STA);
  
  //Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  //Register the send callback
  esp_now_register_send_cb(OnDataSent);

  //Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  //Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

}

void loop() {
  //Create test data

  //Generate a random integer
  int_value = random(1,20);

  //use integer to make a new float
  float_value = 1.3* int_value;

  //invert the boolean value
  bool_value = !bool_value;

  //Format structured data
  strcpy(myData.a, "Welcome!");
  myData.b = int_value;
  myData.c = float_value;
  myData.d = bool_value;

  //send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sending confirmed");
  }
  else {
    Serial.println("Sending error");
  }
  delay(2000);
}



