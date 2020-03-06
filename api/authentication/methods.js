/**
 * @todo testar
 * @todo tentar um cross site scripting
 * @todo terminar a querry com o db
 */

/*-----------------------------------------------------------------------------------*/
// Lista com todos as mensagens de erro (para os logs)
const errors = [
    "Forbidden characteres founded!",
    "Missing args",
    "Missing callback",
    "Invalid CPF", 
    "Wrong querry",
    "Autentication failed"
];
/*-----------------------------------------------------------------------------------*/
// Requisitando os módulos
var mysql = require('mysql');           // Módulo para conectar com o banco de dados
const configs = require("./configs");   // Configurações gerais
const fs = require("fs");               // File system, para persistir logs

/*-----------------------------------------------------------------------------------*/
// Cria a conexão com o mysql
var con = mysql.createConnection({
  host: configs.database.host,
  user: configs.database.user,
  password: configs.database.password,
  database: configs.database.database
});
/*-----------------------------------------------------------------------------------*/
// Constantes globais
const ticketGates = [];             // Lista com as catracas permitidas
const connections = [];             // Array com todas as conexões
/*-----------------------------------------------------------------------------------*/
// Callback chamada quando o socket receber uma mensagem, ela tomará as ações nescessárias
function handleForIncommingRequests(socket)
{
    // Verificação para caso de erro interno (Bizarro por sinal!)
    if(!socket) create_log("", {code:2, fatal:true});
    socket.on("connect", () =>
    {
        this.autenticated = false;
        this.index        = 0;
    });
    // O que será feito quando uma mensagem chegar do cliente
    socket.on("data",async function (data)
    {
        if (this.autenticated)
        {
            create_log(`Uncomming message from client: ${data.toString()}`); // Cria um log com a ação
            var res = 0; 
            var teste = data.toString();
            if (teste.length >= 11) { 
                res = await process_request(data.toString());                    // Faz todo o processamento e guarda isso em uma variável
            }
            await socket.write(`${ res >= 0 ? res : 4 }`);                       // Envia a resposta para o cliente, o protocolo é descrito à seguir
        }
        else 
        {
            res = await authenticateTicketGate(data.toString());  
            this.index = res >= 1? res:null;
            socket.end( );
            return 0;
        }
    });                                                 
    socket.on("error", create_log)
}

// Função que autentica a catraca no início da conexão
function authenticateTicketGate(auth_token)
{
    if (ticketGates.indexOf(auth_token)>=0){ connections.push({auth_token, isConnected:true, state:0, autenticated:null }); return connections.length -1; }
    else { create_log (auth_token, {code:5, fatal:false}); return 3; }
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
    if (request.indexOf(";")<0)
    {
        create_log(request, {code:4, fatal:false});
        return -1;
    }
    let opcode = request.split(";")[0];
    if (opcode == 0) {return tryngAuthentication(request.split(";")[1])}
    else if (opcode == 1) { return processEntry(); }

    else 
    {
        create_log(request, {code:4, fatal:false});
        return -1;
    }

}
function processEntry(act)
{
    if (!act){ create_log(act, {fatal:true, err:1}); return -1; }
    const query = configs.database.updt.replace("$", auth)
                                       .replace("$", act);
    return callDB(query)
            .then((data) =>
            {
                return data;
            })
}
async function tryngAuthentication(request)
{
    let ans;                        // Resposta da função

    // Verifica se existem caracteres inválidos
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

    // Verifica se é um CPF válido
    ans = await isAnCPF(request);

    if (ans < 0){ create_log(request, { code:3, fatal:false} ); return -1; }
    const query = configs.database.auth.replace("$", request);
    return callDB(query)                 // Retorna a resposta do banco de dados
                .then((data) => 
                    {
                        return data;
                    });
                   // Verifica se existem caracteres inválidos
}
// Verifica se é um cpf válido
function isAnCPF(cpf){
    // Elimina CPFs invalidos conhecidos	
	if (cpf.length != 11 || 
		cpf == "00000000000" || 
		cpf == "11111111111" || 
		cpf == "22222222222" || 
		cpf == "33333333333" || 
		cpf == "44444444444" || 
		cpf == "55555555555" || 
		cpf == "66666666666" || 
		cpf == "77777777777" || 
		cpf == "88888888888" || 
		cpf == "99999999999") return -1;
	// Valida 1o digito	
	add = 0;	
	for (i=0; i < 9; i ++)		
		add += parseInt(cpf.charAt(i)) * (10 - i);	
		rev = 11 - (add % 11);	
		if (rev == 10 || rev == 11)		
			rev = 0;	
        if (rev != parseInt(cpf.charAt(9))) return -1;	
	// Valida 2o digito	
	add = 0;	
	for (i = 0; i < 10; i ++)		
		add += parseInt(cpf.charAt(i)) * (11 - i);	
	rev = 11 - (add % 11);	
	if (rev == 10 || rev == 11)	
		rev = 0;	
    if (rev != parseInt(cpf.charAt(10))) return -1;
				
	return 1;   
}

// Verifica se não há caracteres inválidos, e se todos os argumentos estão corretos
function verifyRequest(content)
{
    if (!content) return -2;
    const leng = content.length;
    // Verifica se o conteúdo possui caracteres que não são numéricos
    for (let i = 0; i<leng; i++)
        if ((content.substr(i, 1)  <= "0"-1 || content.substr(i, 1) >= "9"+1)){ console.log(content.substr(i, 1));return -1; }
    return 1;
}

// Função que cria um log
function create_log(data, err)
{
    console.log(data)
    if (err)
    {
        const err_txt = `${err.fatal ? 'Fatal' : 'Warning'}: ${err.code} ${errors[err.code]} ${Date.now()} ${data}\n`
        console.error(err_txt);
        fs.appendFile(configs.logfile, err_txt, (err) => {
            if (err) throw err;
        });
        if(err.fatal) process.kill(process.pid);
    }
    else
    {
        const msg = `Incoming message receiving: ${data}\n`;
        console.error(msg);
        fs.appendFile(configs.logfile, msg, (err) => {
            if (err) throw err;
        });
    }
}

// Chama o banco de dados para fazer a validação
function callDB(query){  

    return new Promise((accept, reject) =>
    {
        con.connect(function (err)
        {
            con.query(query, function (err, result, fields)
            {
                console.log("mysql");
                if (err) { create_log("Mysql: " + err); return -1 };
                if (result.length == 0) { accept(0); }
                if (result.length > 0){
                    if (result[0].bolsaAtiva == 1) accept(2);
                    if (result[0].bolsaAtiva == 0) accept(1);
                }
                accept(-1);
            })
        });
    });
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
 *      2 - Usuário existe e está permitido à entrar (Ou a catraca foi liberada, durante a conexão)
 *      3 - Erro ao autenticar a catraca (no processo de inicialização do servidor)
 *      4 - Erro interno
 */
/**
 * Erros:
 *      0 - Forbiden character
 *      1 - Missing args
 *      2 - Missing callback(fatal)
 *      3 - Invalid CPF given
 *      4 - Wrong query
 *      5 - Auth error (on connection)
*/
/**
 * Opcodes de requisição:
 *      0 - Permitir entrada
 *      1 - Informar entrada
 * Possíveis respostas:
 *      0 - Erro inerno
 *      2 - Ok!
 * Formato da query: opcode;cpf
 */
