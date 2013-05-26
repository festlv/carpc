from evdev import UInput, ecodes


class Input:
    def __init__(self):
        self._ui = UInput()

    def inject_event(self, keycode, typ):
        self._ui.write(ecodes.EV_KEY, keycode, typ)
        self._ui.syn()
