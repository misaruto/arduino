//Includes e variaveis para conexão com internet
#include <UIPEthernet.h>
#include <Dns.h>
#include <ArduinoHttpClient.h>
// See: https://www.arduino.cc/en/Reference/PROGMEM
#define PGMprintln(x) println(F(x))

#define PGMprint(x) print(F(x))
// MAC address configuration
const uint8_t mac[] = { 0x78 , 0x2B, 0xCB, 0xBF, 0xA8, 0x34 };
// Destination host and port
const char dsthost[] = "192.168.1.103";
const uint16_t dstport = 80;
int port = 80;
// Our three main objects
EthernetClient client;
DNSClient DNSclient;
IPAddress dstip;

//Includes do teclado
#include <Keypad.h>

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
int pinLaser = 22;
void setup() {
  pinMode(pinLaser, OUTPUT);
  pinMode(rele, OUTPUT); 
  Serial.begin(19200);
  digitalWrite(pinLaser,HIGH);
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
  int i = 0;
  while(i ==0){
   if (DNSclient.getHostByName(dsthost, dstip)) {
    Serial.PGMprint("Resolved remote host to: ");
    Serial.println(dstip);
    Serial.println(dsthost);
    i =1;
    digitalWrite(rele,LOW);
  } else {
    Serial.PGMprintln("Error: could not resolve remote host.");
    i = 0;
  }
}
 delay(2000);
}

HttpClient client1 = HttpClient(client, dsthost, port);
String cpf= "";
int i = 0;
int stop = 0;
String response = "";

bool travaCatraca(){
  int x =1;
  int c =0;
  Serial.println("Entou na funcao");
  while(x == 1){
    Serial.println(analogRead(A8));
    if(analogRead('A8') >=800){
      c = 1;
    }
    if (analogRead(A8)<=400 && c ==1){
      x =0;
      return true;
    }
  }
}

String one;
void loop() {
  Serial.println("Digite uma tecla: ");
    char tecla_presionada = teclado.getKey();    //Almaceno en una variable la tecla presionada
    if (tecla_presionada != NULL){
      if (tecla_presionada == 'A'){
        cpf="";
        i=0;
      }
      else{
       cpf = cpf + tecla_presionada;
      Serial.print("CPF:");
     Serial.println(cpf);
     i=i+1;
     }
   }
   if(i == 1 && stop == 0 ){
    if(response == "" && stop == 0){
      Serial.println("making GET request");
      client1.get("/arduino/api/index.php?acao=controleCatraca&cpf="+cpf);
      int statusCode = client1.responseStatusCode();
      response = String(client1.responseBody());
      Serial.print("Status code: ");
      Serial.println(statusCode);
      Serial.print("Response:"+response);
      Serial.println(response);
    }
      if(response.equals("1")){
        stop =1;
        digitalWrite(rele,HIGH);
        
        if(travaCatraca()){

          digitalWrite(rele,LOW);

          i = 0;
          cpf = "";
          delay(2000);
          response = "";
          stop = 0;
      }
    }
    else{
                i = 0;
          cpf = "";
          delay(2000);
          response = "";
          stop = 0;
    }
  }
}
