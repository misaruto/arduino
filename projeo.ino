//Includes e variaveis para conexão com internet
#include <UIPEthernet.h>
#include <Dns.h>
#include <ArduinoHttpClient.h>

//Includes do teclado
#include <Keypad.h>

//include lcd
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 22, en = 23, d4 = 24, d5 = 25, d6 = 26, d7 = 27;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// See: https://www.arduino.cc/en/Reference/PROGMEM
#define PGMprintln(x) println(F(x))
#define PGMprint(x) print(F(x))

// MAC address configuration
const uint8_t mac[] = { 0x78 , 0x2B, 0xCB, 0xBF, 0xA8, 0x34 };
// Destination host and port
const char dsthost[] = "192.168.1.141";
const uint16_t dstport = 80;
int port = 80;
// Our three main objects
EthernetClient client;
DNSClient DNSclient;
IPAddress dstip;
//constantes de inicialização do teclado
const byte filas = 4;     //Numero de filas del teclado
const byte columnas = 4;  //Numero de columnas del teclado

//Defino una matriz 4x4 con la posicion de las filas y columnas
char matriz[filas][columnas] =
{

  { '1', '2', '3', 'A'},
  { '4', '5', '6', 'B'},
  { '7', '8', '9', 'C'},
  { '*', '0', '#', 'D'},

};

byte pinesFilas[filas] = {9, 8, 7, 6};       //Pines donde van conectadas las filas del teclado
byte pinesColumnas[columnas] = {5, 4, 3, 10}; //Pines donde van conectadas las columnas del teclado

//Inicializo el teclado con el numero de filas, columnas, los pines del Arduino utilizados y la matriz
Keypad teclado = Keypad( makeKeymap(matriz), pinesFilas, pinesColumnas, filas, columnas);

int rele = 48;
int pinLaser = 28;

