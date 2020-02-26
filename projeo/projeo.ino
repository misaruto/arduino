//Include interface SPI
//Usada tanto pelo UIPEthernet quanto pelo MRC522
//            #####explicando######
/*Os dois modulos se conectam nas mesmas portas, porem tem
2 pinos especificos para cada um, que é o de conexão do modulo com
o arduino, no UIPEthernet é o 51 e no MFRC522 é o 35 */
#include <SPI.h>

//Includes e variaveis para conexão com internet
#include <UIPEthernet.h>

EthernetClient client;
signed long next;
//Define um mac para a interface de internet
uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05};
//Variaveis para conexão
const int ip[4] = {192,168,1,144};
const int port = 8080;

//Include da biblioteca do RFID
#include <MFRC522.h>
//Define os pinos de conexão do modulo com o arduino
#define SS_PIN 35
#define RST_PIN 11
//Instancia a classe
MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
//Define o tipo de etiqueta que será lida, no caso MIFARE Classic
MFRC522::MIFARE_Key key; 

//include lcd
#include <LiquidCrystal.h>
//Armazena os vaores dos pinos onde o LCD está conectado
const int rs = 22, en = 23, d4 = 24, d5 = 25, d6 = 26, d7 = 27;
//Instancia a variavel do tipo LCD que controla o LCD
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//Includes do teclado
#include <Keypad.h>

//constantes de inicialização do teclado
const byte filas = 4;     //Numero de filas do teclado
const byte colunas = 4;  //Numero de colunas do teclado

//Defino una matriz 4x4 com as posições de cada item do teclado
char matriz[filas][colunas] =
{

  { '1', '2', '3', 'A'},
  { '4', '5', '6', 'B'},
  { '7', '8', '9', 'C'},
  { '*', '0', '#', 'D'},

};

byte pinsFilas[filas] = {9, 8, 7, 6}; //Pinos onde estão conectados as linhas do teclado
byte pinsColumnas[colunas] = {5, 4, 3, 10}; //Pinos onde estão conectadas as colunas do teclado
//Instacio a classe do teclado, passando a matriz, pinos das filas e das colunas.
Keypad teclado = Keypad( makeKeymap(matriz), pinsFilas, pinsColumnas, filas, colunas);

//###### Constantes para os modulos menores ####
//Pino do buzzer
#define pinBuzzer 12
//pino do rele
#define rele  48
//pino do laser
#define pinLaser  28

//Variaves de controle
int contDigitos = 0;

//Variveis de dados e respostas
String cpf= "";

int flagLCD = 0;
int statusCode = 0;
//Função que converte o resultdado da leitura do cartão rfid para String
int convertArrayCpf(MFRC522::Uid &nuidPICC);

//Função que verifica os dados do aluno
void liberaCatraca(uint8_t &response);

//Declara a função responsavel por saber quando a catraca foi girada
bool travaCatraca();

//declara função que imprime os dados da conexão
void printConexaoStatus();

//declara função que faz a comunicação via tcp com o servidor
void comunicaComServidor();

