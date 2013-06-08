import requests
import time

class WazeException(Exception):
    pass

class WazeLoginFailed(WazeException):
    pass

class Waze():
    endpoint = 'http://rt.waze.com/rtserver/'

    def _get_api_url(self, method):
        return self.endpoint + method

    def __init__(self, username=None, password=None):
        self._sess = requests.Session()
        if username and password:
            self.login(username, password)

    def _login_request(self, username, password):
        data = "Login,1,%s,%s,4,5,6,7,8,9" % (username, password)
        r = self._sess.post(self._get_api_url('login'), data=data)
        if "LoginSuccessful" in r.content:
            lines = r.content.split("\n")
            text, loginid, server_cookie = lines[1].split(',')
            self._loginid = loginid
            self._server_cookie = server_cookie
            return (loginid, server_cookie)
        else:
            raise WazeLoginFailed(r.content)

    def _at_request(self, lon, lat):
        gps_str = "%f,%f,1.0,1,1" % (lon, lat)
        data = "UID,%s,%s\r\nAt,%s,-1,-1,T\r\nKeepAlive\r\n" % (
            self._loginid, self._server_cookie, gps_str)
        http = urllib3.PoolManager()
        r = http.request('POST', self._get_api_url("command"), data=data,
        r = self._sess.post(self._get_api_url("command"), data=data, stream=True)
        import pdb
        pdb.set_trace()
        for line in r.iter_lines():
            print line

    def _geo_request(self, lon, lat):
        gps_str = "%f,%f,1.0,1,1" % (lon, lat)
        data = "GetGeoServerConfig,1,%s,3,4,5,6" % (
            gps_str)
        r = self._sess.post(self._get_api_url("command"), data=data, stream=True)
        print r.content

    def login(self, username, password):
        res = self._login_request(username, password)

    def at(self, lon, lat):
        res = self._at_request(lon, lat)


