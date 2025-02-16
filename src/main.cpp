#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "config.h"

#define DHTPIN 4
#define DHTTYPE DHT22

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
    Serial.begin(115200);
    dht.begin();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.print(".");
    }

    Serial.println("\nConectado a WiFi!");

    client.setServer(MQTT_SERVER, MQTT_PORT);
    while (!client.connected())
    {
        Serial.print("Conectando a MQTT...");
        if (client.connect("ESP32_DHT22"))
        {
            Serial.println("Conectado!");
        }
        else
        {
            Serial.print("Falló, rc=");
            Serial.print(client.state());
            Serial.println(" Intentando de nuevo...");
            delay(5000);
        }
    }
}

void loop()
{
    if (!client.connected())
    {
        client.connect("ESP32_DHT22");
    }

    float temperatura = dht.readTemperature();
    float humedad = dht.readHumidity();

    if (!isnan(temperatura) && !isnan(humedad))
    {
        String tempStr = String(temperatura);
        String humStr = String(humedad);

        client.publish("casa/salon/temperatura", tempStr.c_str());
        client.publish("casa/salon/humedad", humStr.c_str());

        Serial.println("Datos enviados a MQTT:");
        Serial.println("Temperatura: " + tempStr);
        Serial.println("Humedad: " + humStr);
    }
    else
    {
        Serial.println("Error al leer el sensor DHT!");
    }

    delay(5000);
}
