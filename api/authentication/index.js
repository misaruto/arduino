const mysql      = require("mysql");
const tcp_server = require("./tcp_socket");
const methods    = require("./methods");

methods.handleForIncommingRequests("123;213", (res) =>
{
    if (res) console.log(res);
})