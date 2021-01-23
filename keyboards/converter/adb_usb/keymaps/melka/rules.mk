# MCU name
MCU = atmega32u4

# Bootloader selection
#   Teensy       halfkay
#   Pro Micro    caterina
#   Atmel DFU    atmel-dfu
#   LUFA DFU     lufa-dfu
#   QMK DFU      qmk-dfu
#   ATmega32A    bootloadHID
#   ATmega328P   USBasp
BOOTLOADER = caterina

# Build Options
#   comment out to disable the options.
#
USB_HID_ENABLE   = no
COMMAND_ENABLE   = yes # Commands for debug and configuration
EXTRAKEY_ENABLE  = yes
MOUSEKEY_ENABLE  = no
CONSOLE_ENABLE	 = yes
LTO_ENABLE		 = yes
