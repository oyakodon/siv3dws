const startCaptureMotion = () =>
{
  if (DeviceMotionEvent)
  {
    if (typeof DeviceMotionEvent.requestDeviceMotionPermission === 'function')
    {
      // iOS Safariとか
      requestDeviceMotionPermission();
    } else
    {
      // Android等？
      window.addEventListener('devicemotion', motionEventHandler);
    }
  } else
  {
    alert("DeviceMotionEvent 未対応デバイスです。");
  }
};

const send = (msg)=>
{
  if (ws && ws.readyState == WebSocket.OPEN)
  {
    ws.send(msg);
  }
};

const motionEventHandler = (e) =>
{
  let x = event.accelerationIncludingGravity.x;
  let y = event.accelerationIncludingGravity.y;
  let z = event.accelerationIncludingGravity.z;

  let data = { "x": x, "y": y, "z": z };
  $('#updated').text(JSON.stringify(data));
  send(data);
};

const requestDeviceMotionPermission = () =>
{
  DeviceMotionEvent.requestPermission().then((permissionState) =>
  {
    if (permissionState === 'granted')
    {
      window.addEventListener('devicemotion', motionEventHandler);
    } else
    {
      alert("キャンセルされました。");
    }

  }).catch((e) =>
  {
    alert(e);
  });
}

$('#btn-req').on('click', () =>
{
  startCaptureMotion();
});

$('#btn-stop').on('click', () =>
{
  window.removeEventListener('devicemotion', motionEventHandler);
});

var ws = new WebSocket('ws://192.168.11.2:8080/');

ws.addEventListener('open', (event) =>
{
  console.log("connected.");
});

ws.addEventListener('message', (event) =>
{
  $('#log').append(event.data + '<br />');
});
