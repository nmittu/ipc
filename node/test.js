var IPC = require('./build/Release/IPC');

var msg = new IPC.Message(Buffer.from("Test123"));
msg.setSubject("hello");
console.log(msg.getData().toString());
