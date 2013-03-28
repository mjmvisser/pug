import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "ReleaseOperationTests"

    function init() {
        Util.mkpath(tmpDir + "releasetests/abc/foo/work");
        Util.mkpath(tmpDir + "releasetests/abc/foo/release/main/v001");
        Util.mkpath(tmpDir + "releasetests/abc/foo/release/main/v002");
        Util.touch(tmpDir + "releasetests/abc/foo/work/bar.txt");
        Util.touch(tmpDir + "releasetests/abc/foo/work/dag.0001.txt");
        Util.touch(tmpDir + "releasetests/abc/foo/work/dag.0002.txt");
        Util.touch(tmpDir + "releasetests/abc/foo/work/dag.0003.txt");
        Util.touch(tmpDir + "releasetests/abc/foo/work/dag.0004.txt");
    }

    function cleanup() {
        Util.remove(tmpDir + "releasetests/abc/foo/work/bar.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/work/dag.0001.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/work/dag.0002.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/work/dag.0003.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/work/dag.0004.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/release/main/v001/bar.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/release/main/v002/bar.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/release/main/v003/bar.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/release/main/v004/bar.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/release/main/v003/dag.0001.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/release/main/v003/dag.0002.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/release/main/v003/dag.0003.txt");
        Util.remove(tmpDir + "releasetests/abc/foo/release/main/v003/dag.0004.txt");
        Util.rmdir(tmpDir + "releasetests/abc/foo/work");
        Util.rmdir(tmpDir + "releasetests/abc/foo/work");
        Util.rmdir(tmpDir + "releasetests/abc/foo/release/main/v001");
        Util.rmdir(tmpDir + "releasetests/abc/foo/release/main/v002");
        Util.rmdir(tmpDir + "releasetests/abc/foo/release/main/v003");
        Util.rmdir(tmpDir + "releasetests/abc/foo/release/main/v004");
        Util.rmdir(tmpDir + "releasetests/abc/foo/release/main");
        Util.rmdir(tmpDir + "releasetests/abc/foo/release");
        Util.rmdir(tmpDir + "releasetests/abc/foo");
        Util.rmdir(tmpDir + "releasetests/abc");
        Util.rmdir(tmpDir + "releasetests");
    }

    Root {
        id: root

        operations: [
            UpdateOperation {
                id: update
            },
            ReleaseOperation {
                id: release
                dependencies: update
            }
        ]
        
        fields: [
            Field { name: "ROOT"; regexp: tmpDir },
            Field { name: "FOO" },
            Field { name: "BAR" },
            Field { name: "DAG" },
            FrameSpecField { name: "FRAME" },
            Field { name: "VERSION"; type: Field.Integer; width: 3 }
        ]
        
        Folder {
            id: abc
            name: "abc"
            pattern: "{ROOT}releasetests/abc/"
        }
        
        Folder {
            id: foo
            name: "foo"
            pattern: "{FOO}/"
            root: abc
            
            Folder {
                id: releaseBranch
                name: "releaseBranch"
                pattern: "release/main/v{VERSION}/"
                ReleaseOperation.versionField: "VERSION"
                
                File {
                    id: releaseFile
                    name: "releaseFile"
                    pattern: "{BAR}.txt"
                    ReleaseOperation.source: workFile

                }
                File {
                    id: releaseSeq
                    name: "releaseSeq"
                    pattern: "{DAG}.{FRAME}.txt"
                    ReleaseOperation.source: workSeq
                }
            }
            
            Folder {
                id: workBranch
                name: "workBranch"
                pattern: "work/"
                root: foo
                
                File {
                    id: workFile
                    name: "workFile"
                    pattern: "{BAR}.txt"
                }
                
                File {
                    id: workSeq
                    name: "workSeq"
                    pattern: "{DAG}.{FRAME}.txt"
                }
            }
        }
    }
    
    SignalSpy {
        id: updateSpy
        target: update
        signalName: "finished"
    }

    SignalSpy {
        id: releaseSpy
        target: release
        signalName: "finished"
    }

    function test_lastVersion() {
        var context = {ROOT: tmpDir, FOO: "foo", BAR: "bar"};
        compare(releaseBranch.ReleaseOperation.findLastVersion(context), 2);
    }

    // function test_releaseFile() {
        // var context = {ROOT: tmpDir, FOO: "foo", BAR: "bar"};
        // var workPath = tmpDir + "releasetests/abc/foo/work/bar.txt";
        // var releasePath_v003 = tmpDir + "releasetests/abc/foo/release/main/v003/bar.txt";
        // var releasePath_v004 = tmpDir + "releasetests/abc/foo/release/main/v004/bar.txt";
//         
        // var workFileElementsView = Util.elementsView(workFile);
        // var releaseFileElementsView = Util.elementsView(releaseFile);
//         
        // release.run(releaseFile, context);
        // releaseSpy.wait(500);
        // compare(release.status, Operation.Finished);
        // compare(workFile.details.length, 1);
        // compare(workFileElementsView.elements.length, 1);
        // compare(workFileElementsView.elements[0].path(), workPath);
        // compare(workFile.ReleaseOperation.status, Operation.Finished);
        // compare(releaseFileElementsView.elements.length, 1);
        // compare(releaseFileElementsView.elements[0].path(), releasePath_v003);
        // compare(releaseFile.details.length, 1);
        // verify(Util.exists(releasePath_v003));
//         
        // release.run(releaseFile, context);
        // releaseSpy.wait(500);
        // compare(release.status, Operation.Finished);
        // compare(workFile.ReleaseOperation.status, Operation.Finished);
        // compare(workFile.details.length, 1);
        // compare(releaseFile.details.length, 1);
        // compare(releaseFileElementsView.elements.length, 1);
        // compare(releaseFileElementsView.elements[0].path(), releasePath_v004);
        // verify(Util.exists(releasePath_v004));
    // }
//     
    // function zeroFill( number, width ) {
        // width -= number.toString().length;
        // if ( width > 0 ) {
            // return new Array( width + (/\./.test( number ) ? 2 : 1) ).join( '0' ) + number;
        // }
        // return number + ""; // always return a string
    // }
//     
    // function test_releaseSequence() {
        // var context = {ROOT: tmpDir, FOO: "foo", DAG: "dag"};
        // update.run(releaseSeq, context);
        // updateSpy.wait(500);
// 
        // var releaseSeqElementsView = Util.elementsView(releaseSeq);
//         
        // release.run(releaseSeq, context);
        // releaseSpy.wait(500);
//         
        // var releasePath = tmpDir + "releasetests/abc/foo/release/main/v003/dag.%04d.txt";
//         
        // // console.log(workSeq.details.length);
        // // for (var i = 0; i < workSeq.details.length; i++) {
            // // console.log("detail " + i + " " + workSeq.details[i].pattern);
        // // }
//         
        // compare(workSeq.details.length, 1);
        // compare(releaseSeq.details.length, 1);
        // compare(releaseSeqElementsView.elements.length, 1);
        // compare(releaseSeqElementsView.elements[0].pattern, releasePath);
//         
        // for (var frame = 1; frame <= 4; frame++) {
            // var framePath = releasePath.replace("%04d", zeroFill(frame, 4));
            // verify(Util.exists(framePath), framePath);
        // }
    // }
}
