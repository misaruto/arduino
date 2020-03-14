//Include interface SPI
//Usada tanto pelo UIPEthernet quanto pelo MRC522
//            #####explicando######
/*Os dois modulos se conectam nas mesmas portas, porem tem
  2 pinos especificos para cada um, que é o de conexão do modulo com
  o arduino, no UIPEthernet é o 51 e no MFRC522 é o 35 */


//Includes e variaveis para conexão com internet
#include <UIPEthernet.h>

EthernetClient client;
signed long next;
//Define um mac para a interface de internet
uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};
//Variaveis para conexão
const int ip[4] = {192, 168, 1, 144};
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
MFRC522::StatusCode status;
 byte blockAddr      = 4;
 byte buffer[18];
 byte size = sizeof(buffer);
 byte sector         = 1;
     
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
String cpf = "";
uint8_t giro[2];
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

//declara função que realiza a conexão com o servidor
void conectaServidor(); 