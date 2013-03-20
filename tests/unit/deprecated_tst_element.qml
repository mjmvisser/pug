import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    name: "ElementTests"
    
    Element {
        id: basic
        name: "basic"
        pattern: tmpDir + "filepatterns/foo/bar/someseq.%04d.ext"
    }
    
    function test_basic() {
        compare(basic.directory(), tmpDir + "filepatterns/foo/bar/");
        compare(basic.baseName(), "someseq");
        compare(basic.frameSpec(), "%04d");
        compare(basic.extension(), "ext");
    }
    
    Element {
        id: singleFile
        name: "singleFile"
        pattern: tmpDir + "filepatterns/foo/bar/file.ext"
    }
    
    function test_singleFile() {
        compare(singleFile.directory(), tmpDir + "filepatterns/foo/bar/");
        compare(singleFile.baseName(), "file");
        compare(singleFile.frameSpec(), "");
        compare(singleFile.extension(), "ext");
    }
    
    Element {
        id: frame
        name: "frame"
        pattern: "foo.0001.ext"
    }
    
    function test_frame() {
        compare(frame.directory(), "");
        compare(frame.baseName(), "foo");
        compare(frame.frameSpec(), "%04d");
        compare(frame.extension(), "ext");
    }
    
    Element {
        id: file
        pattern: tmpDir + "filepatterns/file/somefile.ext"
    }
    
    function test_file() {
        compare(file.frames(), []);
        compare(file.paths(), [tmpDir + "filepatterns/file/somefile.ext"]);
    }

    Element {
        id: seq
        name: "seq"
        pattern: tmpDir + "filepatterns/seq/someseq.#.ext"
        frameList: FrameList {
            frames: [1,2,3,4]
        }                
    }
    
    function test_seq() {
        compare(seq.frames(), [1,2,3,4]);
        compare(seq.paths(), [tmpDir + "filepatterns/seq/someseq.0001.ext",
                              tmpDir + "filepatterns/seq/someseq.0002.ext",
                              tmpDir + "filepatterns/seq/someseq.0003.ext",
                              tmpDir + "filepatterns/seq/someseq.0004.ext"]);
    }
}