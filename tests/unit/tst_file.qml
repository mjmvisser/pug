import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "FileTests"

    function init() {
        Util.mkpath(tmpDir + "filetests/abc");
        Util.touch(tmpDir + "filetests/abc/foo.0001.baz");
    }
    
    function cleanup() {
        Util.remove(tmpDir + "filetests/abc/foo.0001.baz");
        Util.remove(tmpDir + "filetests/def/foo.0001.baz");
        Util.rmdir(tmpDir + "filetests/abc");
        Util.rmdir(tmpDir + "filetests/def");
        Util.rmdir(tmpDir + "filetests");
    }
    
    UpdateOperation {
        id: update
    }
    
    CookOperation {
        id: cook
        dependencies: update
    }
 
    Branch {
        id: branch
        pattern: tmpDir + "filetests/"
    
        Field {
            name: "FOO"
        }

        Field {
            name: "BAR"
            regexp: "\\d{4}|(?:%04d)"
        }

        Field {
            name: "BAZ"
        }
        
        File {
            id: file
            name: "file"
            pattern: "abc/{FOO}.{BAR}.{BAZ}"
        }
        
        File {
            id: file2
            name: "file2"
            input: file
            pattern: "def/{FOO}.{BAR}.{BAZ}"
            linkType: File.Symbolic
        }    
    }
    
    SignalSpy {
        id: updateSpy
        target: update
        signalName: "finished"
    }

    SignalSpy {
        id: cookSpy
        target: cook
        signalName: "finished"
    }
    
    function test_file() {
        compare(file.pattern, "abc/{FOO}.{BAR}.{BAZ}");
        
        update.run(file, {FOO: "foo", BAR: "0001", BAZ: "baz"});
        updateSpy.wait(500);
        compare(file.UpdateOperation.status, Operation.Finished);
        compare(branch.elements[0].path, tmpDir + "filetests/");
        compare(file.elements[0].pattern, tmpDir + "filetests/abc/foo.%04d.baz");
        compare(file.elements[0].paths[0], tmpDir + "filetests/abc/foo.0001.baz");
    }
    
    function test_fileLink() {
        cook.run(file2, {});
        cookSpy.wait(500);
        
        verify(Util.exists(tmpDir + "filetests/def/foo.0001.baz"));
    }
}    