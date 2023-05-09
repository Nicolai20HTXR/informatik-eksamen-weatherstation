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
float readTemp()
{
	int a = analogRead(A0);
	float R = 1023.0 / ((float)a) - 1.0;
	R = 100000.0 * R;
	float temperature = 1.0 / (log(R / 100000.0) / 4275 + 1 / 298.15) - 273.15;

	return temperature;
}

void printWEB()
{

	if (client)
	{								  // if you get a client,
		Serial.println("new client"); // print a message out the serial port
		String clientRequest = "";	  // make a String to hold incoming data from the client
		bool toTemp = false;
		while (client.connected())
		{ // loop while the client's connected
			if (client.available())
			{							// if there's bytes to read from the client,
				char c = client.read(); // read a byte, then
				clientRequest += c;
				if (clientRequest.endsWith("\r\n\r\n"))
				{
					break;
				}
			}
		}
		if (clientRequest.startsWith("GET /temperature HTTP/1.1"))
		{
			String sTemperature = "";
			sTemperature += "{\"data\":{\"temp\": ";
			sTemperature += String(readTemp());
			sTemperature += "}}";

			client.println("HTTP/1.1 200 OK");
			client.println("Content-type:application/json");
			client.println();
			client.println(sTemperature);
			client.println();
		}
		else
		{
			client.println("HTTP/1.1 200 OK");
			client.println("Content-type:text/html");
			client.println();
			client.println("404 Page not found.");
			client.println();
		}
		// close the connection:
		client.stop();
		Serial.println(clientRequest);
		Serial.println("client disconnected");
	}
}