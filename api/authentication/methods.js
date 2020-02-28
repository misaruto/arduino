/**
 * @todo tratar os erros
 * @todo testar o mysql
 */

var mysql = require('mysql');
const configs = require("./configs");
const fs = require("fs");

// Conecta ao mysql

var con = mysql.createConnection({
  host: configs.database.host,
  user: configs.database.user,
  password: configs.database.password,
  database: configs.database.database
});


// Interface externa que invocará estes métodos
module.exports = 
{
    handleForIncommingRequests,
}

// Callback chamada quando o socket receber uma mensagem, ela tomará as ações nescessárias
function handleForIncommingRequests(data, callback)
{
    if (!callback) throw new Error("Fatal: Missing callback function at handleForIncommingRequest!");
    if (!data)
    {
        error ({err:"missing Args", code:1}, data); callback({err:"Missing Args", code:1});
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
                            callback({err:"Forbidden characteres founded!", code:0});
                            return -1;
                        }
                        error({err:"Forbidden characteres founded!", code:0,data:`${cpf} ${token}`});
                        callback({err:"Forbidden characteres founded!", code:0});
                        return -1;
                    }
                    if (await verifyRequest(token) < 0)
                    {
                        error({err:"Forbidden characteres founded!", code:0, data:`${cpf} ${token}`});
                        callback({err:"Forbidden characteres founded!", code:0});
                        return -1;
                    }
                    callDB(cpf, token);
                })
                .catch((err) =>
                {
                    error(err);
                    callback(err);
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
    const err_txt = `CRASH:${err.msg} ${err.code} ${Date.now()} ${data}\n`
    console.error(err_txt);
    fs.writeFileSync("crash.txt", err_txt)
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




/**
 * Erros:
 *      0 - Forbiden character
 *      1 - Missing args
 * 
 */