const mysql = require("mysql");
const tcp_server = require("./tcp_socket");
tcp_server.create((socket)=>{console.log("criou")});
tcp_server.listen();
// Callback chamada quando o socket receber uma mensagem, ela tomará as ações nescessárias
function handleForIncommingRequests()
{
    
}

// Faz o parse da requisição, recuperando todos os dados devidamente
function parseRequest()
{

}
// Verifica se não há caracteres inválidos, e se todos os argumentos estão corretos
function verifyRequest()
{

}

// Chama o banco de dados para fazer a validação
function callDB()
{

}
