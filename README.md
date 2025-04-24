# Buitenverlichting
## Setup
- Open `Projects\Applications\FreeRTOS\FreeRTOS_LoRaWAN\.project` in STM32CubeIDE
- Right click the project in the sidebar
- Click "Properties"
- In "C/C++ Build" -> "Settings" -> "MCU/MPU Post build outputs", enable "Convert to Intel Hex file (-O ihex)"
- Apply and Close
- Build 'Debug'
- Flash the microcontroller with the hex file using STM32CubeProgrammer
    - In the menu "Erasing & Programming" (second button from the top in the left sidebar), browse for the hex file
    - Enable "Verify Programming" and "Run after programming"
    - Click "Start Programming"