import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "FileTests"
    property string tmpDir: "/usr/tmp/pugunittests/" 

    function initTestCase() {
        Util.mkpath(tmpDir);
    }
    
    function cleanupTestCase() {
        if (!Util.rmdir(tmpDir)) {
            console.log("Couldn't remove " + tmpDir);
        }
        
    }

    function test__initTestCase() {
        verify(Util.exists(tmpDir));
    }
}