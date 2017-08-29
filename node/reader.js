var IPC = require('./build/Release/IPC');

var recived = 0;

var conn = new IPC.Connection("ipcnodedemo", 1);

function waituntil(num, final){
  if (recived < num){
    setTimeout(function() { waituntil(num, final); }, 500);
  }else {
    final()
  }
}

function cleanup(){
  conn.stopAutoDispatch();
  conn.close();
  conn.destroy();
}


conn.setCallback(msg => {
  console.log(msg.getData().toString());
  recived++;
});

conn.startAutoDispatch();

waituntil(3, cleanup);
