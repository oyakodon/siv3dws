const WebSocket = require('ws');
const http = require('http');
const express = require('express');
// const fs = require('fs');
// const https = require('https');

const port = 8080;

// const options = { 
//   key : fs.readFileSync('key.pem'),
//   cert: fs.readFileSync('cert.pem')
// };
// const https_server = https.createServer(options, app);

const app = express();
app.use('/', express.static(__dirname + '/public'));
const http_server = http.createServer(app);
const ws_server = new WebSocket.Server({ noServer: true });

http_server.on('upgrade', (request, socket, head) =>
{
  ws_server.handleUpgrade(request, socket, head, (ws) =>
  {
    ws_server.emit('connection', ws, request);
  });
});

ws_server.on('connection', (socket) =>
{
  console.log('[WS] connected.');

  socket.on('close', (code, reason) =>
  {
    console.log('[WS] disconnected.');
  });

  socket.on('message', (message) =>
  {
    // メッセージを受信
    console.log('[WS] Received: ' + message);
    
    // 送信元以外のクライアント全てにメッセージを送信
    ws_server.clients.forEach((client) =>
    {
      if (client.readyState == WebSocket.OPEN && client != socket)
      {
        client.send(message);
      }
    });

  });

});

http_server.listen(port, () =>
{
  console.log('Listening on http://localhost:' + port);
});

// https_server.listen(port_https, () =>
// {
//   console.log('Listening on https://localhost:' + port);
// });
