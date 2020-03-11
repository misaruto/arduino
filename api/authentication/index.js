/**
 * API que faz a autenticação e cria os registros no banco de dados, relacionado as catracas
 * @author: Davidson Lucas de Souza (GitHub: https://github.com/Davidson-Souza )
 * @git: https://github.com/misaruto/arduino
 */
// Importando os módulos
const tcp_server = require("./tcp_socket");     // TCP server connect
const methods    = require("./methods");        // All globals methods used on this project

// Inicializando o servidor
methods.create_log("Starting server...");       // Avisa no log que o servidor está sendo inicializado
methods.init(tcp_server);                       // Inicializa tudo