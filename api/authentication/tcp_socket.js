const net   = require("net");

module.exports = {
    server:null,
    onerror:null,
    create: handler =>
    {
        if(!handler && this.onerror) this.onerror("Missing handler");
        if(!handler && !this.onerror) throw new Error("Missing handler");

        this.server = net.createServer(handler);

        if(this.onerror) this.server.on("error", this.onerror);
    },
    listen: configurations => 
    {
        this.server.listen({
            host: 'localhost',
            port: 8080,
            exclusive: true
          }, () => 
          {
              console.log("Listeningo on port 8080...");
          })
    }
}