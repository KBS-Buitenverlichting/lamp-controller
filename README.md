# Buitenverlichting
## Setup
- Open `Projects\Applications\FreeRTOS\FreeRTOS_LoRaWAN\.project` in STM32CubeIDE
- Copy the `.\LoRaWAN\App\se-identity.txt` file and change the extension to .h
- Build 'Debug'
- Flash the microcontroller with the hex file using STM32CubeProgrammer:
    - In the menu "Erasing & Programming" (second button from the top in the left sidebar), browse for the hex file
    - Enable "Verify Programming" and "Run after programming"
    - Click "Start Programming"