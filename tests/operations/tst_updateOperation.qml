import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "UpdateOperationTests"

    function init() {
        Util.mkpath(tmpDir + "updatetests/abc/foo/def/bar/ghi/18");
        Util.touch(tmpDir + "updatetests/abc/foo/def/bar/ghi/18/somefile.0001.ext");
    }
    
    function cleanup() {
        Util.remove(tmpDir + "updatetests/abc/foo/def/bar/ghi/18/somefile.0001.ext");
        Util.rmdir(tmpDir + "updatetests/abc/foo/def/bar/ghi/18");
        Util.rmdir(tmpDir + "updatetests/abc/foo/def/bar/ghi");
        Util.rmdir(tmpDir + "updatetests/abc/foo/def/bar");
        Util.rmdir(tmpDir + "updatetests/abc/foo/def");
        Util.rmdir(tmpDir + "updatetests/abc/foo");
        Util.rmdir(tmpDir + "updatetests/abc");
        Util.rmdir(tmpDir + "updatetests");
    }
    
    Root {
        id: root
        name: "testRoot"
        
        operations: UpdateOperation {
            id: update
        }

        fields: [     
            Field { name: "FOO" },
            Field { name: "BAR" },
            Field { name: "BAZ"; type: Field.Integer },
            Field { name: "FILENAME"; values: "somefile" },
            FrameSpecField { name: "FRAME" }
        ]
                
        Branch {
            id: branch1
            name: "branch1"
            pattern: tmpDir + "updatetests/abc/{FOO}/def/{BAR}/"
            
            Field {
                name: "BAR"
            }
        }
        
        Branch {
            id: branch2
            name: "branch2"
            pattern: "ghi/{BAZ}/"
            root: branch1
            
            File {
                id: file1
                name: "file1"
                pattern: "{FILENAME}.{FRAME}.ext"
            }
        }
    }

    SignalSpy {
        id: spy
        target: update
        signalName: "finished"
    }
    
    function test_update() {
        var env = {FOO: "foo", BAR: "bar", BAZ: 18, FILENAME: "somefile"};
        update.run(file1, env);
        spy.wait(500);
        compare(branch1.UpdateOperation.status, Operation.Finished);
        compare(branch2.UpdateOperation.status, Operation.Finished);
        compare(file1.UpdateOperation.status, Operation.Finished);
        compare(branch1.details[0].element.path, tmpDir + "updatetests/abc/foo/def/bar/");
        compare(branch2.details[0].element.path, tmpDir + "updatetests/abc/foo/def/bar/ghi/18/");
        compare(file1.details[0].element.pattern, tmpDir + "updatetests/abc/foo/def/bar/ghi/18/somefile.%04d.ext");
    }
}