void setup() {

  //iniciando mensagem do lcd
  lcd.begin(16,2);
  lcd.clear();
  lcd.home();
  lcd.print("Iniciando");
  lcd.setCursor(0,1);
  lcd.print("Aguarde...");
  
  //sinal sonoro
  pinMode(pinBuzzer,OUTPUT); 
  tone(pinBuzzer,361,200);
  noTone(pinBuzzer);
  
  //inicia o laser de verificação
  pinMode(pinLaser, OUTPUT);
  digitalWrite(pinLaser,HIGH);
  
  //inicia o rele, inicialmente destrava a catraca, só a trava depois de conectar a internet
  pinMode(rele, OUTPUT); 
  digitalWrite(rele,HIGH);
  
  //inciando serial
  Serial.begin(19200);
  
  //Inicia a interface spi
  SPI.begin(); // Init SPI bus
  //Inicia o modulo do RFID
  rfid.PCD_Init(); // Init MFRC522 
  //Preenche a chave do leitor, padrão 0xFF
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }


  int flagInternet = 0;
  
  while(flagInternet == 0){

    lcd.clear();
    lcd.home();
    lcd.print("Conectando");
    lcd.setCursor(0,1);
    lcd.print("a internet");

    if(Ethernet.begin(mac)){

      if (client.connect(IPAddress(ip[0],ip[1],ip[2],ip[3]),port)){

       Serial.println("Client connected"); 
       printConexaoStatus();
       digitalWrite(rele,LOW);
       next = 0;
       flagInternet =1;

     }
     else{
      Serial.println("Erro ao conectar");
      printConexaoStatus();

      next = 0;
      lcd.clear();
      lcd.home();
      lcd.print("Erro ao conectar");
      lcd.setCursor(0,1);
      lcd.print("a internet");
      delay(2000);
      flagInternet = 0;
    }
  }
  else{
   Serial.println("Erro ao conectar a internet.");
   Serial.println("Tentando novamente");

   lcd.clear();
   lcd.home();
   lcd.print("Erro ao conectar");
   lcd.setCursor(0,1);
   lcd.print("a internet");

   delay(2000);
   flagInternet = 0;
 }
}
}



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
      contDigitos = 0;
      flagLCD = 0;
    }
    else{
      //concatena o novo digito com os que ja tem na string
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
     Serial.println(contDigitos);
     contDigitos = contDigitos + 1;
   }
 }
 if(contDigitos == 11){
  comunicaComServidor();
}
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
if ( ! rfid.PICC_IsNewCardPresent()){
  return;
}

  // Verify if the NUID has been readed
if ( ! rfid.PICC_ReadCardSerial()){
 return;
}

Serial.print(F("PICC type: "));
MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
if ((piccType != MFRC522::PICC_TYPE_MIFARE_MINI) &&  (piccType != MFRC522::PICC_TYPE_MIFARE_1K) && (piccType != MFRC522::PICC_TYPE_MIFARE_4K)) {

  Serial.println(F("Your tag is not of type MIFARE Classic."));
  return;
}
Serial.println(F("A new card has been detected."));

contDigitos = convertArrayCpf(rfid.uid); 

if(contDigitos == 11){
  comunicaComServidor();

}
}
//função que imprime o status da conexão com a internet
void printConexaoStatus(){
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());
}

int convertArrayCpf(MFRC522::Uid &nuid){
  //Inicia o variavel temporaria que armazena os valores lidos do RFID 
  byte nuidPICC;
  // Converte a NUID de array para String
  cpf = "";
  contDigitos=0;
  // laço que vai de 0 a 3, pois são coletado em 3 blocos de 3 digitos e 1 de 2 digitos.
  for (byte flagLop = 0; flagLop < 4; flagLop++) {
  //transforma em array local o que vem do 
    nuidPICC = nuid.uidByte[flagLop];
  //verifica se o digito é menor que 100 e está entre o 1° e o 9° digito
    if(nuidPICC < 100 && contDigitos < 9){
    //se for menor que 100 é necessário adcionar o 0 antes, para se ter 11 digitos
      cpf = cpf + "0"+nuidPICC;
    //incrementa mais 3 no contador de digitos
      contDigitos = contDigitos +3;
    }
  //se os digitos não forem menores que 100
    else{
    //verifica se ainda está entre o 1° e o 9° digito
      if(contDigitos < 9){
      //se sim concatena na string
        cpf = cpf +nuidPICC;
      //incrementa mais 3 nos digitos
        contDigitos = contDigitos +3;
      }
    }
  //Verifica se os ultimos digitos são menores que 10, se está entre o 9° e 11° digito e se está no ultimo bloco 
    if((nuidPICC < 10) &&( contDigitos >=9) && (contDigitos < 11) &&(flagLop == 3)){
    //Concatena o valor com o 0 antes
      cpf = cpf +"0"+ nuidPICC;
    //incrementa o numero de digitos
      contDigitos = contDigitos+2;
    }
  //senão
    else{
    //se for os ultimos digitos, porem são maiores que 10
      if(( contDigitos >=9) && (contDigitos < 11)&&(flagLop == 3)){
        //concatena a String
        cpf = cpf + nuidPICC;
        //incrementa os digitos
        contDigitos=contDigitos+2;
      }
    }
  //imprime a evolução da coleta dos digitos
    Serial.println("CPF: "+cpf);
    Serial.println(contDigitos);
    Serial.println(flagLop);
  }
  //imprime a String contendo os digitos por completa
  Serial.println("CPF: "+cpf);

  // Halt PICC
  rfid.PICC_HaltA();
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  return contDigitos;
}

