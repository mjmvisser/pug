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
 
    Root {
        Folder {
            id: branch
            pattern: tmpDir + "filetests/"
        
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
            
            File {
                id: absFile
                name: "absFile"
                pattern: tmpDir + "filetests/abc/foo.{BAR}.baz"
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
        
        update.run(file, {FOO: "foo", BAR: "0001", BAZ: "baz"});
        updateSpy.wait(500);
        
        var fileElementsView = Util.elementsView(file);
        var branchElementsView = Util.elementsView(branch);
        
        console.log(fileElementsView);
        console.log(fileElementsView.elements.length);
        console.log(fileElementsView.elements[0]);
        
        compare(file.UpdateOperation.status, Operation.Finished);
        compare(branch.details.length, 1);
        compare(branchElementsView.elements[0].path(), tmpDir + "filetests/");
        compare(fileElementsView.elements[0].pattern, tmpDir + "filetests/abc/foo.%04d.baz");
        compare(fileElementsView.elements[0].frames[0].frame, 1);
        compare(fileElementsView.elements[0].frames[0].path(), tmpDir + "filetests/abc/foo.0001.baz");
        fileElementsView.destroy();
    }
    
    function test_fileLink() {
        cook.run(file2, {FOO: "foo", BAR: "0001", BAZ: "baz"});
        cookSpy.wait(500);
        
        verify(Util.exists(tmpDir + "filetests/def/foo.0001.baz"));
    }
    
    function test_absFile() {
        verify(Util.exists(tmpDir + "filetests/abc/foo.0001.baz"));

        update.run(absFile, {BAR: "0001"});
        updateSpy.wait(500);
        
        compare(absFile.UpdateOperation.status, Operation.Finished);

        var absFileElementsView = Util.elementsView(absFile);
        compare(absFileElementsView.elements.length, 1);
        compare(absFileElementsView.elements[0].frames.length, 1);
        compare(absFileElementsView.elements[0].frames[0].frame, 1);
        compare(absFileElementsView.elements[0].pattern, tmpDir + "filetests/abc/foo.%04d.baz");
        compare(absFileElementsView.elements[0].frames[0].path(), tmpDir + "filetests/abc/foo.0001.baz");
        absFileElementsView.destroy();
    }
}    
