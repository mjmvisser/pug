import QtQuick 2.0
import QtTest 1.0
import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

import "../3rdparty/js/sprintf.js" as Sprintf

PugTestCase {
    name: "ShotgunOperationTests"

    property string testImage: Qt.resolvedUrl("SMPTE_Color_Bars_16x9.png").replace("file://", "")

    function init() {
        Util.mkpath(tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/work/mvisser");
        Util.touch(tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/work/mvisser/somefile.txt");
        Util.mkpath(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/images");
        for (var frame=1; frame <= 100; frame++) {
            var framePath = Sprintf.sprintf(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/images/somefile.%04d.jpg", frame);
            Util.copy(testImage, framePath);
        }
    }

    function cleanup() {
        Util.remove(tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/release/main/v001/somefile.txt");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/release/main/v001");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/release/main");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/release");
        Util.remove(tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/work/mvisser/somefile.txt");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/work/mvisser");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/work");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/transfer/from_client");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/transfer");
        for (var frame=1; frame <= 100; frame++) {
            var workFramePath = Sprintf.sprintf(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/images/somefile.%04d.jpg", frame);
            Util.remove(workFramePath);
            var releaseFramePath = Sprintf.sprintf(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/release/main/v001/render/jpg/DEV_001_comp.%04d.jpg", frame);
            Util.remove(releaseFramePath);
        }
        Util.remove(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/images/somefile_thumbnail.jpg");
        Util.remove(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/images/somefile_filmstrip.jpg");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/images");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/release/main/v001/render/jpg");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/release/main/v001/render");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/release/main/v001");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/release/main");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/release");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots/DEV/001");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots/DEV");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/shots");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test");
        Util.rmdir(tmpDir + "shotguntests/projects");
        Util.rmdir(tmpDir + "shotguntests");
    }
    
    Root {
        id: root

        Shotgun {
            id: shotgun
            baseUrl: "https://mokko.shotgunstudio.com"
            apiKey: "ef0623879e54c9f4f2d80f9502a7adea09bfcf8f"
            scriptName: "test"
        }
        
        operations: [
            UpdateOperation {
                id: update
            },
            ShotgunOperation {
                id: shotgunPull
                name: "shotgunPull"
                mode: ShotgunOperation.Pull
                shotgun: shotgun
                dependencies: update
            },
            CookOperation {
                id: cook
                name: "cook"
                dependencies: shotgunPull
            },
            ReleaseOperation {
                id: release
                dependencies: cook
                triggers: shotgunPush
            },
            ShotgunOperation {
                id: shotgunPush
                name: "shotgunPush"
                mode: ShotgunOperation.Push
                shotgun: shotgun
            }
        ]
    
        fields: [
            Field { name: "ROOT"; values: tmpDir + "shotguntests"; defaultValue: tmpDir + "shotguntests" },
            Field { name: "PROJECT" },
            Field { name: "SEQUENCE" },
            Field { name: "SHOT" },
            Field { name: "STEP" },
            Field { name: "TRANSFER"; regexp: "20\\d{6}" },
            Field { name: "VERSION"; type: Field.Integer; width: 3 },
            Field { name: "VARIATION"; defaultValue: "main" },
            Field { name: "USER" },
            Field { name: "FILENAME" },
            FrameSpecField { name: "FRAME" },
            Field { name: "EXT" }
        ]
        
        Branch {
            id: prod
            name: "prod"
            pattern: "{ROOT}/"
            
            ShotgunEntity {
                name: "HumanUser"
                
                ShotgunField {
                    name: "login"
                    field: "USER"
                }
            }
        }
        
        Branch {
            id: project
            name: "project"
            pattern: "projects/{PROJECT}/"
            root: prod
            
            ShotgunEntity {
                name: "Project"
                
                ShotgunField {
                    name: "name"
                    field: "PROJECT"
                } 
            }
        }
        
        Branch {
            id: transfer
            name: "transfer"
            root: project
            pattern: "transfer/from_client/{TRANSFER}/"

            ShotgunDelivery {
                project: project
            }

            Branch {
                id: transferRelease
                name: "transferRelease"
                pattern: "release/{VARIATION}/v{VERSION}/"
                ReleaseOperation.versionField: "VERSION"
                ShotgunOperation.action: ShotgunOperation.Create

                ShotgunPublishEvent {
                    project: project
                    entityType: "Delivery"
                    entity: transfer
                    user: prod
                    code: "from_client_{TRANSFER}_v{VERSION}"
                }
                
                File {
                    id: releaseFile
                    name: "releaseFile"
                    pattern: "{FILENAME}.{EXT}"
                    ShotgunOperation.action: ShotgunOperation.Create

                    ShotgunFile {
                        project: project
                        release: transferRelease
                        user: prod
                    }

                    Node {
                        id: releaseFileElement
                        // dummy node to force a dependency on releaseFile
                        output: true
                        ShotgunOperation.action: ShotgunOperation.Create
                        
                        ShotgunElement {
                            project: project
                            delivery: transfer
                            release: transferRelease
                            user: prod
                            code: "{FILENAME}.{EXT}"   
                            //sourcePathLink: releaseFile   // TODO:           
                        }
                    }
                    
                    ShotgunVersion {
                        project: project
                        entityType: "Delivery"
                        entity: transfer
                        release: transferRelease
                        user: prod
                        code: "from_client_{TRANSFER}_v{VERSION}"
                    }
                }
            }

            Branch {
                id: transferWork
                name: "transferWork"
                pattern: "work/{USER}/"

                File {
                    id: workFile
                    name: "workFile"
                    pattern: "{FILENAME}.{EXT}"
                    ReleaseOperation.target: node("../../transferRelease/releaseFile")
                }
            }
        }
        
        Branch {
            id: seq
            name: "seq"
            pattern: "shots/{SEQUENCE}/"
            root: project
            
            ShotgunScene {
                project: project
            }
        }
        
        Branch {
            id: shot
            name: "shot"
            pattern: "{SHOT}/"
            root: seq

            ShotgunShot {
                project: project
                scene: seq
            }
        }
        
        Branch {
            id: comp
            name: "comp"
            pattern: "{STEP}/"
            root: shot

            ShotgunStep {
            }
            
            Branch {
                id: compRelease
                name: "compRelease"
                pattern: "release/{VARIATION}/v{VERSION}/"
                ReleaseOperation.versionField: "VERSION"
                ShotgunOperation.action: ShotgunOperation.Create

                ShotgunPublishEvent {
                    project: project
                    entityType: "Shot"
                    entity: shot
                    user: prod
                    code: "{SEQUENCE}_{SHOT}_comp_v{VERSION}"
                }
                
                File {
                    id: releaseSeq
                    name: "releaseSeq"
                    pattern: "render/jpg/{SEQUENCE}_{SHOT}_{STEP}.{FRAME}.{EXT}"
                    ShotgunOperation.action: ShotgunOperation.Create
                
                    ShotgunVersion {
                        project: project
                        entityType: "Shot"
                        entity: shot
                        release: compRelease
                        user: prod
                        code: "{SEQUENCE}_{SHOT}_{STEP}_{VARIATION}_v{VERSION}"
                        thumbnail: workSeqThumbnail
                        filmstrip: workSeqFilmstrip
                    }

                    ShotgunFile {
                        project: project
                        release: compRelease
                        user: prod
                        thumbnail: workSeqThumbnail
                        filmstrip: workSeqFilmstrip
                    }
                }
            }
            
            Branch {
                id: compWork
                name: "compWork"
                pattern: "work/{USER}/"
                
                File {
                    id: workSeq
                    name: "workSeq"
                    pattern: "images/{FILENAME}.{FRAME}.{EXT}"
                    ReleaseOperation.target: node("../../transferRelease/releaseSeq")
                    output: true
                }
                
                MakeThumbnail {
                    id: workSeqThumbnail
                    name: "workSeqThumbnail"
                    input: workSeq
                    output: true
                }

                MakeThumbnail {
                    id: workSeqFilmstrip
                    name: "workSeqFilmstrip"
                    input: workSeq
                    filmstrip: true
                    output: true
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
        id: pullSpy
        target: shotgunPull
        signalName: "finished"
    }
    
    SignalSpy {
        id: releaseSpy
        target: release
        signalName: "finished"
    }

    function test_releaseFile() {
        var env = {PROJECT: "888_test",
                   TRANSFER: "20121018",
                   USER: "mvisser",
                   FILENAME: "somefile",
                   EXT: "txt"};
        var workPath = tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/work/mvisser/somefile.txt";
        var releasePath = tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/release/main/v001/somefile.txt";

        var env2 = env;
        env2.VERSION = 1;

        // compare(releaseFile.root, transferRelease);
        // compare(transferRelease.map(env2), tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/release/main/v001/");
        verify(transferRelease.parse(tmpDir + "shotguntests/projects/888_test/transfer/from_client/20121018/release/main/v001/"));

        return;

        verify(Util.exists(workPath));
        compare(workFile.map(env), workPath);
        
        release.run(workFile, env);
        releaseSpy.wait(10000);
        
        verify(Util.exists(releasePath));
        
        compare(workFile.UpdateOperation.status, Operation.Finished);
        compare(transfer.UpdateOperation.status, Operation.Finished);
        compare(project.UpdateOperation.status, Operation.Finished);
        compare(workFile.ShotgunOperation.status, Operation.Finished);
        compare(transfer.ShotgunOperation.status, Operation.Finished);
        compare(project.ShotgunOperation.status, Operation.Finished);
        compare(transfer.ShotgunOperation.status, Operation.Finished);
        compare(releaseFile.ShotgunOperation.status, Operation.Finished);

        compare(project.ShotgunOperation.entities.Project.type, "Project");
        compare(transfer.ShotgunOperation.entities.Delivery.type, "Delivery");
        compare(transferRelease.ShotgunOperation.entities.PublishEvent.type, "PublishEvent");
        compare(releaseFileElement.ShotgunOperation.entities.Element.type, "Element");
        compare(releaseFile.ShotgunOperation.entities.Attachment.type, "Attachment");
        compare(releaseFile.ShotgunOperation.entities.Version.type, "Version");
        compare(transferRelease.ShotgunOperation.entities.PublishEvent.type, "PublishEvent");

        compare(workFile.details.length, 1);
        compare(workFile.details[0].element.path, workPath);
        compare(releaseFile.details.length, 1);
        compare(releaseFile.details[0].element.path, releasePath);
    }

    function test_releaseSeq() {
        var env = {ROOT: tmpDir + "shotguntests",
                   PROJECT: "888_test",
                   SEQUENCE: "DEV",
                   SHOT: "001",
                   STEP: "comp",
                   USER: "mvisser",
                   FILENAME: "somefile",
                   EXT: "jpg"};
        var startFrame = 1;
        var endFrame = 100;
        var workPath = tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/images/somefile.%04d.jpg";
        var releasePath = tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/release/main/v001/render/jpg/DEV_001_comp.%04d.jpg";

        for (var frame = startFrame; frame <= endFrame; frame++) {
            var framePath = Sprintf.sprintf(workPath, frame);
            verify(Util.exists(framePath));
        }

        // check that the config is good
        update.run(compWork, env);
        updateSpy.wait(1000);
    
        compare(workSeq.details.length, 1);
        compare(workSeq.details[0].element.pattern, workPath);

        release.run(compWork, env);
        releaseSpy.wait(10000);
    }
    
    function test_pullFields() {
        var env = {ROOT: tmpDir + "shotguntests",
                   PROJECT: "888_test",
                   SEQUENCE: "DEV",
                   SHOT: "001",
                   USER: "mvisser"};
        shotgunPull.run(shot, env);
        pullSpy.wait(5000);
        
        compare(shot.details[0].entities.Shot.sg_head_in, 1);                
        compare(shot.details[0].entities.Shot.sg_tail_out, 100);                
    }
}
