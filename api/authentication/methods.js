/**
 * @todo testar
 * @todo tentar um cross site scripting
 * @todo terminar a querry com o db
 */
const errors = [
    "Forbidden characteres founded!",
    "Missing args",
    "Missing callback"
];
var mysql = require('mysql');           // Módulo para conectar com o banco de dados
const configs = require("./configs");   // Configurações gerais
const fs = require("fs");               // File system, para persistir logs

// Cria a conexão com o mysql
var con = mysql.createConnection({
  host: configs.database.host,
  user: configs.database.user,
  password: configs.database.password,
  database: configs.database.database
});


// Callback chamada quando o socket receber uma mensagem, ela tomará as ações nescessárias
function handleForIncommingRequests(socket)
{
    // Verificação para caso de erro interno (Bizarro por sinal!)
    if(!socket) create_log("", {code:2, fatal:true});

    // O que será feito quando uma mensagem chegar do cliente
    socket.on("data",async function (data)
    {
        create_log(`Uncomming message from client: ${data.toString()}`); // Cria um log com a ação
        var res = 0; 
        res = await process_request(data.toString());                    // Faz todo o processamento e guarda isso em uma variável
        await socket.write(`${ res >= 0 ? res : 4 }`);                   // Envia a resposta para o cliente, o protocolo é descrito à seguir
    });                                                                  // Caso a resposta seja negativa, isto indica erro interno
    socket.on("error", create_log)
}
// Função que processa a requisição
async function process_request(request)
{
    // Verifica se foi passado um dado
    if (!request)
    {
        create_log(requst, {code:1, fatal:false});
        return -1;
    }
    let ans;                        // Resposta da função
    // Verifica se é um CPF válido
    ans = await isAnCPF(request);
    if (ans < 0){ create_log(request, { code:1, fatal:false} ); return 0; }
    ans = await verifyRequest(request)  // Executa o método que verifica se existem caracteres proibidos
    if (ans < 0)                        // Verifica se possui caracteres proibídos
    {
        // Trata o erro
        if(ans == -2){
            create_log(request, {code:1, fatal:false});
            return -1;
        }
        create_log(request, {code:0, fatal:false});
        return -1;
    }
    return callDB(request)                 // Retorna a resposta do banco de dados
                .then((data) => 
                    {
                        return data;
                    });
}
// Verifica se é um cpf válido
function isAnCPF(cpf){}

// Verifica se não há caracteres inválidos, e se todos os argumentos estão corretos
function verifyRequest(content)
{
    if (!content) return -2;
    const leng = content.length;
    // Verifica se o conteúdo possui caracteres que não são numéricos
    for (let i = 0; i<leng; i++)
        if ((content.substr(i, 1)  <= "0" || content.substr(i, 1) >= "9") && content.substr(i, 1) == ""){ return -1; }
    return 1;
}
// Função que cria um log
function create_log(data, err)
{
    if (err)
    {
        const err_txt = `${err.fatal ? 'Fatal' : 'Warning'}: ${err.code} ${errors[err.code]} ${Date.now()} ${data}\n`
        console.error(err_txt);
        fs.writeFileSync("crash.txt", err_txt)
        if(err.fatal) process.kill(process.pid);
    }
    else
    {
        const msg = `Incoming message receiving: ${data}\n`;
        console.error(msg);
        fs.writeFileSync("crash.txt", msg)
    }
}

// Chama o banco de dados para fazer a validação
function callDB(cpf, callback){
    const query = configs.database.querry.replace("$", cpf);
    return new Promise((accept, reject) =>
    {
        con.connect(function (err)
        {
            console.log(query);
            con.query(query, function (err, result, fields)
            {
                if (err) throw err;
                if (result.length == 0) { accept(0); }
                if (result.length > 0){
                    if (result[0].bolsaAtiva == 1) accept(2);
                    if (result[0].bolsaAtiva == 0) accept(1);
                }
                accept(-1);
            })
        });
    })
}


// Interface externa que invocará estes métodos
module.exports = 
{
    handleForIncommingRequests,
}
/**
 * Respostas para o cliente:
 *      0 - Usuário não existe
 *      1 - Usuário existe mas não tem permissão para entrar
 *      2 - Usuário existe e está permitido à entrar
        4 - Erro interno
 */
/**
 * Erros:
 *      0 - Forbiden character
 *      1 - Missing args
 *      2 - Missing callback(fatal)
 */