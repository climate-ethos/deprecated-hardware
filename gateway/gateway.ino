// Define BAUDRATE
#define BAUDRATE 115200

// Include libraries
#include <Console.h>
#include <SPI.h>
#include <RH_RF95.h> // Radio
#include <T2WhisperNode.h> // T2Message

// RFM95 Radio
RH_RF95 myRadio;
#define RADIO_FREQUENCY 915.2 // Using the first (0) AU915 band
#define RADIO_TX_POWER 13
// #define RADIO_ENCRYPTION_KEY { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F }
#define RADIO_CONFIG RH_RF95::Bw125Cr45Sf128 // Set Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on. Default medium range.
// uint8_t radioBuf[(T2_MESSAGE_HEADERS_LEN + T2_MESSAGE_MAX_DATA_LEN)];

// T2 Message
T2Message myMsg;

// LED "Heartbeat" pin for when radio signals are received
int led = A2;
// Set frequency to listen on
float frequency = 915.2;

void setup()
{
  pinMode(led, OUTPUT);
  Bridge.begin(BAUDRATE);
  Console.begin();
  while (!Console) ; // Wait for console port to be opened
  Console.println("Start Sketch");

  // Setup Radio
  if (!myRadio.init()) {
    Console.println("init failed");
  }
  myRadio.setModemConfig(RADIO_CONFIG);
  myRadio.setFrequency(RADIO_FREQUENCY);
  // uint8_t myRadioEncryptionKey[] = RADIO_ENCRYPTION_KEY;
  // myRadio.setEncryptionKey(myRadioEncryptionKey);
  myRadio.setTxPower(RADIO_TX_POWER);

  Console.print("Listening on frequency: ");
  Console.println(frequency);
}

void loop()
{
  if (myRadio.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
     if (myRadio.recv(buf, &len))
     {
       myMsg.setSerializedMessage(buf, len);
       digitalWrite(led, HIGH);
       // Concatenate two uint8 messages into uint16
       uint16_t radioData = myMsg.data[2] << 8;
       radioData |= myMsg.data[3];
       Console.print("Received ");
       switch(myMsg.sdx){
        case 0x01:
          Console.print("Voltage: ");
          Console.print((float)radioData / 1000);
          Console.println("V");
          break;
        case 0x0a:
          Console.print("Temperature: ");
          Console.print((float)radioData / 100);
          Console.println("C");
          break;
        case 0x0b:
          Console.print("Humidity: ");
          Console.print((float)radioData / 100);
          Console.println("%");
          break;
        default:
          Console.print("Unknown data type: ");
          Console.println(radioData);
          break;
       }
       digitalWrite(led, LOW);
     }
     else
     {
       Console.println("recv failed");
     }
  }
}
