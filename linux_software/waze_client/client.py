import time
import datetime
import curses
import locale
from waze_thread import WazeUpdaterThread
from gps_thread import GpsThread


class WazeClient():
    def __init__(self):

        self.gps_t = GpsThread()
        self.gps_t.setDaemon(True)
        self.gps_t.start()

        self.waze_t = WazeUpdaterThread(update_interval=1, max_age=3600)
        self.waze_t.setDaemon(True)
        self.waze_t.start()
        locale.setlocale(locale.LC_ALL, '')
        self._code = locale.getpreferredencoding()

        try:
            curses.wrapper(self.run)
        except KeyboardInterrupt:
            pass

    def _encstr(self, string):
        return string.encode(self._code)

    def initscr(self, stdscr):
        #use terminal color theme
        curses.use_default_colors()
        try:
            curses.curs_set(0)
        except:
            pass
        size = stdscr.getmaxyx()
        status_height = 1
        self._status_win = stdscr.subwin(size[0] - status_height, 0)
        self._set_status(False, False)

        self._alerts_win = stdscr.subwin(size[0] - status_height,
                                         size[1], 0, 0)

    def _set_status(self, fix, alerts):
        """
        Sets status line to show if fix and alerts are available
        """
        if not fix:
            text = "No GPS fix"
        elif not alerts:
            text = "No Waze alerts available"

        else:
            text = "GPS fix ok, Waze alerts: %d" % len(alerts)

        lines, cols = self._status_win.getmaxyx()

        text = text.rjust(cols - 1)
        helptext = "WAZE ALERTS"
        text = helptext + text[len(helptext):]
        self._status_win.addstr(0, 0, text,
                                curses.A_REVERSE)
        self._status_win.refresh()

    def _prepare_alert_description(self, alert):
        attrs = curses.color_pair(0)

        if alert['type'] == 'WEATHERHAZARD':
            if len(alert['subtype']) > 0:
                if alert['subtype'] == 'HAZARD_ON_ROAD_OBJECT':
                    typ = 'Object on road'
                elif alert['subtype'] == \
                        'HAZARD_ON_ROAD_CONSTRUCTION':
                    typ = 'Construction on road'
                elif alert['subtype'] == 'HAZARD_ON_ROAD_POT_HOLE':
                    typ = 'Pothole on road'
                elif alert['subtype'] in [
                        'HAZARD_ON_SHOULDER_CAR_STOPPED',
                        'HAZARD_ON_ROAD_CAR_STOPPED']:
                    typ = 'Car stopped'
                else:
                    typ = alert['subtype']
            else:
                typ = alert['type']
        elif alert['type'] == 'POLICEMAN':

            if alert['subtype'] == 'POLICE_VISIBLE':
                typ = 'Police visible'
            elif alert['subtype'] == 'POLICE_HIDING':
                typ = 'Police hiding'
            else:
                typ = 'Police'
            attrs = curses.color_pair(0) | curses.A_BOLD
        else:
            typ = alert['type']

        typ = typ.title()

        return (typ, attrs)

    def _prepare_timestamp(self, alert):

        nowdt = long(datetime.datetime.now().strftime("%s"))
        seconds = nowdt - (alert['pubMillis'] / 1000)

        if seconds > 3600:
            return "%dh %dm" % (seconds / 3600, ((seconds % 3600) / 60))
        else:
            return "%dm" % (seconds / 60,)

    def _prepare_alert_line(self, alert):
        description, attrs = self._prepare_alert_description(alert)
        city = alert.get('city', '')
        street = alert.get('street', '')
        if city and street:
            fmt = u"%s in %s, %s"
            args = (description, city, street)
        elif city and not street:
            fmt = u"%s in %s"
            args = (description, city)
        else:
            fmt = u"%s"
            args = (description, )

        text = fmt % args

        lines, cols = self._alerts_win.getmaxyx()

        distance_text = "%.1f km" % round(alert['distance'], 1)
        timestamp_text = self._prepare_timestamp(alert)
        right_text =  "| %s | %s" % (timestamp_text.rjust(8),
                                     distance_text.rjust(7))

        text = text.ljust(cols - 1)
        text = text[0:-len(right_text)] + right_text
        text = self._encstr(text)

        return (text, attrs)

    def run(self, stdscr):
        self.initscr(stdscr)

        while True:
            fix = self.gps_t.last_fix()
            if fix:
                self._fix = fix
                self.waze_t.set_last_position(fix)

            alerts = self.waze_t.get_last_alerts()

            self._set_status(fix, alerts)

            lines, cols = self._alerts_win.getmaxyx()
            if alerts:
                y = 0
                self._alerts_win.erase()
                for alert in alerts:
                    self._alerts_win.move(y, 0)
                    text, attrs = self._prepare_alert_line(alert)
                    self._alerts_win.addstr(text, attrs)
                    y += 1
                    if y == lines:
                        break
            self._alerts_win.refresh()
            stdscr.refresh()
            time.sleep(1)

if __name__ == '__main__':
    c = WazeClient()
