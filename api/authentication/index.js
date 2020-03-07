/**
 * API que faz a autenticação e cria os registros no banco de dados, relacionado as catracas
 * @author: Davidson Lucas de Souza (GitHub: https://github.com/Davidson-Souza )
 * @git: https://github.com/misaruto/arduino
 */
// Importing all modules
const tcp_server = require("./tcp_socket");     // TCP server connect
const methods    = require("./methods");        // All globals methods used on this project

// Main methods and creating server

methods.init(tcp_server);                       // Inicializa tudo