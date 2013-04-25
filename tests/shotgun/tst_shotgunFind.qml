import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "ShotgunFindTests"

    function init() {
        //Shotgun.logLevel = Log.Warning;
        Shotgun.baseUrl = "https://mokko.shotgunstudio.com";
        Shotgun.apiKey = "ef0623879e54c9f4f2d80f9502a7adea09bfcf8f";
        Shotgun.scriptName = "test";
    }
        
    Root {
        //logLevel: Log.Info
        id: root

        operations: [
            UpdateOperation {
                id: update
                name: "update"
            }
        ]
        
        ShotgunFind {
            id: find
            entityType: "Project"
            filters: [["name", "is", "888_test"]]
            fields: ["name", "sg_description", "sg_full_name"]
        }    
    }

    SignalSpy {
        id: spy
        target: update
        signalName: "finished"
    }
    
    function test_shotgunFind() {
        update.run(find, {});
        spy.wait(5000);
        
        compare(find.details.length, 1);
        compare(find.details[0].context, {});
        var expected = {"type": "Project", "id": 104, "name": "888_test", "sg_full_name": "Test", "sg_description": "test project"};
        for (var k in expected)
            compare(find.details[0].entity[k], expected[k]);
    }
}
