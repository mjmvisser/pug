import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "CookOperationTests"

    function init() {
        Util.mkpath(tmpDir + "cooktests/abc/foo/work");
        Util.touch(tmpDir + "cooktests/abc/foo/work/baa.txt");
        Util.touch(tmpDir + "cooktests/abc/foo/work/bar.txt");
        Util.touch(tmpDir + "cooktests/abc/foo/work/baz.txt");
    }

    function cleanup() {
        Util.remove(tmpDir + "cooktests/abc/foo/work/baa.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/bar.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/baz.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/temp/temp_baa.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/temp/temp_bar.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/temp/temp_baz.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/cooked/baa.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/cooked/bar.txt");
        Util.remove(tmpDir + "cooktests/abc/foo/work/cooked/baz.txt");
        Util.rmdir(tmpDir + "cooktests/abc/foo/work/temp");
        Util.rmdir(tmpDir + "cooktests/abc/foo/work/cooked");
        Util.rmdir(tmpDir + "cooktests/abc/foo/work");
        Util.rmdir(tmpDir + "cooktests/abc/foo");
        Util.rmdir(tmpDir + "cooktests/abc");
        Util.rmdir(tmpDir + "cooktests");
    }

    Root {
        id: root

        UpdateOperation {
            id: update
        }
        
        CookOperation {
            id: cook
            dependencies: update
        }
        
        Field {
            name: "ROOT"
            regexp: tmpDir
        }
        
        Field {
            name: "FOO"
        }
        
        Field {
            name: "FILENAME"
            regexp: ".*"
        }
        
        Branch {
            id: abc
            name: "abc"
            pattern: "{ROOT}cooktests/abc/"
        }
        
        Branch {
            id: foo
            name: "foo"
            pattern: "{FOO}/"
            root: abc
            
            Branch {
                id: workBranch
                name: "work"
                pattern: "work/"
                root: foo
                
                File {
                    //logLevel: Log.Debug
                    //UpdateOperation.logLevel: Log.Error
                    id: workFile
                    name: "workFile"
                    pattern: "{FILENAME}"
                }

                CookQueue {
                    //logLevel: Log.Debug
                    //UpdateOperation.logLevel: Log.Error
                    //CookOperation.logLevel: Log.Error
                    id: copier
                    name: "copier"
                    count: input ? input.elements.length : 0
                    
                    property File input: workFile
                    property var elements: []

                    component: Component {
                        PugItem {
                            id: item
                            property int index
                            
                            signal cook(var env)
                            signal cooked(int status)
                            
                            onCook: {
                                debug("onCook===");
                                
                                debug("input is " + copier.input);
                                debug("index is " + index);
                                debug("input.elements[index] is " + copier.input.elements[index]);
                                debug("input.elements[index].path is " + copier.input.elements[index].path);
                                var inputPath = copier.input.elements[index].path;
                                var outputDir = copier.input.elements[index].directory + "temp/";
                                var outputPath = outputDir + "temp_" + copier.input.elements[index].baseName + "." + copier.input.elements[index].extension;
    
                                debug("--- creating " + outputPath + " from " + inputPath);
                                if (!Util.exists(outputDir))
                                    Util.mkpath(outputDir);
                                Util.copy(inputPath, outputPath);
                                
                                var newElement = Util.createElement(copier, {pattern: outputPath, data: copier.input.elements[index].data});
                                copier.elements.push(newElement);
                                debug(item + " sending cooked(" + Operation.Finished + ")");
                                item.cooked(Operation.Finished);
                            }
                        }
                    }

                    Property {
                        name: "input"
                        input: true
                    }
                }
                
                File {
                    //logLevel: Log.Debug
                    //UpdateOperation.logLevel: Log.Error
                    id: cookFile
                    name: "cookFile"
                    pattern: "cooked/{FILENAME}"
                    input: copier
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
        id: cookSpy
        target: cook
        signalName: "finished"
    }

    function test_cookFile() {
        var env = {ROOT: tmpDir, FOO: "foo"};
        var workPaths = [tmpDir + "cooktests/abc/foo/work/baa.txt",
                         tmpDir + "cooktests/abc/foo/work/bar.txt",
                         tmpDir + "cooktests/abc/foo/work/baz.txt"];
        var cookPaths = [tmpDir + "cooktests/abc/foo/work/cooked/baa.txt",
                         tmpDir + "cooktests/abc/foo/work/cooked/bar.txt",
                         tmpDir + "cooktests/abc/foo/work/cooked/baz.txt"];

        update.run(cookFile, env);
        updateSpy.wait(500);
        compare(workFile.UpdateOperation.status, Operation.Finished);
        compare(cookFile.UpdateOperation.status, Operation.Finished);
        compare(workFile.elements[0].path, workPaths[0]);
        compare(workFile.elements[1].path, workPaths[1]);
        compare(workFile.elements[2].path, workPaths[2]);
        
        cook.run(cookFile, env);
        cookSpy.wait(1000);
        compare(cookFile.count, workFile.count);
        compare(workFile.CookOperation.status, Operation.Finished);
        compare(copier.CookOperation.status, Operation.Finished);
        compare(cookFile.CookOperation.status, Operation.Finished);
        compare(workFile.CookOperation.status, Operation.Finished);
        compare(cookFile.elements.length, 3);
        compare(cookFile.elements.length, 3);
        compare(cookFile.elements[0].path, cookPaths[0]);
        compare(cookFile.elements[1].path, cookPaths[1]);
        compare(cookFile.elements[2].path, cookPaths[2]);
    }
}
