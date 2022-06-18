 //BIBLIOTECAS
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//Atribuição de botões
#define Sensor1 D6
#define Sensor2 D5
#define pinled D4

//VARIAVEIS Do SENSOR
//Variavel que registra o estado do sensor entrada
int valorent = 0;   
// Variavel que registra o estado do sensor de saida
int valorsai = 0;   
// Variavel que registra a quantidade de visitantes que entraram
int sensorent = 0;      
// Variavel que registra a quantidade de visitantes que sairam
int sensorsai = 0;      
// Variavel que registra a quantidade de pessoas na sala
int totalsala = 0;     
// Variavel que registra a quantidade anterior de pessoas na sala
int totalanterior = 0; 

//VARIAVEIS DO DISPLAY
int lcdColumns = 16;
int lcdRows = 2;

//CONEXÃO ASP - DISPLAY I2C
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);

//VARIAVEIS CONEXÃO ESP - HTML

const char* ssid = "Janayna";
const char* password = "nanae6158";

//CONEXÃO PORTA
ESP8266WebServer server(80);

//VARIAVEL QUE LIGA A LUZ DO ESP
const int led = 2; 

//FUNÇÃO COM RESPOSTA HTML
void paginahtmlon() {
  digitalWrite(led, 1);

  String page = 
  "<html>"
  "<script>function autoRefresh(){window.location = window.location.href;}setInterval('autoRefresh()',2000);</script>"
  "<title>Sistema de Contagem</title></head>"
  "<style>"
  "h2{font-family: Arial, font-size: 25px;color: whitesmoke;line-height: 400%}"
  "h1{font-size: 50px;color: whitesmoke;margin: 30px;text-align: center;}"
  "body {background-image: url(https://nova-escola-producao.s3.amazonaws.com/EBUupJuWp5ttKkmKf5PZXN2rdxsMJ376JtzZaTE7MQRdbWVNrt5CsVgFGaDK/gettyimages-663622960.jpg);background-repeat: no repeat;background-size: center;background-position: rigth center;}"
  "<meta http-equiv=""refresh" "content=""2"">"
  "</style>""<body>"
  "<h1>Sistema de Contagem</h1>";

  String page2 ="<h2>Entrada de pessoas: ";
  
  String page3 ="</h2><h2>Saida de Pessoas: ";
 
 String page4 = "<h2>Total de Pessoas no Ambiente: ";

 String page5 = "</h2><h2>Capacidade Maxima de Pessoas: 20</h2>"
  "</body>"
  "</html>";


  
  server.send(200, "text/html", page + page2 + sensorent + page3 + sensorsai + page4 + totalsala + page5);
  digitalWrite(led, 0);


}

void paginahtmloff(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  //CONEXÃO DISPLAY
  
 pinMode(Sensor1,INPUT);
 pinMode(Sensor2,INPUT);
 pinMode(pinled,OUTPUT);
 
    Serial.begin(115200);            
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.begin(16, 2);
  lcd.setCursor(0,0);            
  lcd.print("TOT: ");          
  lcd.setCursor(9,0);
  lcd.print("CAP:20 ");
  lcd.setCursor(0,1);
  lcd.print("ENT: ");
  lcd.setCursor(9,1);
  lcd.print("SAI: ");
 
  //CONEXÃO WIFI
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  //IMPRESSÃO MONITOR SERIAL SERVER
  Serial.println("");
  Serial.print("Conectado a rede: ");
  Serial.println(ssid);
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

 //CHAMADA DA PAGINA HTML
  server.on("/contador", paginahtmlon);
  server.onNotFound(paginahtmloff);
  server.begin();
  Serial.println("HTTP server started");

}
  void loop(){
  server.handleClient();
 
   // Seta a variavel valorirent com o status do sensor de entrada
  valorent = digitalRead(Sensor1);    
  // Seta a variavel valorirsai com o status do sensor de saida
  valorsai = digitalRead(Sensor2);  
  // Mostra as informacoes dos sensores no serial monitor
  Serial.print("entrada : ");
  Serial.print(sensorent);
  Serial.print(" saida : ");
  Serial.println(sensorsai);
  // Verifica se o estado do sensor de entrada foi alterado
  if(valorent!=1)
  {
    // Incrementa o valor do contador de entrada em 1 unidade
    sensorent=sensorent+1;
    // Verifica se a pessoa/objeto para em frente ao sensor
    while(digitalRead(Sensor1)!=1)
      {
        delay(100);  
        Serial.println("Sensor de entrada parado !");
      }
    // Atualiza no LCD o valor do contador de entrada
    lcd.setCursor(5,1);
    lcd.print(sensorent);
    // Atualiza no LCD o valor total de visitantes
    lcd.setCursor(5,0);
    lcd.print(sensorent);
  }
  // Verifica se o estado do sensor de saida foi alterado
  if(valorsai!=1)
  {
    // Incrementa o valor do contador de saida em 1 unidade
    sensorsai=sensorsai+1;
    // Verifica se a pessoa/objeto para em frente ao sensor
    while(digitalRead(Sensor2)!=1)
      {
       delay(100);
       Serial.println("Sensor de saida parado !");
      }
    // Atualiza no LCD o valor do contador de saida
    lcd.setCursor(14,1);
    lcd.print(sensorsai);  
  }
  // Calcula o numero de visitantes na sala
  totalsala=sensorent-sensorsai;
    if (totalsala != totalanterior)
  {
    lcd.setCursor(5,0);
    lcd.print("  ");
    lcd.setCursor(5,0);
    lcd.print(totalsala);
  }
    totalanterior = totalsala;

if (totalsala >= 20){

   digitalWrite(pinled,HIGH);
 
}
if(totalsala <= 20){
 
  digitalWrite(pinled,LOW);
 
}
}
