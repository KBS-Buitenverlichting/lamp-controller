# Buitenverlichting
## Setup
- Open `Projects\Applications\FreeRTOS\FreeRTOS_LoRaWAN\.project` in STM32CubeIDE
- Change `.\LoRaWAN\App\se-identity.txt` file in the project from .txt to .h extension
- Build 'Debug'
- Flash the microcontroller with the hex file using STM32CubeProgrammer:
    - In the menu "Erasing & Programming" (second button from the top in the left sidebar), browse for the hex file
    - Enable "Verify Programming" and "Run after programming"
    - Click "Start Programming"