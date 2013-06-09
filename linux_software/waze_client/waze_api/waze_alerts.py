"""
Returns alerts from coordinates.

Uses http://world.waze.com/widget/?lat=56.9115&lon=24.1372&radius_km=50
"""

import requests
import datetime
import json
from math import radians, cos, asin, sin, atan2, degrees, sqrt


URL = "http://world.waze.com/rtserver/web/GeoRSS?format=JSON&types= \
traffic%%2Calerts&mj=1&ma=%(num_alerts)s&jmds=120&jmu=1&left=%(left)f \
&right=%(right)f&bottom=%(bottom)f&top=%(top)f&bo=true"


def get_radius_coordinates(lat, lon, radius):
    """
    Takes one coordinate pair and radius, returns two coordinate pairs which
    form a bounding box around lat/lon with certain radius
    """

    due_north = radians(0)
    due_south = radians(180)
    due_east = radians(90)
    due_west = radians(270)
    earth_radius = 6371.0
    lat_r = radians(lat)
    lon_r = radians(lon)

    def dist_vert(lat_r, distance, earth_radius, direction):
        return degrees(
            asin(
                sin(lat_r) * cos(distance / earth_radius) +
                cos(lat_r) * sin(distance / earth_radius) * cos(direction)
            )
        )

    northmost = dist_vert(lat_r, radius, earth_radius, due_north)
    southmost = dist_vert(lat_r, radius, earth_radius, due_south)

    def dist_horiz(lon_r, distance, earth_radis, direction):
        return degrees(
            lon_r + atan2(
                sin(direction) * sin(radius / earth_radius) * cos(lat_r),
                cos(radius / earth_radius) - sin(lat_r) * sin(lat_r)))
    eastmost = dist_horiz(lon_r, radius, earth_radius, due_east)
    westmost = dist_horiz(lon_r, radius, earth_radius, due_west)

    if northmost > southmost:
        lat1 = southmost
        lat2 = northmost
    else:
        lat1 = northmost
        lat2 = southmost

    if eastmost > westmost:
        lon1 = westmost
        lon2 = eastmost
    else:
        lon1 = eastmost
        lon2 = westmost

    return ((lat1, lon1), (lat2, lon2))


def _dist(coords, fix):
    R = 6371.0  # km
    dLat = radians(coords[0] - fix[0])
    dLon = radians(coords[1] - fix[1])
    lat1 = radians(fix[0])
    lat2 = radians(coords[0])

    a = sin(dLat / 2) * sin(dLat / 2) + \
        sin(dLon / 2) * sin(dLon / 2) * cos(lat1) * cos(lat2)

    c = 2 * atan2(sqrt(a), sqrt(1 - a))
    return R * c

def normalize_alerts(json_resp, fix):

    alerts = []
    nowdt = long(datetime.datetime.now().strftime("%s"))
    if not json_resp['alerts']:
        return alerts 
    for alert in json_resp['alerts']:
        c = alert.get('city', '')
        if ',' in c:
            c = c.split(',')[0]
            alert['city'] = c
        dist = _dist(
            (alert['location']['y'], alert['location']['x']),
            fix)
        alert['distance'] = dist
        alert['deltaT'] = nowdt - (alert['pubMillis'] / 1000)
        alerts.append(alert)


    return alerts


def get_alerts(lat, lon, radius=30, num_alerts=50):
    """
    Returns up to num_alerts from lat/lon coordinates and radius in km
    Returns a list of JSON data (as dict):

    {u'city': u'J\xc5\xabrmala, 99', u'nThumbsUp': 0,
    u'showFacebookPic': False,
    u'reportMood': 5, u'speed': 0, u'roadType': 2, u'comments': [], u'id':
    u'alert-899527005/b0a2effc-0fa2-487a-9041-6ecfc983b2d6',
    u'subtype': u'HAZARD_ON_SHOULDER_CAR_STOPPED', u'inscale': True,
    u'street': u'Me\xc5\xbemalas iela',
    u'location': {u'y': 56.939792, u'x': 23.685087}, u'country': u'LG',
    u'nComments': 0, u'reportBy': u'Dzintars', u'magvar': 111,
    u'type': u'WEATHERHAZARD', u'reportRating': 3,
    u'pubMillis': 1370702193428L,
    u'uuid': u'b0a2effc-0fa2-487a-9041-6ecfc983b2d6'}


    """
    c_box = get_radius_coordinates(lat, lon, radius)
    r = requests.get(URL % {'left': c_box[0][1], 'right': c_box[1][1],
                            'top': c_box[1][0], 'bottom': c_box[0][0],
                            'num_alerts': num_alerts})

    ustr = r.content.decode('utf-8')
    json_resp = json.loads(ustr)

    return normalize_alerts(json_resp, (lat, lon))

if __name__ == "__main__":
    alerts = get_alerts(56.953113, 24.116364)
    print alerts
