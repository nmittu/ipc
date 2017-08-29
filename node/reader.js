var IPC = require('./build/Release/IPC');

var recived = 0;

var conn = new IPC.Connection("ipcnodedemo", 1);

conn.setCallback(msg => {
  console.log(msg.getData().toString());
  recived++;
});

conn.startAutoDispatch();

while (recived < 3){}

conn.stopAutoDispatch();
conn.close();
conn.destroy();
