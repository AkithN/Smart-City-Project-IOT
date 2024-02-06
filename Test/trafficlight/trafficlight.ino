/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid = "Akith";
const char* password = "akith12345";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String redState = "off";
String yellowState = "off";
String greenState = "off";

// Assign output variables to GPIO pins
const int redLED = 26;
const int yellowLED = 27;
const int greenLED = 14;  // Change to the desired GPIO pin

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0; 
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

// Delay between color changes
const unsigned long colorChangeDelay = 3000;
unsigned long colorChangeTime = 0;

void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  // Set outputs to LOW
  digitalWrite(redLED, LOW);
  digitalWrite(yellowLED, LOW);
  digitalWrite(greenLED, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  // Handle web server
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off button 
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Traffic Light Control</h1>");
            
            // Display the single button to control the entire traffic light
            client.println("<p><a href=\"/toggle\"><button class=\"button\">Toggle Traffic Light</button></a></p>");
            
            client.println("</body></html>");
            
            // Process control requests
            if (header.indexOf("GET /toggle") >= 0) {
              // Toggle the traffic light states
              if (redState == "on") {
                redState = "off";
                yellowState = "on";
                greenState = "off";
              } else if (yellowState == "on") {
                redState = "off";
                yellowState = "off";
                greenState = "on";
              } else if (greenState == "on") {
                redState = "on";
                yellowState = "off";
                greenState = "off";
              }
              // Update the traffic light based on the states
              digitalWrite(redLED, redState == "on" ? HIGH : LOW);
              digitalWrite(yellowLED, yellowState == "on" ? HIGH : LOW);
              digitalWrite(greenLED, greenState == "on" ? HIGH : LOW);
              // Record the time of the color change
              colorChangeTime = millis();
            }
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }

    // Perform color changes with delay
    unsigned long elapsedTime = millis() - colorChangeTime;
    if (elapsedTime >= colorChangeDelay) {
      // Perform the next color change
      if (redState == "on") {
        redState = "off";
        yellowState = "on";
        greenState = "off";
      } else if (yellowState == "on") {
        redState = "off";
        yellowState = "off";
        greenState = "on";
      } else if (greenState == "on") {
        redState = "on";
        yellowState = "off";
        greenState = "off";
      }
      // Update the traffic light based on the states
      digitalWrite(redLED, redState == "on" ? HIGH : LOW);
      digitalWrite(yellowLED, yellowState == "on" ? HIGH : LOW);
      digitalWrite(greenLED, greenState == "on" ? HIGH : LOW);
      // Record the time of the color change
      colorChangeTime = millis();
    }

    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
