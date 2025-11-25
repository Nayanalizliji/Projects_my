#include <esp_now.h>
#include <WiFi.h>

//Define Data structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

//Create a structured object
struct_message myData;

const int LED_PIN = LED_BUILTIN;

//Callback function called when data is sent
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.print("Received data from: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", recv_info->src_addr[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
  Serial.print("Data received: ");
  Serial.println(len);
  Serial.print("Character value: ");
  Serial.println(myData.a);
  Serial.print("Integer value: ");
  Serial.println(myData.b);
  Serial.print("Float value: ");
  Serial.println(myData.c);
  Serial.print("Boolean value: ");
  Serial.println(myData.d);
  Serial.println();

  //  Update LED state immediately when data is received
  if (myData.d) {
    Serial.println("Turning LED ON");
    digitalWrite(LED_PIN, LOW);  //  LOW = ON
  } else {
    Serial.println("Turning LED OFF");
    digitalWrite(LED_PIN, HIGH);   //  HIGH = OFF
  }

  
}



void setup() {
  // serial monitor setup
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);  //  Set LED as OUTPUT
  digitalWrite(LED_PIN, HIGH); //  Start with LED OFF
  

  //Set ESP32 as a WiFi Station
  WiFi.mode(WIFI_STA);
  
  //Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  //Register callback fn
  esp_now_register_recv_cb(OnDataRecv);

}

void loop() {
  
}


