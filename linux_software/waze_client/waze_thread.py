import threading
import time

from waze_api.waze_alerts import get_alerts


class WazeUpdaterThread(threading.Thread):
    _last_alerts = None
    _last_position = None

    _last_alerts_lock = threading.Lock()
    _last_position_lock = threading.Lock()

    def set_last_position(self, position):
        with self._last_position_lock:
            self._last_position = position

    def _get_last_position(self):
        with self._last_position_lock:
            return self._last_position

    def get_last_alerts(self):
        with self._last_alerts_lock:
            return self._last_alerts

    def _set_last_alerts(self, alerts):
        with self._last_alerts_lock:
            self._last_alerts = alerts

    def __init__(self, *args, **kwargs):
        self._update_interval = float(kwargs.pop('update_interval'))
        super(WazeUpdaterThread, self).__init__(*args, **kwargs)

    def run(self):
        while True:
            pos = self._get_last_position()
            if pos:
                alerts = get_alerts(pos[0], pos[1], radius=20, num_alerts=50)

                if len(alerts) > 0:
                    self._set_last_alerts(alerts)
            time.sleep(self._update_interval)
