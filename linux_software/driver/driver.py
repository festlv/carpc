import argparse
from button_map import BUTTONS
from serial_reader import SerialReader
from serial_uinput import Input
from Queue import Queue, Empty


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("port", help="Serial port to listen for events.")
    args = parser.parse_args()
    inp = Input()

    q = Queue()
    s = SerialReader(args.port, q)
    s.start()

    while True:
        try:
            item = q.get(block=False)
            keycode = BUTTONS.get(item[0], False)
            if keycode:
                inp.inject_event(keycode, item[1])
        except Empty:
            pass
        except KeyboardInterrupt:
            break

    s.stop()

if __name__ == '__main__':
    main()
