#include "WifiManager.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

static const char *AP_SSID = "CalidadAire-Config";
static const unsigned long WIFI_TIMEOUT_MS = 15000;

static Preferences prefs;
static WebServer server(80);
static DNSServer dnsServer;

static String scanSSIDs[30];
static int scanCount = 0;
static volatile bool shouldRestart = false;

static const char HTML_HEAD[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Configurar WiFi</title>
<style>
*{margin:0;padding:0;box-sizing:border-box}
body{font-family:system-ui,sans-serif;background:#16213e;color:#fff;display:flex;justify-content:center;align-items:center;min-height:100vh;padding:20px}
.card{background:#1a1a2e;border-radius:12px;padding:24px;max-width:360px;width:100%;box-shadow:0 4px 20px rgba(0,0,0,.4)}
h2{text-align:center;margin-bottom:20px;font-size:1.3rem}
label{display:block;font-size:.8rem;color:#aaa;margin-bottom:4px}
select,input{width:100%;padding:10px;border:1px solid #333;border-radius:8px;background:#0f3460;color:#fff;font-size:1rem;margin-bottom:16px}
select:focus,input:focus{outline:none;border-color:#e94560}
button{width:100%;padding:12px;background:#e94560;border:none;border-radius:8px;color:#fff;font-size:1rem;font-weight:600;cursor:pointer}
button:hover{background:#c73650}
.spinner{display:none;text-align:center;padding:16px;color:#e94560}
</style>
</head>
<body>
<div class="card">
<h2>Configurar WiFi</h2>
<form onsubmit="return conectar()">
<label>Red</label>
<select id="ssid">
)rawliteral";

static const char HTML_MID[] PROGMEM = R"rawliteral(
</select>
<label>Contrasena</label>
<input id="pass" type="password" placeholder="Clave WiFi">
<button type="submit">Conectar</button>
</form>
<div class="spinner" id="spin">Conectando...</div>
</div>
<script>
function conectar(){
document.querySelector('form').style.display='none';
document.getElementById('spin').style.display='block';
var s=document.getElementById('ssid').value;
var p=document.getElementById('pass').value;
fetch('/save',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'ssid='+encodeURIComponent(s)+'&pass='+encodeURIComponent(p)});
return false;
}
</script>
</body>
</html>
)rawliteral";

static void sendPortalPage() {
  String html;
  html.reserve(3000);
  html += FPSTR(HTML_HEAD);
  if (scanCount > 0) {
    for (int i = 0; i < scanCount; i++) {
      html += "<option>" + scanSSIDs[i] + "</option>";
    }
  } else {
    html += "<option>-- sin redes --</option>";
  }
  html += FPSTR(HTML_MID);
  server.send(200, "text/html", html);
}

static void scanNetworks() {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(200);
  int n = WiFi.scanNetworks();
  if (n <= 0) {
    scanCount = 0;
    return;
  }
  if (n > 30) n = 30;
  scanCount = n;
  for (int i = 0; i < n; i++) {
    scanSSIDs[i] = WiFi.SSID(i);
  }
  WiFi.scanDelete();
}

bool hasWifiConfig(void) {
  prefs.begin("wifi", true);
  String ssid = prefs.getString("ssid", "");
  prefs.end();
  return ssid.length() > 0;
}

bool setupWiFi(void) {
  prefs.begin("wifi", true);
  String ssid = prefs.getString("ssid", "");
  String pass = prefs.getString("pass", "");
  prefs.end();

  if (ssid.length() == 0) return false;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());

  Serial.print("Conectando a ");
  Serial.print(ssid);

  const unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_TIMEOUT_MS) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi conectado.");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.println("\nNo se pudo conectar.");
  return false;
}

static void registerRoutes() {
  server.on("/", sendPortalPage);
  server.on("/generate_204", sendPortalPage);
  server.on("/hotspot-detect.html", sendPortalPage);

  server.on("/save", []() {
    if (!server.hasArg("ssid") || !server.hasArg("pass")) {
      server.send(400, "text/plain", "Faltan datos");
      return;
    }

    prefs.begin("wifi", false);
    prefs.putString("ssid", server.arg("ssid"));
    prefs.putString("pass", server.arg("pass"));
    prefs.end();

    server.send(200, "text/html", R"rawliteral(
<!DOCTYPE html><html lang="es"><head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>OK</title><style>body{font-family:system-ui,sans-serif;background:#16213e;color:#fff;display:flex;justify-content:center;align-items:center;height:100vh;text-align:center}
.ok{color:#28a745;font-size:3rem;margin-bottom:10px}h2{font-size:1.2rem}</style></head><body><div>
<div class="ok">&#10003;</div><h2>Configuracion guardada</h2><p>Reiniciando...</p></div></body></html>
)rawliteral");

    delay(2000);
    shouldRestart = true;
  });

  server.onNotFound(sendPortalPage);
}

// Portal cautivo: crea red WiFi propia con pagina de configuracion
void startConfigPortal(void) {
  scanNetworks();

  WiFi.mode(WIFI_AP_STA);
  if (!WiFi.softAP(AP_SSID)) {
    Serial.println("Error al iniciar AP. Reiniciando en 2s...");
    delay(2000);
    ESP.restart();
  }

  dnsServer.start(53, "*", WiFi.softAPIP());

  static bool routesRegistered = false;
  if (!routesRegistered) {
    routesRegistered = true;
    registerRoutes();
  }

  server.begin();

  Serial.println("\nPortal cautivo activo. Conectate a: CalidadAire-Config");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  while (true) {
    if (shouldRestart) ESP.restart();
    dnsServer.processNextRequest();
    server.handleClient();
    delay(10);
  }
}
