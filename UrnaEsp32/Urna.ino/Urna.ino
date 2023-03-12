/* 
Uma urna que coleta votos em 2 opções diferentes, AZUL e VERMELHO,
com opção de abstenção (BRANCO).

Expõe uma API REST para controle remoto:
GET /           => info sobre a urna
GET /reiniciar  => reinicia contagem
GET /placar     => contagem atual de votos
*/

// Configurações
#define WIFI_SSID "nome da rede"
#define WIFI_PWD  "senha da rede"
#define NOME_URNA "urna-0?" // use um nome diferente para cada dispositivo
#define PORTA_HTTP 80

// Libs
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>

// PINs
#define BOTAO_VERMELHO 4
#define BOTAO_BRANCO   5
#define BOTAO_AZUL     15
#define VOTO_LIBERADO  13

// Parâmetros
#define PAUSA_APOS_VOTO 2000

// Placar
int votosVermelho  = 0;
int votosBranco    = 0;
int votosAzul      = 0;

// Servidor HTTP
WebServer server(PORTA_HTTP);
String url = "http://__NOME_URNA__.local:__PORTA_HTTP__/";

void setup() {
  Serial.begin(115200);
  Serial.println("\n\n--- Iniciando urna...");

  // PINs
  pinMode(BOTAO_VERMELHO, INPUT_PULLUP);
  pinMode(BOTAO_BRANCO, INPUT_PULLUP);
  pinMode(BOTAO_AZUL, INPUT_PULLUP);

  // LEDs
  pinMode(VOTO_LIBERADO, OUTPUT);

  // Wi-fi
  conectaWiFi();

  // mDNS
  iniciaMDNS();

  // Web server
  iniciaWebServer();

  url.replace("__NOME_URNA__", NOME_URNA);
  url.replace("__PORTA_HTTP__", String(PORTA_HTTP));

  Serial.println("Respondendo em " + url);
  Serial.println("--- Urna online");
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
  // Rotas
  server.on("/", HTTP_GET, getInfo);
  server.on("/reiniciar", HTTP_GET, getReiniciar);
  server.on("/placar", HTTP_GET, getPlacar);
  server.begin();
}

void getInfo() {
  Serial.println("GET /");

  String resposta = R"====(
    {
      "online": true,
      "urna": "__URNA__"
    }
  )====";
  resposta.replace("__URNA__", NOME_URNA);

  server.send(200, "application/json", resposta);
}

void getReiniciar() {
  Serial.println("GET /reiniciar");

  votosVermelho  = 0;
  votosBranco    = 0;
  votosAzul      = 0;

  server.send(200);
}

void getPlacar() {
  Serial.println("GET /placar");

  String resposta = R"====(
    {
      "urna": "__URNA__",
      "vermelhos": __VERMELHOS__,
      "brancos": __BRANCOS__,
      "azuis": __AZUIS__
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