//função que verifica o estado da catraca, sempre retorna true.
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

//faz a comunicação com o servidor
void comunicaComServidor(){
  Serial.println("Baianor kk");
 //verifica se já recebeu os 11 numeros do cpf
  if(cpf.length() == 11){
    //cria um vetor de 11 espaços
    uint8_t cpf_converted[11];
    //converte a string cpf para um array uint8_t
    cpf.toCharArray(cpf_converted,11);
    //faz uma verificação necessária mas que nn faço a minima ideia do pq
    if (((signed long)(millis() - next)) > 0){

      next = millis() + 5000;
      Serial.println("Client connect");
      //comunica com o servidor via tcp
      client.write(cpf_converted,11);
      //verirca se a comunicação aind persiste
      if((client.available()) > 0){
        //variavel temporaria para a resposta do server
        uint8_t msg;
        //pega a resposta do servidor, retorno em ASCII
        msg = client.read();

        Serial.println(msg);
        //chama função que libera a catraca
        liberaCatraca(msg);

        contDigitos = 0;
        cpf = "";        
        flagLCD=0;
      }
      else{
        Serial.println("Client connect failed");
      }
    }
    else{
      contDigitos = 0;
      cpf = ""; 
      flagLCD=0;

      Serial.println("Client connect failed");
    }
  }
    //Verifica se o conseguiu comunicar com o servidor
    /*verifica a resposta da comunicação tcp, 2 = aluno existe,
    tem créditos e foram descontados créditos da sua carteira*/
}
//função que pega a resposta do servidor e decide se libera ou nn
void liberaCatraca(uint8_t &response){

  /* ####Respostas possives do RESPONSE####
            #Respostas vem em ASCII#
 *  2 - 50 tudo certo, aluno existe e está autorizado;
 *  4 - 53 Aluno existe mas não está autorizado
 *  5 - 54 Aluno não existe
 */

  Serial.println(response);
  //Como a resposta vem em ASCII, converte o codigo decimal que indica tudo ok para ASCII 
  if(response =='2'){
    //envia mensagem que foi autorizado
    lcd.clear();
    lcd.home();
    lcd.print("Autorizado");
    //envia sinal sonoro
    tone(pinBuzzer,929,200);
    
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
      contDigitos = 0;
      cpf = "";
      flagLCD=0;

    }
  }
  else if(response == '4'){

    //avisa que aluno não está autorizado
    lcd.clear();
    lcd.home();
    lcd.print("Nao autorizado");
    lcd.setCursor(0,1);
    lcd.print("Informe a CGAE");
    delay(4000);
    //reinicia todas as flags e variaveis
    contDigitos = 0;
    cpf = "";
    flagLCD=0;

  }
  else if(response == '5'){

    //avisa que aluno não existe

    lcd.clear();
    lcd.home();
    lcd.print("Dados invalidos");
    delay(2000);
    //reinicia todas as flags e variaveis
    contDigitos = 0;
    cpf = "";
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
    contDigitos = 0;
    cpf = "";
    flagLCD=0;
  }
}
