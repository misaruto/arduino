#ifndef CONEXAO_H
#define CONEXAO_H

//Include padrão para usar funções do arduino
#include <Arduino.h>

//includes para variaveis do tipo uint16_t e uint8_t
#include <Stdint.h>
#include <inttypes.h>

//Include da biblioteca que faz a conexão com a internet
//#### Necessária a inclusão da bibliotece SPI.h no seu arquivo .ino
#include <UIPEthernet.h>

//include String
#include <String.h>

//include da interface SPI
#include <SPI.h>

private:
    EthernetClient Client;
    uint8_t *mac[];
    uint8_t *ip[];
    uint16_t *port;
    uint8_t identficador;

public:
    Conexao(uint8_t &mac[], uint8_t &ip[], uint16_t &port);
    bool inicarModulo();
    bool conectar(uint8_t & identificador);
    bool autenticar(uint8_t & identificador);
    bool autenticar();
    void printConexStatus();
    uint8_t converterStrUint8(String & msg);
    bool tcpWrite(String msg);
    uint8_t tcpRead();

#endif