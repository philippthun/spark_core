/*
  WebServer

  Uses the following libraries:
  - Webduino port for Spark Core (https://github.com/m-mcgowan/Webduino)
  - Adafruit MDNS port for Spark Core (https://github.com/krvarma/MDNS_SparkCore)

  --------------------------------------------------------------------------------
  Copyright (c) 2015 Philipp Thun

  Permission is hereby granted, free of charge, to any person obtaining a copy of
  this software and associated documentation files (the "Software"), to deal in
  the Software without restriction, including without limitation the rights to
  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
  the Software, and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
  --------------------------------------------------------------------------------
*/

#include "application.h"
#include "WebServer/WebServer.h"
#include "mdns/mdns.h"

#define PREFIX ""
WebServer webServer(PREFIX, 80);
MDNSResponder mdns;
char macAddress[32];
char *ssid;
int rssi;
char ipAddress[32];
char gateway[32];

void rootCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  if (type == WebServer::GET)
  {
    server.httpSuccess();
    server.printf("<h1>Hello, I'm Sparky!</h1>");
  }
  else
  {
    server.httpFail();
  }
}

void networkCmd(WebServer &server, WebServer::ConnectionType type, char *, bool)
{
  if (type == WebServer::GET)
  {
    server.httpSuccess();
    server.print("<h1>Network Info</h1>");
    server.print("<dl>");
    server.printf("<dt>MAC Address</dt><dd>%s</dd>", macAddress);
    server.printf("<dt>SSID</dt><dd>%s</dd>", ssid);
    server.printf("<dt>RSSI</dt><dd>%ddB</dd>", rssi);
    server.printf("<dt>IP Address</dt><dd>%s</dd>", ipAddress);
    server.printf("<dt>Gateway</dt><dd>%s</dd>", gateway);
    server.print("</dl>");
  }
  else
  {
    server.httpFail();
  }
}

void setup()
{
  byte mac[6]; WiFi.macAddress(mac);
  ssid = WiFi.SSID();
  rssi = WiFi.RSSI();
  IPAddress localIp = WiFi.localIP();
  IPAddress gatewayIp = WiFi.gatewayIP();

  sprintf(macAddress, "%02x:%02x:%02x:%02x:%02x:%02x", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
  sprintf(ipAddress, "%d.%d.%d.%d", localIp[0], localIp[1], localIp[2], localIp[3]);
  sprintf(gateway, "%d.%d.%d.%d", gatewayIp[0], gatewayIp[1], gatewayIp[2], gatewayIp[3]);

  Spark.variable("ipAddress", ipAddress, STRING);

  webServer.setDefaultCommand(&rootCmd);
  webServer.addCommand("network", &networkCmd);
  webServer.begin();

  uint32_t ip = (localIp[0] * 16777216) + (localIp[1] * 65536) + (localIp[2] * 256) + (localIp[3]);
  mdns.begin("sparky", ip);
}

void loop()
{
  char buff[64];
  int len = 64;

  webServer.processConnection(buff, &len);
  mdns.update();
}
