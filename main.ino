#include <WiFiNINA.h>
// Am i in test-branch?

char ssid[] = "SSID of WiFi";	   //  your network SSID (name) between the " "
char pass[] = "Password for WiFi"; // your network password between the " "

int status = WL_IDLE_STATUS; // connection status

WiFiServer server(80); // server socket
WiFiClient client = server.available();

void setup()
{
	Serial.begin(9600);
	while (!Serial)
	{
	};

	enable_WiFi();
	connect_WiFi();

	server.begin();
	printWifiStatus();
}

void loop()
{
	client = server.available();

	if (client)
	{
		printWEB();
	}
}

void printWifiStatus()
{
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your board's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");

	Serial.print("To see this page in action, open a browser to http://");
	Serial.println(ip);
}

void enable_WiFi()
{
	// check for the WiFi module:
	if (WiFi.status() == WL_NO_MODULE)
	{
		Serial.println("Communication with WiFi module failed!");
		// don't continue
		while (true)
		{
		};
	}

	String fv = WiFi.firmwareVersion();
	if (fv < "1.0.0")
	{
		Serial.println("Please upgrade the firmware");
	}
}

void connect_WiFi()
{
	// attempt to connect to Wifi network:
	while (status != WL_CONNECTED)
	{
		Serial.print("Attempting to connect to SSID: ");
		Serial.println(ssid);
		// Connect to WPA/WPA2 network. Change this line if using open or WEP network:
		status = WiFi.begin(ssid, pass);

		// wait 10 seconds for connection:
		delay(2500);
	}
}

void printWEB()
{

	if (client)
	{								  // if you get a client,
		Serial.println("new client"); // print a message out the serial port
		String currentLine = "";	  // make a String to hold incoming data from the client
		bool toTemp = false;
		while (client.connected())
		{ // loop while the client's connected
			if (client.available())
			{							// if there's bytes to read from the client,
				char c = client.read(); // read a byte, then
				Serial.write(c);		// print it out the serial monitor
				if (c == '\n')
				{ // if the byte is a newline character

					// if the current line is blank, you got two newline characters in a row.
					// that's the end of the client HTTP request, so send a response:
					if (currentLine == "GET /test HTTP/1.1")
					{
						toTemp = true;
					}
					else if (currentLine.length() == 0)
					{

						// HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
						// and a content-type so the client knows what's coming, then a blank line:
						if (toTemp)
						{
							client.println("HTTP/1.1 200 OK");
							client.println("Content-type:application/json");
							client.println();
							int a = analogRead(A0);

							float R = 1023.0 / ((float)a) - 1.0;
							R = 100000.0 * R;

							float temperature = 1.0 / (log(R / 100000.0) / 4275 + 1 / 298.15) - 273.15;
							client.print("{\"data\":{\"temp\": ");
							client.print(temperature);
							client.print("}}");
							client.println();
							// break out of the while loop:
							break;
						}
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println();
						client.print("<p>Temperature:</p><p id=\"test\">Loading...</p>");
						client.print("<p>Counter:</p><p id=\"testy\">1</p>");

						client.print(String("<script>setInterval(()=>{fetch(") + String(WiFi.localIP()) + String("/test\",{mode: \"same-origin\", referrerPolicy: \"origin\"}).then((r) => {return r.text()}).then((text)=>{document.querySelector(\"#test\").innerHTML = JSON.parse(text)[\"data\"][\"temp\"]})},10000)</script>"));
						client.print(String("<script>setInterval(()=>{document.querySelector(\"#testy\").innerHTML = parseInt(document.querySelector(\"#testy\").innerHTML) + 1},10000)</script>"));
						client.println();
						// break out of the while loop:
						break;
					}
					else
					{ // if you got a newline, then clear currentLine:
						currentLine = "";
					}
				}
				else if (c != '\r')
				{					  // if you got anything else but a carriage return character,
					currentLine += c; // add it to the end of the currentLine
				}
			}
		}
		// close the connection:
		client.stop();
		Serial.println("client disconnected");
	}
}