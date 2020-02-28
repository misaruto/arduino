/**
 * @todo testar
 * @todo tratar a resposta do mysql
 * @todo tentar um mysql injection
 * @todo tentar um cross site scripting
 */

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
function handleForIncommingRequests(data, callback)
{
    if (!callback) error({fatal:true, error:"Missing callback on handleForIncommingRequests", code:3 })
    if (!data)
    {
        error ({err:"missing Args", code:1}, data);
        return;
    }

    parseRequest(data)
                .then(async function (cpf, token)
                {
                    let ans;
                    if (ans = await verifyRequest(cpf) < 0)
                    {
                        if(ans == -2){
                            error({err:"Missing args", code:1,data:`${cpf} ${token}`});
                            return -1;
                        }
                        error({err:"Forbidden characteres founded!", code:0,data:`${cpf} ${token}`});
                        return -1;
                    }
                if (await verifyRequest(token) < 0) { 
                    if(ans == -2){
                        error({err:"Missing args", code:1,data:`${cpf} ${token}`});
                        return -1;
                    }
                    error({err:"Forbidden characteres founded!", code:0, data:`${cpf} ${token}`});
                    return -1; }
                    const res = callDB(cpf, token);
                })
                .catch((err) =>
                {
                    error(err);
                    return -1;
                });
}
// Faz o parse da requisição, recuperando todos os dados devidamente
function parseRequest(data)
{
    return new Promise((accept, reject) =>
    {
        
        if ( !data || !data.indexOf(";") ) reject({err:"Missing args", code:1, data});
        const d = data.split(";");
        accept(d[0], d[1]);
    })
}
// Verifica se não há caracteres inválidos, e se todos os argumentos estão corretos

function verifyRequest(content)
{
    if (!content) return -2;
    const leng = content.length;
    for (let i = 0; i<leng; i++)
        if (content.substr(i, 1)  < "0" || content.substr(i, 1) > "9" ) return -1;
    return 1;
}
function error(err, data)
{
    const err_txt = `${err.fatal ? 'Fatal' : 'Warning'}: ${err.err} ${err.code} ${Date.now()} ${data}\n`
    console.error(err_txt);
    fs.writeFileSync("crash.txt", err_txt)
    if(err.fatal) process.kill(process.pid);
}

// Chama o banco de dados para fazer a validação
function callDB(cpf, token)
{
    configs.database.querry.format(cpg, token)
    con.connect(function(err) {
        if (err) error();
        con.query(querry, function (err, result, fields) {
          if (err) throw err;
          console.log(result);
        });
      });
}



// Interface externa que invocará estes métodos
module.exports = 
{
    handleForIncommingRequests,
    error,
}

/**
 * Erros:
 *      0 - Forbiden character
 *      1 - Missing args
 *      3 - Missing callback(fatal)
 */