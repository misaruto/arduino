const mysql = require("mysql");
const configs = require("./configs.json");
// Cria a conexão com o mysql
var con = mysql.createConnection({
  host: configs.database.host,
  user: configs.database.user,
  password: configs.database.password,
  database: configs.database.database
});
callDB("123456")
			.then((data)=>{ console.log(data)})


