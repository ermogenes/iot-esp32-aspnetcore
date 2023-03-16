/* 
Uma urna que coleta votos em 3 opções diferentes: VERMELHO, BRANCO e AZUL.

Expõe uma API REST para obtenção do placar:
GET /placar
{
  "nome": string,
  "vermelho": int,
  "branco": int,
  "azul": int
}
*/

// Configurações (faça os ajustes necessários para seu ambiente)
#define WIFI_SSID "nome da sua rede aqui"
#define WIFI_PWD  "sua senha aqui"
#define NOME_URNA "urna-0x" // use um nome diferente para cada dispositivo
#define PORTA_HTTP 80
#define PAUSA_APOS_VOTO 2000

// PINs
#define BOTAO_VERMELHO 4
#define BOTAO_BRANCO   5
#define BOTAO_AZUL     15
#define VOTO_LIBERADO  13

// Libs
#include <WiFi.h>
#include <ESPmDNS.h>    ''
#include <WebServer.h>

// Placar
int votosVermelho  = 0;
int votosBranco    = 0;
int votosAzul      = 0;

// Servidor HTTP
WebServer server(PORTA_HTTP);

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n--- Iniciando urna...");

  // PINs e LEDs
  pinMode(BOTAO_VERMELHO, INPUT_PULLUP);
  pinMode(BOTAO_BRANCO, INPUT_PULLUP);
  pinMode(BOTAO_AZUL, INPUT_PULLUP);
  pinMode(VOTO_LIBERADO, OUTPUT);

  // Conectividade
  conectaWiFi();
  iniciaMDNS();
  iniciaWebServer();

  Serial.print("--- Urna [");
  Serial.print(NOME_URNA);
  Serial.print(".local] respondendo na porta [");
  Serial.print(PORTA_HTTP);
  Serial.println("]");
}

void conectaWiFi() {
  Serial.print("Conectando à rede [");
  Serial.print(WIFI_SSID);
  Serial.println("]");
  
  WiFi.begin(WIFI_SSID, WIFI_PWD);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
 
  Serial.print("\nConectado com IP [");
  Serial.print(WiFi.localIP());
  Serial.println("]");
}

void iniciaMDNS() {
  while(!MDNS.begin(NOME_URNA)) {
     Serial.println("Erro ao iniciar mDNS.");
  }
}

void iniciaWebServer() {
  server.on("/placar", HTTP_GET, getPlacar);
  server.begin();
}

void getPlacar() {
  Serial.println("GET /placar");

  String resposta = R"====(
    {
      "nome": "__URNA__",
      "vermelho": __VERMELHOS__,
      "branco": __BRANCOS__,
      "azul": __AZUIS__
    }
  )====";
  resposta.replace("__URNA__", NOME_URNA);
  resposta.replace("__VERMELHOS__", String(votosVermelho));
  resposta.replace("__BRANCOS__", String(votosBranco));
  resposta.replace("__AZUIS__", String(votosAzul));

  server.send(200, "application/json", resposta);
}

void loop() {
  digitalWrite(VOTO_LIBERADO, HIGH);

  if      (digitalRead(BOTAO_VERMELHO) == LOW) votosVermelho++;
  else if (digitalRead(BOTAO_AZUL)     == LOW) votosAzul++;
  else if (digitalRead(BOTAO_BRANCO)   == LOW) votosBranco++;
  else { 
    // Atende requisições HTTP
    server.handleClient();
    delay(2);
    return;
  }

  digitalWrite(VOTO_LIBERADO, LOW);
  exibePlacar();
  delay(PAUSA_APOS_VOTO);
}

void exibePlacar() {
  Serial.print("Vermelho: "); Serial.print(votosVermelho);
  Serial.print(", Branco: "); Serial.print(votosBranco);
  Serial.print(", Azul: ");   Serial.println(votosAzul);
}