void setup() {

  //iniciando mensagem do lcd
  lcd.begin(16, 2);
  lcd.clear();
  lcd.home();
  lcd.print("Iniciando");
  lcd.setCursor(0,1);
  lcd.print("Aguarde...");

  pinMode(pinLaser, OUTPUT);
  digitalWrite(pinLaser,HIGH);
  pinMode(rele, OUTPUT); 
  
  //inciando serial
  Serial.begin(19200);

  // Initializing Ethernet
  Serial.PGMprintln("Initializing Ethernet, waiting for DHCP...");
  Ethernet.begin(mac);
  Serial.PGMprint("IP: ");
  Serial.println(Ethernet.localIP());
  Serial.PGMprint("Subnet mask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.PGMprint("Gateway: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.PGMprint("DNS: ");
  Serial.println(Ethernet.dnsServerIP());

  // Initializing DNS
  Serial.PGMprintln("Initializing DNS client...");
  DNSclient.begin(Ethernet.dnsServerIP());
  int i =0;
  while(i ==0){
    if (DNSclient.getHostByName(dsthost, dstip)) {
      Serial.PGMprint("Resolved remote host to: ");
      Serial.println(dstip);
      Serial.println(dsthost);
      i =1;
      delay(5000);
      digitalWrite(rele,LOW);
      lcd.clear();
      lcd.home();
      lcd.print("Sistema inciado");
      lcd.setCursor(0,1);
      lcd.print("Insira o CPF");
    } 
    else {
      Serial.PGMprintln("Error: could not resolve remote host.");
      lcd.clear();
      lcd.home();
      lcd.print("Erro ao conectar");
      lcd.setCursor(0,1);
      lcd.print("a internet");
      i = 0;
    }
  }
  delay(2000);
}

HttpClient client1 = HttpClient(client, dsthost, port);

//função que verifica o estado da catraca
bool travaCatraca(){

  //flag de entrada no loop
  int flagLoop =1;
  //flag que verifica se o ldr foi em algum momento coberto.
  int flagLDR =0;
  
  Serial.println("Entou na funcao");
  
  while(flagLoop == 1){
    Serial.println(analogRead(A8));

    /*verifica se o pino A8 que conecta a um LDR se fez leitura menor que 400,
    o que indica que o sensor foi tapado*/
    if(analogRead(A8) <=400){
      //muda o valor da flag
      flagLDR = 1;
    }

    //verifica se há incidencia de luz no ldr e se o mesmo já foi tapado anteriormente
    if (analogRead(A8)>=800 && flagLDR == 1){
      flagLoop =0;
      return true;
    }
  }
}

String cpf= "";
int i = 0;
int stp = 0;
String response = "";
int flagLCD=0;
int statusCode = 0;
void loop() {

  //flag pra verifica se a mensagem já apareceu, para não reenviar informações para o LCD
  if(flagLCD == 0){
    Serial.println("Digite uma tecla: ");
    lcd.clear();
    lcd.home();
    lcd.print("Digite seu CPF");
    lcd.setCursor(0,1);
    flagLCD = 1;
  }
  
  char tecla_presionada = teclado.getKey();    //Almaceno en una variable la tecla presionada
  if (tecla_presionada != NULL){
    if (tecla_presionada == 'A'){
      cpf="";
      i=0;
    }
    else{
     cpf = cpf + tecla_presionada;
     //Muda mensagem da tela
     lcd.clear();
     lcd.home();
     lcd.print("Digite seu CPF");
     lcd.setCursor(0,1);
     lcd.print("CPF:"+cpf);
     //informações para serial
     Serial.print("CPF:");
     Serial.println(cpf);
     i=i+1;
   }
 }

 //verifica se já recebeu os 11 numeros do cpf
 if(i == 1 && stp == 0 ){
  /*verifica se já recebeu o resultado da verificação no banco de dados e se já pode acessar
  essa area -> stop = 1 -> não; stop = 0 ->Sim */
  if(response == "" && stp == 0){
    Serial.println("making GET request");
    lcd.clear();
    lcd.home();
    lcd.print("Verificando...");
    client1.get("/arduino/api/index.php?acao=controleCatraca&cpf="+cpf);
    statusCode = client1.responseStatusCode();
    response = String(client1.responseBody());
    Serial.print("Status code: ");
    Serial.println(statusCode);
    Serial.print("Response:"+response);
    Serial.println(response);
  }
    //Verifica se o conseguiu acessar a internet
  if(statusCode == 200){
    /*verifica a resposta da requisição http, 200 = aluno existe,
    tem créditos e foram descontados créditos da sua carteira*/
    if(response.equals("200")){
      //envia mensagem que foi autorizado
      lcd.clear();
      lcd.home();
      lcd.print("Autorizado");

      //flag para não fazer uma nova requisição
      stp =1;

      //libera a catraca
      digitalWrite(rele,HIGH);

      //envia mensagem de que liberou a catraca
      lcd.setCursor(0,1);
      lcd.print("Catraca liberada");

    //chama e verifica a função que verifica os casos da catraca
      if(travaCatraca()){

      //trava a catraca
        digitalWrite(rele,LOW);
      //zera todas as variaveis
      //envia mensagem de que travou a catraca
        lcd.setCursor(0,1);
        lcd.print("Catraca travada");
        i = 0;
        cpf = "";
        response = "";
        stp = 0;
        flagLCD=0;
      }
    }
    else{
    //avisa que algo deu errado e informa o codigo do erro
      lcd.clear();
      lcd.home();
      lcd.print("Erro, codigo:");
      lcd.setCursor(0,1);
      lcd.print(response);

    //reinicia todas as flags e variaveis
      i = 0;
      cpf = "";
      response = "";
      stp = 0;
      flagLCD=0;
    }
  }
  else{
   lcd.clear();
   lcd.home();
   lcd.print("Erro:");
   lcd.setCursor(0,1);
   lcd.print("Desconectado.");
   setup();
   HttpClient client1 = HttpClient(client, dsthost, port);
   i = 0;
   cpf = "";
   response = "";
   stp = 0;
   flagLCD=0;
 }
}
}
