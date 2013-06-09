from evdev import ecodes as e

BUTTONS = {
    '0': e.KEY_UP,
    '1': e.KEY_DOWN,
    '2': e.KEY_LEFT,
    '3': e.KEY_RIGHT,
    '4': e.KEY_ENTER,
    '5': e.KEY_N,  # back key
    '6': e.KEY_J,  # yes
    '7': e.KEY_K,  # no
    '8': e.KEY_VOLUMEUP,
    '9': e.KEY_VOLUMEDOWN,
    '10': e.KEY_NEXT,
    '11': e.KEY_PREVIOUS
}
