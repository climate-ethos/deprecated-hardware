EtHOS Gateway
=================
This documentation provides instructions for configuring a Duinotech XC4394/Dragino LG01 gateway to read temperature sensor data and forward it to the server.

The full user manual for each gateway can be found below (14/07/2022):
- [Duinotech XC4394](https://www.jaycar.com.au/medias/sys_master/images/images/9671859634206/XC4394-manualMain.pdf)
- [Dragino LG01](https://www.dragino.com/downloads/downloads/UserManual/LG01_LoRa_Gateway_User_Manual.pdf)

Both gateways are almost identical so the documentation both gateways is cross-compatible to an extent.

## Setup
1. In the preferences (file -> preferences) enter the following url under `Additional Boards Manager URLs`: `http://www.dragino.com/downloads/downloads/YunShield/package_dragino_yun_test_index.json`
2. Install the Dragino boards under (tools -> boards -> boards manager)
3. Set the board profile to be `Dragino Yun + UNO or LG01/OLG01` (tools -> board -> Dragino Yun + UNO or LG01/OLG01)
4. Download the RadioHead library by clicking [here](https://github.com/dragino/RadioHead/archive/master.zip)
5. Extract the zip file to the Arduino library folder
6. Clone the code in this repository to your PC

## Instructions
1. Open `gateway.c` in the Arduino IDE
2. Connect to the gateway's wifi network or ensure it is on the same network as you
3. Change the Arduino port to the gateway's network port (tools -> port -> Duinotech xx.xx.xx.xx.x)
4. If prompted for password enter the password (default is either `duinotech` or `dragino`)
5. Click the upload button to upload the Arduino sketch to the gateway

Note: If you want to use the Arduino console/serial monitor you will need to click on the Gateway's port again before connecting (tools -> port -> Duinotech xx.xx.xx.xx.x)