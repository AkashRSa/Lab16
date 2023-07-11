#include "Particle.h"
#include "oled-wing-adafruit.h"

SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);

OledWingAdafruit display;

const size_t UART_TX_BUF_SIZE = 20;

void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);

const BleUuid serviceUuid("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");

const BleUuid rxUuid("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");
BleCharacteristic rxCharacteristic("rx", BleCharacteristicProperty::WRITE_WO_RSP, rxUuid, serviceUuid, onDataReceived, NULL);

const BleUuid txUuid("6E400003-B5A3-F393-E0A9-E50E24DCCA9E");
BleCharacteristic txCharacteristic("tx", BleCharacteristicProperty::NOTIFY, txUuid, serviceUuid);

bool isConnected = false;
bool showMessage = false;
bool sendMessage = true;
bool d = true;

void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);

void setup()
{
  Serial.begin(9600);
  while (!Serial.isConnected())
  {
  }
  RGB.control(true);
  RGB.color(255, 255, 0);

  BLE.addCharacteristic(txCharacteristic);
  BLE.addCharacteristic(rxCharacteristic);

  BleAdvertisingData data;
  data.appendServiceUUID(serviceUuid);
  BLE.setDeviceName("Fenton's Argon"); // D4:AF:C7:44:BF:F8
  BLE.on();
  BLE.advertise(&data);

  display.setup();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  Serial.println("setup");
  display.display();
}

void loop()
{
  display.clearDisplay();
  if (BLE.connected())
  {
    RGB.color(0, 0, 255);
  }
  else
  {
    RGB.color(255, 255, 0);
  }
  if (sendMessage)
  {
    delay(1000);
    uint8_t txBuf[10];
    String message = "Connected!";
    message.toCharArray((char *)txBuf, message.length() + 1);
    txCharacteristic.setValue(txBuf, message.length() + 1);
  }
  display.loop();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  if (showMessage)
  {
    display.println("Message Received!");
  }
  if (!d)
  {
    BLE.disconnect();
  }
  display.display();
}

void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
{
  for (size_t i = 0; i < len; i++)
  {
    Serial.write(data[i]);
    if (data[i] == 49)
    {
      showMessage = true;
    }
    else if (data[i] == 48)
    {
      Serial.println("Disconnect");
      showMessage = false;
      d = false;
      // BLE.off();
      // BLE.end();
    }
    else
    {
      Serial.println("Other");
    }
  }
}
