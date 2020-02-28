const mysql      = require("mysql");
const tcp_server = require("./tcp_socket");
const methods    = require("./methods");

tcp_server.create(methods.handleForIncommingRequests);
tcp_server.onerror = methods.error;
tcp_server.listen();