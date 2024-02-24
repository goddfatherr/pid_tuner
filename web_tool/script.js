const sliderContainer = document.getElementById('slider-container');
const statusDisplay = document.getElementById('status');
const sendButton = document.getElementById('send-button');

let ws; // WebSocket connection

// Update value display on slider change
const sliders = sliderContainer.querySelectorAll('.slider');
sliders.forEach(slider => {
  const valueSpan = slider.parentElement.querySelector('span');
  valueSpan.textContent = slider.value;

  slider.addEventListener('input', () => {
    valueSpan.textContent = slider.value;
  });
});

// Send button click handler
sendButton.addEventListener('click', () => {
  if (!ws || ws.readyState !== WebSocket.OPEN) {
    alert('Please connect to the server first!');
    return;
  }

  const kp = document.getElementById('kp').value;
  const ki = document.getElementById('ki').value;
  const kd = document.getElementById('kd').value;

  const data = `${kp} ${ki} ${kd}`;
  ws.send(data);
});

// WebSocket connection and status management
function connectWebSocket() {
  ws = new WebSocket("ws://172.22.202.69:80/ws");


  ws.addEventListener('open', () => {
    statusDisplay.textContent = 'Connected';
  });

  ws.addEventListener('close', () => {
    statusDisplay.textContent = 'Disconnected';
  });
}

connectWebSocket(); // Initiate connection on page load
