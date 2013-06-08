import unittest
import waze
import urllib3

class LoginTestCase(unittest.TestCase):

    def testSuccessfulLogin(self):
        w = waze.Waze()
        w.login('festlv', 'waze123')

    def testFailingLogin(self):
        with self.assertRaises(waze.WazeException):
            w = waze.Waze()
            w.login("test", "test")

class AtTestCase(unittest.TestCase):
    def testAt(self):
        w = waze.Waze()
        w.login('festlv', 'waze123')
        w.at(54.2211, 11.1220)
        w._geo_request(54.123,11.123)
if __name__ == '__main__':
    unittest.main()
