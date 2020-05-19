BUILD_OPTION_NAMES = \
  BOOTMAGIC_ENABLE \
  MOUSEKEY_ENABLE \
  EXTRAKEY_ENABLE \
  CONSOLE_ENABLE \
  COMMAND_ENABLE \
  NKRO_ENABLE \
  TERMINAL_ENABLE \
  CUSTOM_MATRIX \
  DEBOUNCE_TYPE \
  SPLIT_KEYBOARD \
  DYNAMIC_KEYMAP_ENABLE \
  USB_HID_ENABLE \
  VIA_ENABLE

HARDWARE_OPTION_NAMES = \
  SLEEP_LED_ENABLE \
  BACKLIGHT_ENABLE \
  BACKLIGHT_CUSTOM_DRIVER \
  RGBLIGHT_ENABLE \
  RGBLIGHT_CUSTOM_DRIVER \
  RGB_MATRIX_ENABLE \
  SERIAL_LINK_ENABLE \
  CIE1931_CURVE \
  MIDI_ENABLE \
  BLUETOOTH_ENABLE \
  NRF24_ENABLE \
  AUDIO_ENABLE \
  FAUXCLICKY_ENABLE \
  HD44780_ENABLE \
  ENCODER_ENABLE \
  LCD_ENABLE \
  LED_TABLES \
  POINTING_DEVICE_ENABLE \
  VISUALIZER_ENABLE \
  DIP_SWITCH_ENABLE

OTHER_OPTION_NAMES = \
  UNICODE_ENABLE \
  UCIS_ENABLE \
  UNICODEMAP_ENABLE \
  UNICODE_COMMON \
  API_SYSEX_ENABLE \
  AUTO_SHIFT_ENABLE \
  AUTO_SHIFT_MODIFIERS \
  COMBO_ENABLE \
  KEY_LOCK_ENABLE \
  LEADER_ENABLE \
  PRINTING_ENABLE \
  STENO_ENABLE \
  TAP_DANCE_ENABLE \
  VIRTSER_ENABLE \
  OLED_ENABLE \
  LED_BACK_ENABLE \
  LED_UNDERGLOW_ENABLE \
  LED_ANIMATIONS \
  IOS_DEVICE_ENABLE \
  HELIX ZINC \
  ADAFRUIT_BLE_ENABLE \
  AUTOLOG_ENABLE \
  DEBUG_ENABLE \
  ENCODER_ENABLE_CUSTOM \
  GERMAN_ENABLE \
  HAPTIC_ENABLE \
  HHKB_RN42_ENABLE \
  ISSI_ENABLE \
  KEYBOARD_LOCK_ENABLE \
  KEYLOGGER_ENABLE \
  LCD_BACKLIGHT_ENABLE \
  MACROS_ENABLED \
  ONEHAND_ENABLE \
  PS2_MOUSE_ENABLE \
  RAW_ENABLE \
  SWAP_HANDS_ENABLE \
  USB_6KRO_ENABLE \
  WATCHDOG_ENABLE \
  XT_ENABLE \
  ERGOINU \
  NO_USB_STARTUP_CHECK \
  DISABLE_PROMICRO_LEDs \
  MITOSIS_DATAGROK_BOTTOMSPACE \
  MITOSIS_DATAGROK_SLOWUART \
  RGB_MATRIX_KEYPRESSES \
  LED_MIRRORED \
  RGBLIGHT_FULL_POWER \
  Link_Time_Optimization \
  LINK_TIME_OPTIMIZATION_ENABLE

define NAME_ECHO
	@echo "  $1 = $($1)		# $(origin $1)"

endef

.PHONY: show_build_options0 show_build_options
show_build_options0:
	@echo " KEYBOARD = $(KEYBOARD)"
	@echo " KEYMAP   = $(KEYMAP)"
	@echo " MCU      = $(MCU)"
	@echo " MCU_SERIES = $(MCU_SERIES)"
	@echo " PLATFORM = $(PLATFORM)"
	@echo " FIRMWARE_FORMAT = $(FIRMWARE_FORMAT)"
	@echo
	@echo "Build Options:"
	$(foreach A_OPTION_NAME,$(sort $(BUILD_OPTION_NAMES)),\
		$(call NAME_ECHO,$(A_OPTION_NAME)))

show_build_options: show_build_options0
	@echo
	@echo "If you want to know more, please try 'show_all_features' or 'show_full_features'"
	@echo

.PHONY: show_all_features
show_all_features: show_build_options0
	@echo
	@echo "Hardware Options:"
	$(foreach A_OPTION_NAME,$(sort $(HARDWARE_OPTION_NAMES)),\
		$(if $($(A_OPTION_NAME)),$(call NAME_ECHO,$(A_OPTION_NAME))))
	@echo
	@echo "Other Options:"
	$(foreach A_OPTION_NAME,$(sort $(OTHER_OPTION_NAMES)),\
		$(if $($(A_OPTION_NAME)),$(call NAME_ECHO,$(A_OPTION_NAME))))

.PHONY: show_full_features
show_full_features: show_build_options0
	@echo
	@echo "Hardware Options:"
	$(foreach A_OPTION_NAME,$(sort $(HARDWARE_OPTION_NAMES)),\
		$(call NAME_ECHO,$(A_OPTION_NAME)))
	@echo
	@echo "Other Options:"
	$(foreach A_OPTION_NAME,$(sort $(OTHER_OPTION_NAMES)),\
		$(call NAME_ECHO,$(A_OPTION_NAME)))
