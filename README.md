# CD4051 avec RP2040 (raspberry pico) en CircuitPython et C++

## [Branchements du/des CD4051](#branchement)

## Avec CircuitPython
- [Installation de CircuitPython](#installation-de-circuitpython)
- [Installation de thonny-ide](#installation-de-thonny-ide)
- [Exemple de programme pour 2 potentiomètre directement branchés et aussi 1 potentiomètre sur le CD4051 ](#exemple-de-programme)

## Avec C++
- [le zip](#avec-cpp)

## Branchement
![Image.png](./img.jpg)

### Installation de CircuitPython
 - Firmware : https://circuitpython.org/board/raspberry_pi_pico. Mettre le pico en mode boot et glisser déposer le .uf2.
 - circuitpython : Télécharger la librairie depuis https://github.com/adafruit/Adafruit_CircuitPython_Bundle/releases ou https://circuitpython.org/libraries.
   Mettre les librairies adafruit_midi et adafruit_mcp3xxx dans le dossier lib du pico.

   Si besoin d'une remise à 0 du RP2040 alors télécharger le reset (.uf2) : https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html#resetting-flash-memory

### Installation de thonny-ide
[Télécharger ici](https://thonny.org). Lors de la programmation il faudra bien choisir circuitpython et non pas micropython.


### Exemple de programme
[Code](./rp2040-cd4051.txt)

## Avec CPP
Travaillez avec ![im](im2.png) dans vscode pour importer [Le zip](SUPER_MIDI_2_potars_et_CD4051.zip)
<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>
