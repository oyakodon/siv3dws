var ws = new WebSocket('ws://192.168.11.2:8080/');

ws.addEventListener('open', (event) =>
{
  console.log("connected.");
});

ws.addEventListener('message', (event) =>
{
  console.log(event.data);
  $('#log').append(event.data + "<br />");
});

const send = (msg) =>
{
  if (ws && ws.readyState == WebSocket.OPEN)
  {
    ws.send(msg);
  }
}

$("#btn-send").on('click', () =>
{
  text = $("#text").val();

  if (text)
  {
    send(text);
  }
});
