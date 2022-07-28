/*
 * DHT22 Sensor on a Whisper Node
 */

#include <T2WhisperNode.h>
#include <LowPower.h>
#include <DHT.h>

#include <RH_RF69.h>

// DHT 22
#define DHT_PWD_PIN A2 // We power the DHT22 via a MCU GPIO so we can control when it's up or not
#define DHT_DATA_PIN A1
#define DHT_TYPE DHT22
DHT dht(DHT_DATA_PIN, DHT_TYPE);

// SPI Flash
T2Flash myFlash;

// RFM69 Radio
#define RADIO_FREQUENCY 915.2 // Using the first (0) AU915 band
#define RADIO_TX_POWER 13
#define RADIO_ENCRYPTION_KEY { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F }
RH_RF69 myRadio;
uint8_t radioBuf[(T2_MESSAGE_HEADERS_LEN + T2_MESSAGE_MAX_DATA_LEN)];

// T2 Message
T2Message myMsg;
#define nodeAddr 0x81
#define baseAddr 0x0a

void setup()
{
  // Serial
  Serial.begin(115200);
  Serial.println(F("Example: DHT22 Sensor Node"));

  Serial.println(F("Putting Radio and SPI Flash to Sleep"));
  // Radio - Initialize the radio and put it to sleep to save energy
  myRadio.init();
  myRadio.setModemConfig(RH_RF69::FSK_Rb250Fd250);
  myRadio.setFrequency(RADIO_FREQUENCY);
  uint8_t myRadioEncryptionKey[] = RADIO_ENCRYPTION_KEY;
  myRadio.setEncryptionKey(myRadioEncryptionKey);
  myRadio.setTxPower(RADIO_TX_POWER);
  myRadio.sleep();

  // Flash - We're not using, so just power it down to save energy
  myFlash.init(T2_WPN_FLASH_SPI_CS);
  myFlash.powerDown();

  // DHT22
  digitalWrite(DHT_PWD_PIN, LOW);
  pinMode(DHT_PWD_PIN, OUTPUT);
  dht.begin();

  // Setup the Blue LED pin
  digitalWrite(T2_WPN_LED_1, LOW); // Set LED to Off
  pinMode(T2_WPN_LED_1, OUTPUT);   // Set LED pint to OUTPUT

}

uint8_t loopCount = 7;

void loop()
{

  // We only do something every 7 sleep cycles (7 * 8 = 56 seconds)
  if(loopCount == 7)
  {

    // Blue LED blink means we just wake up
    digitalWrite(T2_WPN_LED_1, HIGH);
    delay(5);
    digitalWrite(T2_WPN_LED_1, LOW);

    // Do some work!
    sendTestVoltage(1);
    sendTestVoltage(2);
    sendTempHumidity();

    loopCount = 0;

  }

  // Using Low-Power library to put the MCU to Sleep
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  loopCount++;

}

void sendTempHumidity()
{
  // Turn the Sensor ON
  digitalWrite(DHT_PWD_PIN, HIGH);

  // Read temperature as Celsius * 100 to remove any decimals
  // False, True means we don't want Fahrenheit and we want to force the reading.
  int16_t t = dht.readTemperature(false, true) * 100;

  // * 100 to remove any decimals. We do not force the reading, so we use the last info
  uint16_t h = dht.readHumidity() * 100;

  //Turn the Sensor OFF
  digitalWrite(DHT_PWD_PIN, LOW);

  /*
  // For Debbuging only
  Serial.println(t);
  Serial.println(h);
  delay(10);
  */

  // Prepare Temperature Message and send
  myMsg.cmd = 0x03; // Return Data
  myMsg.idx = 0x05; // HID and Sensors
  myMsg.sdx = 0x0a; // Temperature
  myMsg.data[0] = 0x01; // Operation Temp in Celcius * 100
  myMsg.data[1] = 0x01; // Sensor ID - we only have 1 in this case
  myMsg.data[2] = t >> 8;
  myMsg.data[3] = t;
  myMsg.len = 4; // Update length
  sendMessage();

  // Now change message details with the Humidity details and send it
  myMsg.cmd = 0x03; // Return Data
  myMsg.idx = 0x05; // HID and Sensors
  myMsg.sdx = 0x0b; // Humidity
  myMsg.data[0] = 0x01; // Operation Humidity in % * 100
  myMsg.data[1] = 0x01; // Sensor ID - we only have 1 in this case
  myMsg.data[2] = h >> 8;
  myMsg.data[3] = h;
  myMsg.len = 4; // Update length
  sendMessage();

}

void sendTestVoltage(uint8_t supply)
{
  uint16_t voltage = 0;

  // Get Voltage readings from supply/battery
  if(supply == 1)
  {
    voltage = T2Utils::readVoltage(T2_WPN_VBAT_VOLTAGE, T2_WPN_VBAT_CONTROL);
    myMsg.sdx = 0x64; // Battery
  }

  if(supply == 2)
  {
    voltage = T2Utils::readVoltage(T2_WPN_VIN_VOLTAGE, T2_WPN_VIN_CONTROL);
    myMsg.sdx = 0x65; // Supply
  }

  // Fill the message details and send it
  myMsg.cmd = 0x03; // Return Data
  myMsg.idx = 0x06; // Node Info
  myMsg.data[0] = 0x01; // Operation Last Reading
  myMsg.data[1] = 0x01; // Battery/Supply Index, if multiple supplies
  myMsg.data[2] = voltage >> 8;
  myMsg.data[3] = voltage;
  myMsg.len = 4; // Update length

  sendMessage();

}

void sendMessage()
{
  uint8_t radioBufLen = 0;

  // Prepare the Message headers
  myMsg.src = nodeAddr;
  myMsg.dst = baseAddr;

  // Encode Message and get the full length
  myMsg.getSerializedMessage(radioBuf, &radioBufLen);

  // Send it
  myRadio.send(radioBuf, radioBufLen);
  myRadio.waitPacketSent(100);
  myRadio.sleep();

}
