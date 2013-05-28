import serial
import threading

BAUD_RATE = 115200


class SerialReader(threading.Thread):
    """Handles reading input data from serial console.
    Usage:
        q = threading.Queue()
        s = SerialReader("/dev/ttyS0", q)
        while True:
            keypress = q.get()
            #process keypress here
    """

    def __init__(self, port, queue):
        super(SerialReader, self).__init__()
        self._serial = serial.Serial(port, 115200, timeout=1)
        self._queue = queue
        self._stop = threading.Event()

    def stop(self):
        self._stop.set()

    def run(self):
        try:
            while not self._stop.isSet():
                line = self._serial.readline()
                if line[0] == '#':
                    #received a comment line
                    print line
                    continue
                split = [i.strip() for i in line.split(':')]
                if len(split) == 2:
                    print "Received: %s" % line
                    keycode, typ = split
                    self._queue.put((keycode, int(typ)))
        except KeyboardInterrupt:
            pass
