//Includes e variaveis para conexão com internet

#include <UIPEthernet.h>

EthernetClient client;
signed long next;


uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};


//include lcd
#include <LiquidCrystal.h>

// initialize the library b#include <UIPEthernet.h>
// associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 22, en = 23, d4 = 24, d5 = 25, d6 = 26, d7 = 27;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

 //buzzer
 #define pinBuzzer 12

//RFID
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 35
#define RST_PIN 11

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 

// Init array that will store new NUID 
byte nuidPICC[4];


// See: https://www.arduino.cc/en/Reference/PROGMEM
#define PGMprintln(x) println(F(x))
#define PGMprint(x) print(F(x))



//Includes do teclado
#include <Keypad.h>

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

void liberaCatraca(uint8_t response);

void setup() {

  //iniciando mensagem do lcd
  lcd.begin(16, 2);
  lcd.clear();
  lcd.home();
  lcd.print("Iniciando");
  lcd.setCursor(0,1);
  lcd.print("Aguarde...");
  //sinal sonoro
  
  pinMode(pinBuzzer,OUTPUT); 
  tone(pinBuzzer,261,200);
  noTone(pinBuzzer);
  
  //inicia o lase de verificação
  pinMode(pinLaser, OUTPUT);
  digitalWrite(pinLaser,HIGH);
  
  //inicia o rele, inicialmente destrava a catraca, só a trava depois de conectar a internet
  pinMode(rele, OUTPUT); 
  digitalWrite(rele,HIGH);
  
  //inciando serial
  Serial.begin(19200);
  
  //rfid
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 

  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This scan the MIFARE Classsic NUID."));

  // Initializing Ethernet
  Serial.PGMprintln("Initializing Ethernet, waiting for DHCP...");
  
  int i =0;
  
  while(i == 0){
  
  lcd.clear();
  lcd.home();
  lcd.print("Conectando");
  lcd.setCursor(0,1);
  lcd.print("a internet");
  
  if(Ethernet.begin(mac)){
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());
  
  if (client.connect(IPAddress(192,168,1,144),8080))
    {
     Serial.println("Client connected"); 
     digitalWrite(rele,LOW);
     next = 0;
      i=1;
  
 }
  else{
    
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());
  next = 0;

    lcd.clear();
    lcd.home();
    lcd.print("Erro ao conectar");
    lcd.setCursor(0,1);
    lcd.print("a internet");
    delay(2000);
    i = 0;
  
  }
 }
 
i =0;
}
}


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
/* ####Respostas possives do RESPONSE####
 *  200 tudo certo, aluno existe e está autorizado;
 *  401 Aluno existe mas não está autorizado
 *  400 Aluno não existe
 */
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
     Serial.println(i);
     i=i+1;
   }
 }
  if(i == 11){
    getRequest(cpf);
  }
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
 if ( ! rfid.PICC_IsNewCardPresent())
  return;

  // Verify if the NUID has been readed
if ( ! rfid.PICC_ReadCardSerial())
  return;

Serial.print(F("PICC type: "));
MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
  piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
  Serial.println(F("Your tag is not of type MIFARE Classic."));
return;
}


Serial.println(F("A new card has been detected."));

    // Store NUID into nuidPICC array
cpf = "";
i=0;
for (byte flagLop = 0; flagLop < 4; flagLop++) {
  nuidPICC[flagLop] = rfid.uid.uidByte[flagLop];

  if(nuidPICC[flagLop]<100 && i <9){
    cpf = cpf + "0"+nuidPICC[flagLop];
    i = i +3;
  }
  else{
    if(i<9){

      cpf = cpf +nuidPICC[flagLop];
      i = i +3;
    }
  }
  if((nuidPICC[flagLop]<10) &&( i >=9) && (i < 11) &&(flagLop == 3)){
    i=i+2;
    cpf = cpf +"0"+ nuidPICC[flagLop];
  }
  else
  {

    if(( i >=9) && (i < 11)&&(flagLop == 3)){
      i=i+2;
      cpf = cpf + nuidPICC[flagLop];
    }
  }
  Serial.println("CPF: "+cpf);
  Serial.println(i);
  Serial.println(flagLop);
}
Serial.println("CPF: "+cpf);


  // Halt PICC
rfid.PICC_HaltA();

  // Stop encryption on PCD
rfid.PCD_StopCrypto1();
if(i == 11){
  getRequest(cpf);
}
}

void getRequest(String &cpf){
  Serial.println("Baianor kk");
 //verifica se já recebeu os 11 numeros do cpf
if(cpf.length() == 11){
  uint8_t a[11];
  cpf.toCharArray(a,11);
  Serial.println("Entro");
   if (((signed long)(millis() - next)) > 0)
    {
      next = millis() + 5000;
      Serial.println("Client connect");
      // replace hostname with name of machine running tcpserver.pl
//      if (client.connect("server.local",5000))
          
          client.write(a,11);
          
          if((client.available()) > 0)
            {
              uint8_t msg;
              msg = client.read();
              Serial.println(msg);
              
              liberaCatraca(msg);

                  i = 0;
      cpf = "";
      response = "";
      stp = 0;
      flagLCD=0;
            }
            else{
              Serial.println("Client connect failed");
            }
        }
      else{
      i = 0;
      cpf = "";
      response = "";
      stp = 0;
      flagLCD=0;
        Serial.println("Client connect failed");
    }
    }
    //Verifica se o conseguiu acessar a internet
    /*verifica a resposta da requisição http, 200 = aluno existe,
    tem créditos e foram descontados créditos da sua carteira*/
}

void liberaCatraca(uint8_t response){
    Serial.println(response);
    Serial.print("a");
      if(response =='2'){
      
      
      //envia mensagem que foi autorizado
      lcd.clear();
      lcd.home();
      lcd.print("Autorizado");
      //envia sinal sonoro
      tone(pinBuzzer,929,200);
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
    else if(response == 401){
       //avisa que aluno não está autorizado
      lcd.clear();
      lcd.home();
      lcd.print("Nao autorizado");
      lcd.setCursor(0,1);
      lcd.print("Informe a CGAE");
      delay(4000);
    //reinicia todas as flags e variaveis
      i = 0;
      cpf = "";
      response = "";
      stp = 0;
      flagLCD=0;
    }
    else if(response == 400){
       //avisa que aluno não existe
      lcd.clear();
      lcd.home();
      lcd.print("Dados invalidos");
      delay(2000);
    //reinicia todas as flags e variaveis
      i = 0;
      cpf = "";
      response = "";
      stp = 0;
      flagLCD=0;
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
