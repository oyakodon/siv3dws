const send = (msg)=>
{
  if (ws && ws.readyState == WebSocket.OPEN)
  {
    ws.send(msg);
  }
};

let origin = { "x": 0, "y": 0 };
let touched = false;
const isTouchDevice = ('ontouchstart' in window);

$('#ctrlPad').on('touchstart mousedown', (e) =>
{
  e.preventDefault();
  origin.x = isTouchDevice ? e.changedTouches[0].pageX : e.pageX;
  origin.y = isTouchDevice ? e.changedTouches[0].pageY : e.pageY;
  touched = true;
});

$('#ctrlPad').on('touchmove mousemove', (e) =>
{
  if (!touched) return;

  e.preventDefault();
  x = isTouchDevice ? e.changedTouches[0].pageX : e.pageX;
  y = isTouchDevice ? e.changedTouches[0].pageY : e.pageY;

  let diff = { "x": x - origin.x, "y": y - origin.y };

  $('#updated').text(JSON.stringify(diff));
  send(JSON.stringify(diff));

});

// $('#ctrlPad').on('touchend mouseup', (e) =>
// {
//   if (!touched) return;
//   touched = false;
// });

$(document).on('touchend mouseup', (event) =>
{
  if (!$(event.target).closest('#target').length)
  {
    if (!touched) return;
    touched = false;
  }
});

var ws = new WebSocket('ws://192.168.11.2:8080/');

ws.addEventListener('open', (event) =>
{
  console.log("connected.");
});

ws.addEventListener('message', (event) =>
{
  // $('#log').append(event.data + '<br />');
  console.log(event.data);
});
