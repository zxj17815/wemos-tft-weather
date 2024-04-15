#include <Arduino.h> // Include the necessary header file for the String class

// API key
const String API_KEY = "";

// Post code
const String POST_CODE = "330481";

// API URL
const String API_URL = "https://restapi.amap.com/v3/weather/weatherInfo?city=" + POST_CODE + "&key=" + API_KEY;
