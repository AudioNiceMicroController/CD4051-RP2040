# CD4051 avec RP2040 (raspberry pico)

## Installer circuitpython
 - Il faut aller sur https://circuitpython.org/board/raspberry_pi_pico/ et télécharger la dernière version. Débrancher le pico, appuyer sur le bouton en le rebranchant (mode DFU). Glisser déposer le .uf2.
 - Télécharger https://github.com/adafruit/Adafruit_CircuitPython_Bundle/releases. Copier adafruit_midi dans lib qui est à la racide du RP2040.

## Thonny IDE
[Télécharger ici](https://thonny.org). Lors de la programmation il faudra bien choisir circuitpython et non pas micropython.

## Branchements du/des CD4051
![image.png](./img.jpg)

## Programme pour 2 potentiomètres directement branché et aussi 2 potentiomètres sur le CD4051
```
# Contrôleur MIDI hybride (CD4051 + potars directs) avec sélection de ports
# Envoie des messages Control Change en temps réel
# - 8 voies possibles via CD4051 (CC_BASE + canal)
# - Potars directs sur entrées dédiées

import board
import analogio
import digitalio
import time
import usb_midi
import adafruit_midi
from adafruit_midi.control_change import ControlChange

# ================================
# Configuration MIDI
# ================================
CANAL_MIDI = 6
CC_BASE = 20      # CC20 → CC27 pour les canaux du CD4051
SEUIL = 2      # Seuil anti-flutter

midi = adafruit_midi.MIDI(midi_out=usb_midi.ports[1], out_channel=CANAL_MIDI)

# ================================
# Configuration du CD4051
# ================================
S0 = digitalio.DigitalInOut(board.GP0)
S1 = digitalio.DigitalInOut(board.GP1)
S2 = digitalio.DigitalInOut(board.GP2)
INH = digitalio.DigitalInOut(board.GP3)

for pin in [S0, S1, S2, INH]:
    pin.direction = digitalio.Direction.OUTPUT

adc = analogio.AnalogIn(board.A0)  # Entrée reliée au CD4051

def activer_multiplexeur(actif: bool):
    """Active/désactive le multiplexeur"""
    INH.value = not actif
    time.sleep(0.0001)

def selectionner_canal(canal: int):
    """Sélectionne un canal du CD4051 (0–7)"""
    activer_multiplexeur(False)
    S0.value = (canal >> 0) & 1
    S1.value = (canal >> 1) & 1
    S2.value = (canal >> 2) & 1
    activer_multiplexeur(True)

def valeur_vers_midi(valeur_adc: int) -> int:
    """Convertit valeur ADC (0–65535) en valeur MIDI (0–127)"""
    #return max(0, min(127, int((valeur_adc / 65535) * 127)))
    return max(0, min(127, int((valeur_adc / 4095) * 127)))# le 12 bits: 4095 = 2^12-1

# États précédents pour les 8 canaux du MUX
valeur_prec_mux = [0] * 8

# Liste des canaux actifs à lire sur le CD4051
ports = [0, 1, 2, 3, 4, 5, 6, 7]  # tous les canaux
ports = [0,1]  # exemple : seulement A2 et A3

# ================================
# Potars en entrée directe
# ================================
pots_directs = [
    {"pin": analogio.AnalogIn(board.A1), "cc": 30, "last": -1},
    {"pin": analogio.AnalogIn(board.A2), "cc": 31, "last": -1},
]

# ================================
# Boucle principale
# ================================
print("🎛️ Contrôleur MIDI hybride (CD4051 + potars directs)")
print("====================================================")
print(f"Canal MIDI: {CANAL_MIDI}, CC_BASE: {CC_BASE}")
print(f"Ports actifs CD4051: {ports}")
print(f"Seuil: {SEUIL}")
print()

try:
    while True:
        # --- Lecture CD4051 ---
        for canal_actuel in ports:
            selectionner_canal(canal_actuel)
            valeur_adc_16_bits = adc.value# 12 bits normalement mais python renvoie avec un décallage
            valeur_adc_12bit = adc.value >> 4   # divise par 16
            valeur_midi = valeur_vers_midi(valeur_adc_12bit)
            
            if abs(valeur_midi - valeur_prec_mux[canal_actuel]) >= SEUIL:
                
                cc_num = CC_BASE + canal_actuel
                midi.send(ControlChange(cc_num, valeur_midi))

                valeur_prec_mux[canal_actuel] = valeur_midi
                pourcentage = (valeur_midi / 127) * 100
                #print(f"[MUX] Canal {canal_actuel} → CC{cc_num}: {valeur_midi:3d}/127 ({pourcentage:5.1f}%)")
                #print(f"[MUX] Canal {canal_actuel} → CC{cc_num}: {valeur_midi}")

        # --- Lecture potars directs ---
        for pot in pots_directs:
            valeur_adc_16_bits = pot["pin"].value
            valeur_adc_12bit = valeur_adc_16_bits >> 4
            valeur_midi = valeur_vers_midi(valeur_adc_12bit)
            if abs(valeur_midi - pot["last"]) >= SEUIL:
                
                midi.send(ControlChange(pot["cc"], valeur_midi))

                pot["last"] = valeur_midi
                #print(f"[DIR] CC{pot['cc']}: {valeur_midi:3d}")
                #print(f"[DIR] CC{pot['cc']}: {valeur_adc_12bit} {valeur_midi}")

        #time.sleep(0.01)  # boucle fluide

except KeyboardInterrupt:
    activer_multiplexeur(False)

    # Remise à zéro de tous les contrôleurs
    for canal in ports:
        midi.send(ControlChange(CC_BASE + canal, 0, channel=CANAL_MIDI))
    for pot in pots_directs:
        midi.send(ControlChange(pot["cc"], 0, channel=CANAL_MIDI))

    print("\n📴 Arrêt du contrôleur MIDI")
    print("🎵 Tous les CC remis à zéro")


```
