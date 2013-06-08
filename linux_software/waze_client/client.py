import time
from waze_thread import WazeUpdaterThread
from gps_thread import GpsThread


def main():
    gps_t = GpsThread()
    gps_t.setDaemon(True)
    gps_t.start()

    waze_t = WazeUpdaterThread(update_interval=10)
    waze_t.setDaemon(True)
    waze_t.start()

    while True:
        fix = gps_t.last_fix()
        if fix:
            waze_t.set_last_position(fix)
        print fix

        alerts = waze_t.get_last_alerts()
        if alerts:
            print alerts
            print len(alerts)
        time.sleep(10)

if __name__ == '__main__':
    main()
