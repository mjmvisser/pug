from nose.tools import *
from pug import PugAPI

class TestAPI(object):
    def setup(self):
        self.api = PugAPI("../config/BasicConfig.qml")
        
    def test_parse(self):
        expected = {"A": "foo", "B": "bar", "C": "baz", "D": "doakes", "E": 0}
        parsed = self.api.parse("d", "/prod/foo/bar/baz/doakes_0000")
        
        assert_equal(parsed, expected)
    
    def test_map(self):
        expected = "/prod/foo/bar/baz/doakes_0000";
        mapped = self.api.map("d", {"A": "foo", "B": "bar", "C": "baz", "D": "doakes", "E": 0})
        
        assert_equal(mapped, expected);
