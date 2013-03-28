import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "FileTests"

    function init() {
        Util.mkpath(tmpDir + "filetests/abc");
        Util.touch(tmpDir + "filetests/abc/foo.0001.baz");
        Util.touch(tmpDir + "filetests/abc/foo.0002.baz");
        Util.touch(tmpDir + "filetests/abc/foo.0003.baz");
        Util.touch(tmpDir + "filetests/abc/foo.0004.baz");
        Util.touch(tmpDir + "filetests/abc/foo.0005.baz");
    }
    
    function cleanup() {
        Util.remove(tmpDir + "filetests/abc/foo.0001.baz");
        Util.remove(tmpDir + "filetests/abc/foo.0002.baz");
        Util.remove(tmpDir + "filetests/abc/foo.0003.baz");
        Util.remove(tmpDir + "filetests/abc/foo.0004.baz");
        Util.remove(tmpDir + "filetests/abc/foo.0005.baz");
        Util.remove(tmpDir + "filetests/def/foo.0001.baz");
        Util.remove(tmpDir + "filetests/def/foo.0002.baz");
        Util.remove(tmpDir + "filetests/def/foo.0003.baz");
        Util.remove(tmpDir + "filetests/def/foo.0004.baz");
        Util.remove(tmpDir + "filetests/def/foo.0005.baz");
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
 
    Root {
        fields: [
            Field {
                name: "FOO" 
            },
            Field {
                name: "BAR"
                regexp: "\\d{4}|(?:%04d)"
                defaultValue: "%04d"
            },
            Field {
                name: "BAZ"
            }
        ]
        
        File {
            id: absFile
            name: "absFile"
            pattern: tmpDir + "filetests/abc/{FOO}.{BAR}.{BAZ}"
        }
        
        Folder {
            id: branch
            name: "filetests"
            pattern: tmpDir + "filetests/"
        
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
        
        updateSpy.clear();
        update.run(file, {FOO: "foo", BAR: "0001", BAZ: "baz"});
        updateSpy.wait(500);
        
        var fileElementsView = Util.elementsView(file);
        var branchElementsView = Util.elementsView(branch);
        
        compare(file.UpdateOperation.status, Operation.Finished);
        compare(branch.details.length, 1);
        compare(branchElementsView.elements[0].path(), tmpDir + "filetests/");
        compare(fileElementsView.elements[0].pattern, tmpDir + "filetests/abc/foo.%04d.baz");
        verify(fileElementsView.elements[0].frames[0]);
        compare(fileElementsView.elements[0].frames[0].frame, 1);
        compare(fileElementsView.elements[0].frames[0].path(), tmpDir + "filetests/abc/foo.0001.baz");
    }
    
    function test_fileLink() {
        cook.run(file2, {FOO: "foo", BAR: "0001", BAZ: "baz"});
        cookSpy.wait(500);
        
        verify(Util.exists(tmpDir + "filetests/def/foo.0001.baz"));
    }
    
    function test_absFile() {
        verify(Util.exists(tmpDir + "filetests/abc/foo.0001.baz"));

        updateSpy.clear();
        update.run(absFile, {});
        updateSpy.wait(500);
        
        compare(absFile.UpdateOperation.status, Operation.Finished);

        var absFileElementsView = Util.elementsView(absFile);
        compare(absFileElementsView.elements.length, 1);
        compare(absFileElementsView.elements[0].frames.length, 5);
        compare(absFileElementsView.elements[0].frames[0].frame, 1);
        compare(absFileElementsView.elements[0].frames[1].frame, 2);
        compare(absFileElementsView.elements[0].frames[2].frame, 3);
        compare(absFileElementsView.elements[0].frames[3].frame, 4);
        compare(absFileElementsView.elements[0].frames[4].frame, 5);
        compare(absFileElementsView.elements[0].pattern, tmpDir + "filetests/abc/foo.%04d.baz");
        compare(absFileElementsView.elements[0].frames[0].path(), tmpDir + "filetests/abc/foo.0001.baz");
        compare(absFileElementsView.elements[0].frames[1].path(), tmpDir + "filetests/abc/foo.0002.baz");
        compare(absFileElementsView.elements[0].frames[2].path(), tmpDir + "filetests/abc/foo.0003.baz");
        compare(absFileElementsView.elements[0].frames[3].path(), tmpDir + "filetests/abc/foo.0004.baz");
        compare(absFileElementsView.elements[0].frames[4].path(), tmpDir + "filetests/abc/foo.0005.baz");
    }
}    
