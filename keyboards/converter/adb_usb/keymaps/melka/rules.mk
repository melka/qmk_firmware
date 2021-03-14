# MCU name
MCU = atmega16u2

# Bootloader selection
#   Teensy       halfkay
#   Pro Micro    caterina
#   Atmel DFU    atmel-dfu
#   LUFA DFU     lufa-dfu
#   QMK DFU      qmk-dfu
#   ATmega32A    bootloadHID
#   ATmega328P   USBasp
BOOTLOADER = qmk-dfu

# Build Options
#   comment out to disable the options.
#
EXTRAKEY_ENABLE  = no
NO_USB_STARTUP_CHECK = yes
LTO_ENABLE		 = yes
