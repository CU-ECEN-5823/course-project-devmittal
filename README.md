# ECEN 5823 Project

## Home Automation - [Project Documentation](https://drive.google.com/drive/u/1/folders/1Xr8iJkr0jLGmn8_0hWulgEYgaV0E8OCy)

This project aims to automate a home by providing a level of convenience and security to the user. This project focuses on two pivotal areas of a home - the kitchen and the bedroom. In the kitchen, one node of the mesh is responsible for detecting fire and gas leakage. In case of a fire or gas leakage, a buzzer is sounded to alert the user to take action to prevent the fire/gas leakage. In the bedroom, a motion detector and a light intensity sensor on a low power node are used to automate the intensity of an LED located in the same room, thereby making a user’s life convenient as they do not have to set the intensity of the light manually.

Team Members

- Souvik De - [Low Power Node](https://github.com/CU-ECEN-5823/course-project-svikde)
- Isha Sawant - [Low Power Node](https://github.com/CU-ECEN-5823/course-project-IshaS27)
- Devansh Mittal - [Friend Node](https://github.com/CU-ECEN-5823/course-project-devmittal)

### Friend Node - [Individual Project Documentation](https://drive.google.com/drive/u/1/folders/1KaiH6MU10pee8-20MCRns5au36piGTqI)

I will be developing the friend node as part of the application. The friend node will be responsible for receiving the sensor data from the two low power nodes. Its role in this application is to support the two low power nodes by ensuring that the two low power nodes consume as little power as possible by driving output based on data received. It has two actuators connected to it - a buzzer to sound an alarm in case of fire/gas leak in the kitchen which is supported by one low power node, and a PWM controlled LED which changes its intensity based on the motion of a person and the luminosity in a bedroom which is supported by the other low power node.  

### Project Status

The 2 actuators have been interfaced and tested with the Blue Gecko 13. The LED has been tested by supplying a PWM signal to it using both the LETIMER and TIMER modules. There is a decision to be made as there is a tradeoff between less energy consumption by using the LETIMER versus better PWM output by using the high frequency, higher energy consuming TIMER module. The buzzer has also been successfully tested. The base Bluetooth state machine code has been written in accordance with the command table to provision, connect and establish itself as a server and a friend node. The UI (LCD Display) has been integrated and tested successfully. The block diagram is updated to reflect the subsystems that will be incorporated into my subsystem.

The Checked in code consists of the following elements:

- Code to output PWM signal using both LETIMER and TIMER modules
- Code to output tone on the Buzzer
- LCD code updated
- Timestamp added to logs
- Bluetooth state machine to connect, provision, set up as server and friend node
- While the base BT code has been incorporated, it has not yet been tested in conjunction with other nodes. More granularity needs to be added to code.