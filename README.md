# CD4051 avec RP2040 
## Cablage du projet
[Branchements du/des CD4051](#branchement)
## Méthode 1 : CircuitPython
- [Installation de CircuitPython et les bibliothèques a utiliser](#installation-de-circuitpython)
- [Installation de thonny-ide](#installation-de-thonny-ide)
- [Exemple de programme pour 2 potentiomètre directement branchés et aussi 1 potentiomètre sur le CD4051 ](#exemple-de-programme)
## Méthode 2 : C++
- [Avec CPP : vscode et le plugin officiel raspberry pico](#avec-cpp)

## Branchement
- [Avec CPP : vscode et le plugin officiel raspberry pico](#branchement)
![Image.png](./img.jpg)
***

# MCP3008 avec RP2040 
## Avec circuitpython
## Avec C/C++
### Installation de CircuitPython
 - Firmware : https://circuitpython.org/board/raspberry_pi_pico. Mettre le pico en mode boot et glisser déposer le .uf2.
 - circuitpython : Télécharger la librairie depuis https://github.com/adafruit/Adafruit_CircuitPython_Bundle/releases ou https://circuitpython.org/libraries.
   Mettre les librairies adafruit_midi et adafruit_mcp3xxx dans le dossier lib du pico.

   Si besoin d'une remise à 0 du RP2040 alors télécharger le reset (.uf2) : https://www.raspberrypi.com/documentation/microcontrollers/pico-series.html#resetting-flash-memory
### Installation de thonny-ide
[Télécharger ici](https://thonny.org). Lors de la programmation il faudra bien choisir circuitpython et non pas micropython.
### Exemple de programme
Voir SUPER_MIDI_2_potars_et_CD4051_circuitpython
## Avec CPP
Travaillez avec ![im](im2.png) dans vscode pour importer SUPER_MIDI_2_potars_et_CD4051





<br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br><br>
