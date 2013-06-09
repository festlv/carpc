import gps
import threading
import time


class GpsThread(threading.Thread):

    _fix_lock = threading.Lock()
    _last_fix = None

    def run(self):
        gpsd = gps.gps(mode=gps.WATCH_ENABLE)
        while True:
            try:
                gpsd.next()
            except gps.client.json_error:
                pass  # fvz
            except StopIteration:
                time.sleep(10)
            time.sleep(1)
            if gpsd.fix.latitude != 0:
                with self._fix_lock:
                    self._last_fix = (gpsd.fix.latitude, gpsd.fix.longitude)

    def last_fix(self):
        with self._fix_lock:
            return self._last_fix
