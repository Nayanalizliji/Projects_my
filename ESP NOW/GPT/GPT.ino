#include <esp_now.h>
#include <WiFi.h>

// Define Data Structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  bool d;
} struct_message;

// Create a structured object
struct_message myData;

const int LED_PIN = LED_BUILTIN;  // ✅ Use LED_BUILTIN since it's working

// Callback function when data is received
void OnDataRecv(const esp_now_recv_info_t *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));

  Serial.print("Received myData.d: ");
  Serial.println(myData.d);

  // ✅ Update LED state immediately when data is received
  if (myData.d) {
    Serial.println("Turning LED ON");
    digitalWrite(LED_PIN, HIGH);  // ✅ HIGH = ON
  } else {
    Serial.println("Turning LED OFF");
    digitalWrite(LED_PIN, LOW);   // ✅ LOW = OFF
  }
}

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_PIN, OUTPUT);  // ✅ Set LED as OUTPUT
  digitalWrite(LED_PIN, LOW); // ✅ Start with LED OFF

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // ✅ Check if myData.d is changing
  Serial.print("myData.d NOW: ");
  Serial.println(myData.d);

  if (myData.d) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  } else {
    digitalWrite(LED_PIN, LOW);  // Keep LED OFF
  }
}
