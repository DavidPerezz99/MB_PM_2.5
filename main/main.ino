#include <WiFi.h>
#include <HTTPClient.h>
#define R1  10000
#define C1  (float)1.009249522e-03
#define C2  (float)2.378405444e-04
#define C3  (float)2.019202697e-07
const char* ssid = "Tenda_0FD9A0";
const char* password = "J0qPhgD6";
const char* serverName = "https://script.google.com/macros/s/AKfycbzlS15IPWkJZm0HjCkzFlCpwR9eb7aRcgOYe7_qqp15gLm-2iYNxKEU61xUoEo2yiL5/exec";
float avg[3]={0,0,0};

const int samplesPerSecond = 20;
const int totalSamples = 20; // Numero de muestras en 1 segundo

float samples[totalSamples];
int sampleIndex = 0;


void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  
  Serial.println("Connected to WiFi");
}
float getTemperature(void)
{
  static int avgArrayIndex=0;
  int Vo;
  float logR2,R2, T, Tc;
  Vo = analogRead(35);
  R2 = R1 * (4096.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  T = (1.0 / (C1 + C2*logR2 + C3*logR2*logR2*logR2));
  Tc = T - 273.15;
  avg[avgArrayIndex++] = Tc;
  if(avgArrayIndex > 2)
  avgArrayIndex = 0;
  Tc = (avg[0] + avg[1] + avg[2])/3;
  return Tc;
}
void loop() {
  if (sampleIndex < totalSamples) {
    samples[sampleIndex] = getTemperature(); // Suponiendo que el sensor de temperatura está conectado al pin 34
    sampleIndex++;
    delay(1000 / samplesPerSecond); // Esperar para la siguiente muestra
  } else {
    sendSamples();
    sampleIndex = 0; // Reiniciar el índice para la siguiente ronda de muestras
  }
}

void sendSamples() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Inicia la solicitud HTTP
    http.begin(serverName);
    http.addHeader("Content-Type", "application/json");

    // Crea el payload JSON
    String jsonPayload = "{\"samples\":[";
    for (int i = 0; i < totalSamples; i++) {
      jsonPayload += String(samples[i]);
      if (i < totalSamples - 1) jsonPayload += ",";
    }
    jsonPayload += "]}";

    // Envía la solicitud POST
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println(httpResponseCode);
      Serial.println(response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    // Finaliza la conexión
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
