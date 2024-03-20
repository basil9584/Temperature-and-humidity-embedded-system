#include <DHT.h>  // Include the DHT library to interface with DHT series humidity and temperature sensors

#define DHT_PIN 2         // Digital I/O pin number on the Arduino for connecting the DHT sensor
#define DHT_TYPE DHT11    // Specifies the type of DHT sensor used, DHT11, which is a basic, low-cost digital temperature and humidity sensor

DHT dht(DHT_PIN, DHT_TYPE); // Create a DHT sensor object with the specified pin and sensor type

#define R25 10000  // Nominal resistance at 25°C in ohms. Used as a reference point for temperature calculations.
#define BETA 3950  // Beta coefficient in Kelvin. A parameter that indicates the change in thermistor resistance with temperature change.
#define VCC 5      // Supply Voltage in volts. The operating voltage supplied to both the thermistor and the Arduino.
#define RS 10000   // Series Resistor value in ohms connected in series with the thermistor.

String msg; // String to hold the message that will be printed to the serial monitor.
String temp_text, hum_text, real_temp_text; // Strings to hold the text global values

// Variables for calculations
float Rt, Vr, Ln, Tc, Tk, Vrt, Tf; // Variables for thermistor resistance, voltage, natural logarithm, temperature in Celsius, Kelvin, and Fahrenheit.
float hum, temp; // Variables to store the humidity and temperature readings from the DHT sensor.

void setup() {
  Tk = 25 + 273.15; // Initial temperature in Kelvin. Converts 25°C to Kelvin as a reference starting point.
  Serial.begin(9600); // Begin serial communication at 9600 baud rate for outputting data to the serial monitor.
  dht.begin(); // Initialize the DHT sensor for temperature and humidity readings.
}

void loop() {
  // Thermistor calculations for determining temperature
  Vrt = analogRead(A0); // Read the analog voltage across the thermistor
  Vrt = (5.00 / 1023.00) * Vrt; // Convert that reading to voltage, assuming a 5V supply and 10-bit ADC resolution.
  Vr = VCC - Vrt; // Calculate the voltage across the series resistor.
  Rt = Vrt / (Vr / RS); // Calculate the thermistor resistance using the voltage divider formula.
  Ln = log(Rt / R25); // Calculate the natural logarithm of the resistance ratio for temperature conversion.
  Tc = (1 / ((Ln / BETA) + (1 / Tk))) - 273.15; // Convert the result to Celsius temperature.
  Tf = (Tc * 9.0 / 5.0) + 32; // Convert Celsius temperature to Fahrenheit.

  // Reading humidity and temperature from DHT sensor
  hum = dht.readHumidity(); // Read the humidity value from the DHT sensor.
  temp = dht.readTemperature(); // Read the temperature in Celsius from the DHT sensor.

  // Check if the sensor readings failed and print an error message
  if (isnan(hum) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!"); // Print error message if DHT sensor readings are not available.
  } else {
    // Determine the text global values based on temperature and humidity
    if (Tc < 21.0) { // If the temperature is below 21°C, set the temperature text to "Cold"
      temp_text = "Cold";
    } else if (Tc >= 21.0 && Tc < 24.0) { // If the temperature is between 21°C and 24°C, set the temperature text to "Normal"
      temp_text = "Normal";
    } else { // If the temperature is 24°C or above, set the temperature text to "Hot"
      temp_text = "Hot";
    }

    if (hum >= 70.0) { // If the humidity is 70% or above, set the humidity text to "High"
      hum_text = "High";
    } else if (hum >= 60.0 && hum < 70.0) { // If the humidity is between 60% and 70%, set the humidity text to "Normal"
      hum_text = "Normal";
    } else { // If the humidity is below 60%, set the humidity text to "Low"
      hum_text = "Low";
    }

    // Determine the real temperature text based on the actual temperature reading
    if (temp < 21.0) { // If the actual temperature is below 21°C, set the real temperature text to "Cold"
      real_temp_text = "Cold";
    } else if (temp >= 21.0 && temp < 24.0) { // If the actual temperature is between 21°C and 24°C, set the real temperature text to "Normal"
      real_temp_text = "Normal";
    } else { // If the actual temperature is 24°C or above, set the real temperature text to "Hot"
      real_temp_text = "Hot";
    }

    // Format and print the results to the serial monitor with labels and repeated global values
    msg = "Temperature: " + String(Tc, 1) + " °C, Temperature: " + String(Tf, 1) + " °F, Humidity: " + String(hum, 1) + "%, Actual temperature: " + String(temp, 1) + " °C, Actual temperature: " + String(temp * 9.0 / 5.0 + 32, 1) + " °F, " + temp_text + ", " + hum_text + ", " + real_temp_text + ", " + String(Tc, 1) + ", " + String(Tf, 1) + ", " + String(hum, 1) + ", " + String(temp, 1) + ", " + String(temp * 9.0 / 5.0 + 32, 1);
    Serial.println(msg); // Display the formatted string
  }

  delay(500); // Pause for half a second before the next loop iteration to allow for serial data to be sent and to not overload the sensors with constant readings.
}