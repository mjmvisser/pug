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
        
        Branch {
            id: abc
            name: "abc"
            pattern: "{ROOT}releasetests/abc/"
        }
        
        Branch {
            id: foo
            name: "foo"
            pattern: "{FOO}/"
            root: abc
            
            Branch {
                id: releaseBranch
                name: "releaseBranch"
                pattern: "release/main/v{VERSION}/"
                root: foo
                ReleaseOperation.versionField: "VERSION"
                
                File {
                    id: releaseFile
                    name: "releaseFile"
                    pattern: "{BAR}.txt"
                    root: releaseBranch

                }
                File {
                    id: releaseSeq
                    name: "releaseSeq"
                    pattern: "{DAG}.{FRAME}.txt"
                    root: releaseBranch
                }
            }
            
            Branch {
                id: workBranch
                name: "workBranch"
                pattern: "work/"
                root: foo
                
                File {
                    id: workFile
                    name: "workFile"
                    pattern: "{BAR}.txt"
                    root: workBranch
                    ReleaseOperation.target: releaseFile
                }
                
                File {
                    id: workSeq
                    name: "workSeq"
                    pattern: "{DAG}.{FRAME}.txt"
                    root: workBranch
                    ReleaseOperation.target: releaseSeq
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
        var env = {ROOT: tmpDir, FOO: "foo", BAR: "bar"};
        compare(releaseFile.ReleaseOperation.findLastVersion(env), 2);
    }

    function test_releaseFile() {
        var env = {ROOT: tmpDir, FOO: "foo", BAR: "bar"};
        var workPath = tmpDir + "releasetests/abc/foo/work/bar.txt";
        var releasePath_v003 = tmpDir + "releasetests/abc/foo/release/main/v003/bar.txt";
        var releasePath_v004 = tmpDir + "releasetests/abc/foo/release/main/v004/bar.txt";
        
        release.run(workFile, env);
        releaseSpy.wait(500);
        compare(release.status, Operation.Finished);
        compare(workFile.details.length, 1);
        compare(workFile.details[0].element.path, workPath);
        compare(workFile.ReleaseOperation.status, Operation.Finished);
        compare(releaseFile.details[0].element.path, releasePath_v003);
        compare(releaseFile.details.length, 1);
        verify(Util.exists(releasePath_v003));
        
        release.run(workFile, env);
        releaseSpy.wait(500);
        compare(release.status, Operation.Finished);
        compare(workFile.ReleaseOperation.status, Operation.Finished);
        compare(workFile.details.length, 1);
        compare(releaseFile.details.length, 1);
        compare(releaseFile.details[0].element.path, releasePath_v004);
        verify(Util.exists(releasePath_v004));
    }
    
    function zeroFill( number, width ) {
        width -= number.toString().length;
        if ( width > 0 ) {
            return new Array( width + (/\./.test( number ) ? 2 : 1) ).join( '0' ) + number;
        }
        return number + ""; // always return a string
    }
    
    function test_releaseSequence() {
        var env = {ROOT: tmpDir, FOO: "foo", DAG: "dag"};
        update.run(workSeq, env);
        updateSpy.wait(500);
        
        release.run(workSeq, env);
        releaseSpy.wait(500);
        
        var releasePath = tmpDir + "releasetests/abc/foo/release/main/v003/dag.%04d.txt";
        
        console.log(workSeq.details.length);
        for (var i = 0; i < workSeq.details.length; i++) {
            console.log("detail " + i + " " + workSeq.details[i].pattern);
        }
        
        compare(workSeq.details.length, 1);
        compare(releaseSeq.details.length, 1);
        compare(releaseSeq.details[0].element.pattern, releasePath);
        compare(releaseBranch.details[0].element.path, tmpDir + "releasetests/abc/foo/release/main/v003/");
        
        for (var frame = 1; frame <= 4; frame++) {
            var framePath = releasePath.replace("%04d", zeroFill(frame, 4));
            verify(Util.exists(framePath), framePath);
        }
    }
}
