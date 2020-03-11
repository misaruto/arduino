#include "Conexao.h"

//construtor
Conexao::Conexao(uint8_t& mac[],uint8_t & ip[],uint16_t & port){
    EthernetClient client;
    this->mac = *mac;
    this->ip = *ip;
    this->port = *port;
    this->identificador = "";
} 
//Função que inicia o modulo
bool Conexao::iniciarModulo(){
    Spi.begin();
     if(Ethernet.begin(this->mac)){
        return true;
    }
    else{
        return false;
    }
}

bool Conexao::conectar(){
    if(this->client.connect(IPAddress(this->ip[0],this->ip[1],this->ip[2],this->ip[3]),this->port)){
        return true;
    }
    else{
        return false;
    }
}

bool Conexao::autenticar(uint8_t & identificador){
    this->client.write(identificador);
    while (this->client.available() >= 0){
        if (this->client.available > 0){
            if (this->client.read() == 2){
                this->identificador = identificador;
                return true;
            }
            else{
                return false;
            }
        }
    }
}

bool Conexao::autenticar(){
    if (this.idetificador != ""){
        this->client.write(identificador);
        while (this->client.available() >= 0){
            if (this->client.available > 0){
                if (this->client.read() == 2){
                    this->identificador = identificador;
                    return true;
                }
                else{
                    return false;
                }
            }
        }
    }
    else{
        return false
    }   
}

uint8_t Conexao::converterStrUint8(String & msg){
     uint8_t txt[];
     msg.toCharArray(txt,msg.length +1);
     return txt;
 }

bool Conexao::tcpWrite(String msg){
    //Vetor que armazena a mensagem que sera comunicada
    uint8_t mensagem[];
    //variavel que armazena o tamanho da mensagem
    size_t taomanho = msg.length;
    //converte a mensagem de string para uint8_t
    mensagem = this->converterStrUint8(msg);
    //escreve a mensagem para servidor tcp
    if(this->client.write(mensagem,tamanho)){
        return true;
    }
    else{
        return false;
    }
}
uint8_t Conexao::tcpRead(){
    while (this->client.available() >= 0){
        if (this->client.available > 0){
            return this->client.read();
        }
        else{
            return false;
        }
    }
}