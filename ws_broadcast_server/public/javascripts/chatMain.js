const send = (msg) =>
{
  if (msg && ws && ws.readyState == WebSocket.OPEN)
  {
    ws.send(msg);
  }
};

const prependMessage = (payload) =>
{
  if (payload && payload.name)
  {
    if (payload.text)
    {
      $("#messages").prepend(payload.name + ": " + payload.text +"<br />");
    } else if (payload.image)
    {
      $("#messages").prepend(payload.name + ": (画像のみ)<br />");
    }
  }
};

$('#btn-send').on('click', () =>
{
  name = $("#name").val();
  text = $("#message").val();

  if (name && text)
  {
    const payload = { "type": "message", "name": name, "text": text };
    send(JSON.stringify(payload));
    prependMessage(payload);

    $("#message").val("");
  }
});

var ws = new WebSocket('ws://192.168.11.2:8080/');

ws.addEventListener('open', (event) =>
{
  console.log("connected.");
});

ws.addEventListener('message', (event) =>
{
  const message = JSON.parse(event.data);
  console.log(message.type);
  if (message.type)
  {
    switch(message.type)
    {
      case "join": $("#messages").prepend("< " + message.name + "さんが参加しました ><br />"); break;
      case "leave": $("#messages").prepend("< " + message.name + "さんが退出しました ><br />"); break;
      case "message": prependMessage(message); break;
    }
  }

});
