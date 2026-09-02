# Controle LED RGB por Bluetooth

App web instalável para controlar por BLE o projeto ESP32 `led_rgb`.

## Como usar

1. Grave no ESP32 o firmware atualizado do projeto `led_rgb`.
2. Ligue o Bluetooth do computador ou celular.
3. Sirva esta pasta em `localhost` (por exemplo, com a extensão Live Server no VS Code).
4. Abra no Chrome ou Edge e pressione **Conectar ao LED**.
5. Escolha `LED RGB ESP32` e use o seletor de cor.

Web Bluetooth exige uma página segura (`https://`) ou `localhost`. No Android, Chrome é compatível. Safari no iPhone/iPad não oferece Web Bluetooth; para iOS seria necessário um app nativo.

O app envia cores no formato `#RRGGBB`. O potenciômetro do circuito continua controlando o brilho da cor selecionada.
