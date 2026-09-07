const SERVICE_UUID = '4fafc201-1fb5-459e-8fcc-c5c9c331914b';
const COLOR_UUID = 'beb5483e-36e1-4688-b7f5-ea07361b26a8';

const picker = document.querySelector('#colorPicker');
const connectButton = document.querySelector('#connectButton');
const statusText = document.querySelector('#status');
const hexValue = document.querySelector('#hexValue');
const lamp = document.querySelector('#lamp');
let colorCharacteristic;
let sendTimer;

function setStatus(message, connected = false) {
  statusText.textContent = message;
  statusText.classList.toggle('connected', connected);
  picker.disabled = !connected;
  connectButton.textContent = connected ? 'Desconectar' : 'Conectar ao LED';
}

async function sendColor(color) {
  if (!colorCharacteristic) return;
  const payload = new TextEncoder().encode(color.toUpperCase());
  await colorCharacteristic.writeValueWithoutResponse(payload);
}

function selectColor(color) {
  picker.value = color;
  const red = parseInt(color.slice(1, 3), 16);
  const green = parseInt(color.slice(3, 5), 16);
  const blue = parseInt(color.slice(5, 7), 16);
  hexValue.value = `${color.toUpperCase()} · R: ${red} · G: ${green} · B: ${blue}`;
  lamp.style.setProperty('--selected', color);
  clearTimeout(sendTimer);
  sendTimer = setTimeout(() => sendColor(color).catch(showError), 60);
}

function disconnected() {
  colorCharacteristic = undefined;
  setStatus('Bluetooth desconectado');
}

function showError(error) {
  console.error(error);
  setStatus(error.message || 'Não foi possível conectar');
}

connectButton.addEventListener('click', async () => {
  if (colorCharacteristic) {
    colorCharacteristic.service.device.gatt.disconnect();
    return;
  }

  if (!navigator.bluetooth) {
    setStatus('Este navegador não oferece Web Bluetooth');
    return;
  }

  connectButton.disabled = true;
  setStatus('Procurando o LED…');
  try {
    const device = await navigator.bluetooth.requestDevice({
      filters: [{ name: 'LED RGB ESP32' }],
      optionalServices: [SERVICE_UUID]
    });
    device.addEventListener('gattserverdisconnected', disconnected);
    const server = await device.gatt.connect();
    const service = await server.getPrimaryService(SERVICE_UUID);
    colorCharacteristic = await service.getCharacteristic(COLOR_UUID);
    setStatus(`Conectado a ${device.name}`, true);
    await sendColor(picker.value);
  } catch (error) {
    showError(error);
  } finally {
    connectButton.disabled = false;
  }
});

picker.addEventListener('input', event => selectColor(event.target.value));
document.querySelectorAll('.preset').forEach(button =>
  button.addEventListener('click', () => selectColor(button.dataset.color))
);

// Remove older offline caches so changes made during development appear immediately.
if ('serviceWorker' in navigator) {
  navigator.serviceWorker.getRegistrations().then(registrations =>
    registrations.forEach(registration => registration.unregister())
  );
  caches.keys().then(keys => keys.forEach(key => caches.delete(key)));
}
