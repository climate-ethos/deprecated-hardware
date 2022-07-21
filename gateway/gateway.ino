// Define BAUDRATE
#define BAUDRATE 115200

// Include libraries
#include <Console.h>
#include <SPI.h>
#include <RH_RF95.h>

// Singleton instance of the radio driver
RH_RF95 rf95;

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
  if (!rf95.init())
    Console.println("init failed");
  // Setup ISM frequency
  rf95.setFrequency(frequency);
  // Setup Power,dBm
  rf95.setTxPower(13);

  // Setup Spreading Factor (6 ~ 12)
  rf95.setSpreadingFactor(7);

  // Setup BandWidth, option: 7800,10400,15600,20800,31200,41700,62500,125000,250000,500000
  rf95.setSignalBandwidth(125000);

  // Setup Coding Rate:5(4/5),6(4/6),7(4/7),8(4/8)
  rf95.setCodingRate4(5);

  Console.print("Listening on frequency: ");
  Console.println(frequency);
}

void loop()
{
  if (rf95.available())
  {
    // Should be a message for us now
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    // For Testing {{{
    buf[0] = 1;
    buf[1] = 3;
    buf[2] = 3;
    buf[3] = 7;
    len = 4;
    digitalWrite(led, HIGH);
    RH_RF95::printBuffer("request: ", buf, len);
    Console.print("got request: ");
    Console.println((char*)buf);
    Console.print("RSSI: ");
    Console.println(rf95.lastRssi(), DEC);
    digitalWrite(led, LOW);
    // Send data to MPU
    Bridge.transfer(buf, len);
    // }}}
    // if (rf95.recv(buf, &len))
    // {
    //   digitalWrite(led, HIGH);
    //   RH_RF95::printBuffer("request: ", buf, len);
    //   Console.print("got request: ");
    //   Console.println((char*)buf);
    //   Console.print("RSSI: ");
    //   Console.println(rf95.lastRssi(), DEC);
    //   digitalWrite(led, LOW);
    // }
    // else
    // {
    //   Console.println("recv failed");
    // }
  }
}
