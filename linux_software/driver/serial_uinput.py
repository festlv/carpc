from evdev import UInput, ecodes


class Input:
    def __init__(self):
        self._ui = UInput()

    def inject_event(self, keycode, typ):
	print "Injecting keycode: %s\n" % keycode
        self._ui.write(ecodes.EV_KEY, keycode, typ)
        self._ui.syn()
