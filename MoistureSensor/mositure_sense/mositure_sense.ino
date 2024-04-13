#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// #define DEBUG_MODE
#define ANALOG_PIN (A0)
#define CHANNEL_ID (2499687)
#define SAMPLE_COUNT (10U)
#define MAX_RETRY_COUNT (10U)
#define WIFI_COUNT (3U)
const char* ssids[WIFI_COUNT] = { "MANAYIL NET NALUPLACKAL", "MANAYIL NET NALUPLACKAL EXT 2.4G", "MANAYIL NET NALUPLACKAL_5G" };
const char* passwords[WIFI_COUNT] = { "9447806258", "9447806258", "9447806258" };
const char* thingSpeakApiKey = "XEXO1892AAEQ53QD";

int connectWiFi(const char* ssid, const char* password);
void debugInit();
void debugPrint(String message);

WiFiClient client;

void setup() {
  int val, sum;
  float avg;

  debugInit();

  sum = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    val = analogRead(ANALOG_PIN);
    sum += val;
    debugPrint("Sensor Value: " + String(val));
    delay(1000U);
  }
  avg = (float)sum / SAMPLE_COUNT;
  debugPrint("Average Sensor Value: " + String(avg));

  for (int i = 0; i < WIFI_COUNT; i++) {
    if (connectWiFi(ssids[i], passwords[i]) == 0) {
      debugPrint("WiFi connected");
      break;
    } else {
      debugPrint("WiFi not connected");
    }
  }

  ThingSpeak.begin(client);
  if (ThingSpeak.writeField(CHANNEL_ID, 1, avg, thingSpeakApiKey) == 200) {
    debugPrint("ThingSpeak write successful");
  } else {
    debugPrint("ThingSpeak write failed");
  }

  ESP.deepSleep(ESP.deepSleepMax()); /* Wait for max deep sleep time (~ 3.5 hours) */
}

void loop() {
}

int connectWiFi(const char* ssid, const char* password) {
  int retryCnt = 0U;

  debugPrint("Connecting to " + String(ssid));
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    debugPrint(".");
    if (++retryCnt > MAX_RETRY_COUNT) {
      break;
    }
  }

  if (WiFi.status() == WL_CONNECTED) {
    return 0;
  } else {
    return -1;
  }
}

#ifdef DEBUG_MODE
void debugInit() {
  Serial.begin(115200);
}
void debugPrint(String message) {
  Serial.println(message);
}
#else
inline void debugInit() {
}
inline void debugPrint(String message) {
}
#endif
