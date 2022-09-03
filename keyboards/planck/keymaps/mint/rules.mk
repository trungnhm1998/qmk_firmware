SRC += muse.c

NKRO_ENABLE = yes            # Nkey Rollover - if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
AUDIO_ENABLE = yes
TAP_DANCE_ENABLe = yes
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes

BACKLIGHT_ENABLE = no      # Enable keyboard backlight functionality
BOOTMAGIC_ENABLE = no
CONSOLE_ENABLE = no         # Console for debug(+400)
MIDI_ENABLE = no            # MIDI controls
BLUETOOTH_ENABLE = no       # Enable Bluetooth with the Adafruit EZ-Key HID
RGBLIGHT_ENABLE = no        # Enable WS2812 RGB underlight.
UNICODE_ENABLE = no         # Unicode

# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no    # Breathing sleep LED during USB suspend
