/*-----------------------------------------------------------------------------------*/
// Lista com todos as mensagens de erro (para os logs)
const errors = [
    "Forbidden characteres founded!",
    "Missing args",
    "Missing callback",
    "Invalid CPF", 
    "Wrong querry",
    "Autentication failed",
    "Error on connect to mysql"
];
/*-----------------------------------------------------------------------------------*/
// Requisitando os módulos
const mysql   = require('mysql');           // Módulo para conectar com o banco de dados
const configs = require("./configs");       // Configurações gerais
const fs      = require("fs");              // File system, para persistir logs

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
let ticketGates = [];        // Lista com as catracas permitidas
let connections = [];        // Array com todas as conexões
/*-----------------------------------------------------------------------------------*/
// Callback chamada quando o socket receber uma mensagem, ela tomará as ações nescessárias
function handleForIncommingRequests(socket)
{
    // Verificação para caso de erro interno (Bizarro por sinal!)
   if(!socket) create_log("", {code:2, fatal:true});
    // Oque será feito quando a conexão for aberta
    socket.on("connect", () =>
    {
        this.autenticated = false;                  // Atributo que dirá se a catraca está autorisada
        this.index        = 0;                      // Atributo que guardará o id da catraca em connections
    });
    socket.on("close",() =>
    {
        create_log("Client disconnected ");
    })
    // O que será feito quando uma mensagem chegar do cliente
    socket.on("data", async function (data)
    {
        // Verifica se ela está logada
        if (this.autenticated)                                              // Se sim, faça o seguinte:
        {
            create_log(`${data.toString()}`);                               // Cria um log com a ação
            let res = 0;                                                    // Variável temporária que guardará a resposta para o cliente
            var request = data.toString();                                  // Recupera a requisição
            res = await process_request(request, this.index);               // Faz todo o processamento e guarda isso em uma variável
            this.write(`${ res >= 0 ? res : 4 }`);                          // Envia a resposta para o cliente, o protocolo é descrito à seguirs
        }
        else                                                                 // Caso contrário, faça isso: 
        {
            create_log("New ticketGate:" + data.toString());                 // Cria um log com a ação
            res = await authenticateTicketGate(data.toString());             // Tenta autenticar a catraca
            await socket.write(`${res >= 0? 2:-1}`);                         // Devolve o resultado para o cliente
            this.index = res >= 0 ? res : null;                              // Se a catraca foi autorizada, salve o índice dela...
            this.autenticated = res >= 0 ? true : false;                     // ... e diga que ela está autorizada
            if (res < 0){ socket.end();  create_log("Autentication Faled!") }// Se não foi autorizada, encerra a conexão
            else { create_log("Authenticated Successful")}
        }
    });                                                 
    socket.on("error", create_log)                              // Caso dê erro com a conexão, jogue isto no log
}

// Função que autentica a catraca no início da conexão
function authenticateTicketGate(auth_token)
{
    if (ticketGates.indexOf(auth_token)>=0){ connections.push({auth_token, isConnected:true, state:0, autenticated:null }); return connections.length -1; }
    else { create_log (auth_token, {code:5, fatal:false}); return -1; }
}
// Função que processa a requisição
async function process_request(request, index)
{
    let res;
    // Verifica se foi passado um dado
    if (!request)
    {
        create_log(requst, {code:1, fatal:false});
        return -1;
    }
    if (request.indexOf(";") < 0)
    {
        create_log(request, {code:4, fatal:false});
        return -1;
    }
    else
    {
        let opcode = request.split(";")[0];
        if (opcode == 0) {
            res = await tryngAuthentication(request.split(";")[1], connections[index].autenticated); 
            if (res == 2 ){ connections[index].autenticated = request.split(";")[1]; return 2; }
            else return res;
        }
       
        else if (opcode == 1 && connections[index].autenticated) { 
            res = await processEntry(request.split(";")[1], connections[index].autenticated, connections[index].auth_token);
            if (res == 2){ connections[index].autenticated = false; return 2; }
            else return res;
        }
        else 
        {
            create_log(request, {code:4, fatal:false});
            return -1;
        }
    }

}

/**
 * Stores on the database the moviment of one strudent through the ticketgate.
 * @param {int} act Direction of the moviment 
 * @param {string} auth The code of the student
 * @param {string} ticketate The id of the ticket gate
 */

// Processa a entrada de um aluno(a direção)
function processEntry(act, auth, ticketgate)
{
    if (!act || !auth){ create_log(act, {fatal:true, err:1}); return -1; }

    const query = configs.database.updt.replace("$", auth)
                                       .replace("$", ticketgate)
                                       .replace("$", act);
    return callDB(query)
            .then((data) =>
            {
                if (data){ return 2; }
                else { return -1; }
            })
}
// Tenta autenticar o aluno
async function tryngAuthentication(request, index)
{
    let ans;                                            // Resposta da função
    ans = await verifyRequest(request)                  // Executa o método que verifica se existem caracteres proibidos

    if (ans < 0)                                        // Verifica se possui caracteres proibídos
    {
        
        // Trata o erro
        if(ans == -2){
            create_log(request, {code:1, fatal:false}); // Não foi recebido parâmetro (ou está vazio)
            return -1;
        }
        create_log(request, {code:0, fatal:false});     // Caracter inválido recebido
        return -1;
    }

    ans = await isAnCPF(request);                       // Verifica se é um CPF válido

    if (ans < 0){ create_log(request, { code:3, fatal:false} ); return -1; }
    const query = configs.database.auth.replace("$", request);
    return callDB(query)                                // Chama o banco de dados
                .then((result) => 
                    {
                        if (result.length == 0) { return 0; }
                        if (result.length > 0){
                            if (result[0].bolsaAtiva == 1) return 2;
                            if (result[0].bolsaAtiva == 0) return 1;
                        }
                        return -1;
                    });
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
        if ((content.substr(i, 1)  <= "0"-1 || content.substr(i, 1) >= "9"+1)){ return -1; }
    return 1;
}

// Função que cria um log
function create_log(data, err)
{
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
        const msg = `Note: ${data}\n`;
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
                if (err) { create_log("Mysql: " + err); return -1 };
                accept(result, fields);
            })
        });
    });
}
async function init(tcpServer){
    await getAlowedTicketGate();                   // Busca a lista de catracas permitidas
    tcpServer.create(handleForIncommingRequests);  // Cria um servidor
    tcpServer.onerror = create_log;                // Passa create_log como callback para erros no server
    tcpServer.listen();                            // Ouve por requisições
}

// Busca no banco de dados, a lista de todas as catracas com permissão
function getAlowedTicketGate()
{
    callDB(configs.database.ticketGate)
        .then((data) =>
        {
            if (data.length > 0)  
                for(let i = 0; i<data.length; i++)
                    ticketGates.push (`${ data[i].validador }`) ;
        });
}

// Interface externa que invocará estes métodos
module.exports = 
{
    init,
    create_log,
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
 *      6 - Fail to connect to mysql (on start)(fatal)
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
