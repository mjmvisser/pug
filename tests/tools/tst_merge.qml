import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    name: "MergeTests"
    
    property string tmpDir: "/usr/tmp/pugunittests/"
    
    function init() {
        Util.mkpath(tmpDir + "mergetests/abc");
        Util.touch(tmpDir + "mergetests/abc/def.ghi");
        Util.touch(tmpDir + "mergetests/abc/jkl.mno");
        Util.touch(tmpDir + "mergetests/abc/pqr.stu");
    }
    
    function cleanup() {
        Util.remove(tmpDir + "mergetests/abc/def.ghi");
        Util.remove(tmpDir + "mergetests/def/jkl.mno");
        Util.remove(tmpDir + "mergetests/def/pqr.stu");
        Util.rmdir(tmpDir + "mergetests/abc");
        Util.rmdir(tmpDir + "mergetests");
    }

    Root {
        UpdateOperation {
            id: update
        }
        
        CookOperation {
            id: cook
            dependencies: update
        }
    
        Branch {
            pattern: tmpDir + "mergetests/abc/";
            
            File {
                id: defGhi
                pattern: "def.ghi"
            }
            
            File {
                id: jklMno
                pattern: "jkl.mno"
            }
            
            File {
                id: pqrStu
                pattern: "pqr.stu"
            }
            
            Merge {
                id: merge
                input1: defGhi
                input2: jklMno
                input3: pqrStu
            }
        }
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }
    
    function test_merge() {
        cook.run(merge, {});
        spy.wait(500);
        
        compare(merge.details.length, 3);
        compare(merge.details[0].element.path, "/usr/tmp/pugunittests/mergetests/abc/def.ghi");
        compare(merge.details[1].element.path, "/usr/tmp/pugunittests/mergetests/abc/jkl.mno");
        compare(merge.details[2].element.path, "/usr/tmp/pugunittests/mergetests/abc/pqr.stu");
    }
}