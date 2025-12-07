#include "esp8266RESTModule.h"

// Function to check if an API endpoint is alive
bool ESP8266RESTModule::checkAPIEndpoint(const String &url)
{
    DBGL("Checking API endpoint: %s", url.c_str());

    WiFiClient wifiClient;
    HTTPClient http;

    // Use the new API format with WiFiClient
    if (!http.begin(wifiClient, url))
    {
        DBGL("Failed to initialize HTTP connection");
        return false;
    }

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        DBGL("API endpoint response code: %d", httpResponseCode);
        String payload = http.getString();
        DBGL("Response: %s", payload.c_str());
        http.end();
        return true;
    }
    else
    {
        DBGL("API endpoint check failed, error: %d", httpResponseCode);
        http.end();
        return false;
    }
}

bool ESP8266RESTModule::postToAPIEndpoint(const String &url, const String &payload)
{
    DBGL("Posting to API endpoint: %s", url.c_str());

    WiFiClient wifiClient;
    HTTPClient http;

    // Use the new API format with WiFiClient
    if (!http.begin(wifiClient, url))
    {
        DBGL("Failed to initialize HTTP connection");
        return false;
    }

    http.addHeader("Content-Type", "application/json");

    // Send HTTP POST request
    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0)
    {
        DBGL("API endpoint POST response code: %d", httpResponseCode);
        String response = http.getString();
        DBGL("Response: %s", response.c_str());
        http.end();
        return true;
    }
    else
    {
        DBGL("API endpoint POST failed, error: %d", httpResponseCode);
        http.end();
        return false;
    }
}

bool ESP8266RESTModule::isAlive()
{
    return checkAPIEndpoint(apiEndpoint + "is-alive");
}

bool ESP8266RESTModule::updateTemperatureReading(const String componentId, const String &temperature)
{
    String url = apiEndpoint + "temperature-reading";
    String payload = "{\"componentId\":\"" + componentId + "\",\"temperature\":\"" + temperature + "\",\"timestamp\":\"null\"" + "}";
    return postToAPIEndpoint(url, payload);
}