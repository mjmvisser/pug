import QtQuick 2.0
import QtTest 1.0
import Pug 1.0
import ShotgunEntities 1.0
import MokkoTools 1.0

import "../3rdparty/js/sprintf.js" as Sprintf

PugTestCase {
    name: "ShotgunEntityTests"

    property string testImage: Qt.resolvedUrl("SMPTE_Color_Bars_16x9.png").replace("file://", "")

    function init() {
        //Shotgun.logLevel = Log.Trace;
        Shotgun.baseUrl = "https://mokko.shotgunstudio.com";
        Shotgun.apiKey = "ef0623879e54c9f4f2d80f9502a7adea09bfcf8f";
        Shotgun.scriptName = "test";
        
        Util.mkpath(tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/work/mvisser");
        Util.touch(tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/work/mvisser/somefile.txt");
        Util.mkpath(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/images");
        for (var frame=1; frame <= 100; frame++) {
            var framePath = Sprintf.sprintf(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/images/somefile.%04d.jpg", frame);
            Util.copy(testImage, framePath);
        }
        updateSpy.clear();
        releaseSpy.clear();
    }

    function cleanup() {
        Util.remove(tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/release/main/v001/somefile.txt");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/release/main/v001");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/release/main");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/release");
        Util.remove(tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/work/mvisser/somefile.txt");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/work/mvisser");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/work");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/delivery/from_client");
        Util.rmdir(tmpDir + "shotguntests/projects/888_test/delivery");
        for (var frame=1; frame <= 100; frame++) {
            var workFramePath = Sprintf.sprintf(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/images/somefile.%04d.jpg", frame);
            Util.remove(workFramePath);
            var releaseFramePath = Sprintf.sprintf(tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/release/main/v001/render/jpg/DEV_001_comp.%04d.jpg", frame);
            Util.remove(releaseFramePath);
        }
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
        //logLevel: Log.Info
        id: root

        operations: [
            UpdateOperation {
                id: update
            },
            CookOperation {
                id: cook
                name: "cook"
                dependencies: update
            },
            ReleaseOperation {
                id: release
                dependencies: cook
                triggers: cook
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

        Folder {
            id: prod
            name: "prod"
            pattern: "{ROOT}/"
            
            ShotgunHumanUser {
                id: sg_user
            }
        }
        
        Folder {
            id: project
            name: "project"
            pattern: "projects/{PROJECT}/"
            root: prod
            
            ShotgunProject {
                id: sg_project
                name: "sg_project"
            }
        }

        Folder {
            id: delivery
            name: "delivery"
            root: project
            pattern: "delivery/from_client/{TRANSFER}/"

            ShotgunStep {
                id: sg_deliveryStep
                step: "turnover" 
            }
            
            ShotgunDelivery {
                id: sg_delivery
                project: sg_project
            }

            Folder {
                id: deliveryRelease
                name: "deliveryRelease"
                pattern: "release/{VARIATION}/v{VERSION}/"
                ReleaseOperation.versionField: "VERSION"

                ShotgunVersion {
                    id: sg_deliveryVersion
                    action: ShotgunEntity.Create
                    project: sg_project
                    link: sg_delivery
                    step: sg_deliveryStep
                    user: sg_user
                    code: "from_client_{TRANSFER}_v{VERSION}"
                }
                
                File {
                    id: releaseFile
                    name: "releaseFile"
                    pattern: "{FILENAME}.{EXT}"
                    ReleaseOperation.source: workFile

                    ShotgunPublishEvent {
                        id: sg_releaseFile
                        name: "sg_releaseSeqFile"
                        action: ShotgunEntity.Create
                        project: sg_project
                        user: sg_user
                        link: sg_delivery
                        step: sg_deliveryStep
                        code: "from_client_{TRANSFER}_v{VERSION}"
                    }
                }
            }

            Folder {
                id: deliveryWork
                name: "deliveryWork"
                pattern: "work/{USER}/"

                File {
                    id: workFile
                    name: "workFile"
                    pattern: "{FILENAME}.{EXT}"
                }
            }
        }
        
        Folder {
            id: seq
            name: "seq"
            pattern: "shots/{SEQUENCE}/"
            root: project
            
            ShotgunScene {
                id: sg_seq
                project: sg_project
            }
        }
        
        Folder {
            id: shot
            name: "shot"
            pattern: "{SHOT}/"
            root: seq

            ShotgunShot {
                id: sg_shot
                project: sg_project
                scene: sg_seq
            }
        }
        
        Folder {
            id: comp
            name: "comp"
            pattern: "{STEP}/"
            root: shot

            ShotgunStep {
                id: sg_compStep
                step: "comp"
            }
            
            Folder {
                id: compRelease
                name: "compRelease"
                pattern: "release/{VARIATION}/v{VERSION}/"
                ReleaseOperation.versionField: "VERSION"
                ReleaseOperation.source: compWork

                ShotgunVersion {
                    id: sg_compRelease
                    action: ShotgunEntity.Create
                    name: "sg_compRelease"
                    project: sg_project
                    link: sg_shot
                    user: sg_user
                    step: sg_compStep
                    code: "{SEQUENCE}_{SHOT}_comp_v{VERSION}"
                }
                
                File {
                    id: releaseSeq
                    name: "releaseSeq"
                    pattern: "render/jpg/{SEQUENCE}_{SHOT}_{STEP}.{FRAME}.{EXT}"
                    ReleaseOperation.source: workSeq
                
                    ShotgunPublishEvent {
                        id: sg_releaseSeqFile
                        name: "sg_releaseSeqFile"
                        action: ShotgunEntity.Create
                        project: sg_project
                        user: sg_user
                        link: sg_shot
                        step: sg_compStep
                        code: "{SEQUENCE}_{SHOT}_comp_v{VERSION}"
                    }
                }
            }
            
            Folder {
                id: compWork
                name: "compWork"
                pattern: "work/{USER}/"
                
                File {
                    id: workSeq
                    name: "workSeq"
                    pattern: "images/{FILENAME}.{FRAME}.{EXT}"
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
        id: releaseSpy
        target: release
        signalName: "finished"
    }

    function skip_test_1updateFile() {
        // TODO: why does this fail if it runs AFTER test_releaseFile?
        var context = {PROJECT: "888_test",
                   TRANSFER: "20121018",
                   USER: "mvisser",
                   FILENAME: "somefile",
                   EXT: "txt"};
        var workPath = tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/work/mvisser/somefile.txt";

        verify(Util.exists(workPath));
        compare(workFile.map(context), workPath);
        
        update.run(releaseFile, context);
        updateSpy.wait(10000);
        
        compare(workFile.UpdateOperation.status, Operation.Finished);
        compare(delivery.UpdateOperation.status, Operation.Finished);
        compare(project.UpdateOperation.status, Operation.Finished);
        compare(releaseFile.UpdateOperation.status, Operation.Finished);

        compare(sg_project.details.length, 1);
        verify(sg_project.details[0].entity);
        compare(sg_project.details[0].entity.type, "Project");
        compare(sg_delivery.details.length, 1);
        verify(sg_delivery.details[0].entity);
        compare(sg_delivery.details[0].entity.type, "Delivery");
        
        compare(workFile.details.length, 1);
        compare(workFile.details[0].element.pattern, workPath);
        compare(sg_deliveryVersion.details.length, 0);
        compare(sg_releaseFile.details.length, 0);
        compare(releaseFile.details.length, 0);
    }

    function test_releaseFile() {
        var context = {PROJECT: "888_test",
                   TRANSFER: "20121018",
                   USER: "mvisser",
                   FILENAME: "somefile",
                   EXT: "txt"};
        var workPath = tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/work/mvisser/somefile.txt";
        var releasePath = tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/release/main/v001/somefile.txt";

        verify(deliveryRelease.parse(tmpDir + "shotguntests/projects/888_test/delivery/from_client/20121018/release/main/v001/"));

        verify(Util.exists(workPath));
        compare(workFile.map(context), workPath);
        
        release.run(releaseFile, context);
        releaseSpy.wait(10000);
        
        compare(workFile.UpdateOperation.status, Operation.Finished);
        compare(delivery.UpdateOperation.status, Operation.Finished);
        compare(project.UpdateOperation.status, Operation.Finished);
        compare(releaseFile.ReleaseOperation.status, Operation.Finished);
        compare(sg_project.UpdateOperation.status, Operation.Finished);
        compare(sg_project.ReleaseOperation.status, Operation.Finished);
        compare(sg_delivery.UpdateOperation.status, Operation.Finished);
        compare(sg_delivery.ReleaseOperation.status, Operation.Finished);
        compare(sg_deliveryVersion.UpdateOperation.status, Operation.Finished);
        compare(sg_deliveryVersion.ReleaseOperation.status, Operation.Finished);
        compare(sg_releaseFile.UpdateOperation.status, Operation.Finished);
        compare(sg_releaseFile.ReleaseOperation.status, Operation.Finished);

        verify(Util.exists(releasePath));
        
        compare(sg_project.details.length, 1);
        compare(sg_project.details[0].entity.type, "Project");
        compare(sg_delivery.details.length, 1);
        compare(sg_delivery.details[0].entity.type, "Delivery");
        compare(sg_deliveryVersion.details.length, 1);
        compare(sg_deliveryVersion.details[0].entity.type, "Version");
        compare(sg_releaseFile.details.length, 1);
        compare(sg_releaseFile.details[0].entity.type, "PublishEvent");

        compare(workFile.details.length, 1);
        compare(workFile.details[0].element.pattern, workPath);
        compare(releaseFile.details.length, 1);
        compare(releaseFile.details[0].element.pattern, releasePath);
    }

    function test_releaseSeq() {
        var context = {ROOT: tmpDir + "shotguntests",
                       PROJECT: "888_test",
                       SEQUENCE: "DEV",
                       SHOT: "001",
                       STEP: "comp",
                       USER: "mvisser",
                       FILENAME: "somefile",
                       EXT: "jpg",
                       PUGWORK: tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/pugtemp/"};
        var startFrame = 1;
        var endFrame = 100;
        var workPath = tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/work/mvisser/images/somefile.%04d.jpg";
        var releasePath = tmpDir + "shotguntests/projects/888_test/shots/DEV/001/comp/release/main/v001/render/jpg/DEV_001_comp.%04d.jpg";

        for (var frame = startFrame; frame <= endFrame; frame++) {
            var framePath = Sprintf.sprintf(workPath, frame);
            verify(Util.exists(framePath));
        }

        // check that the config is good
        update.run(compWork, context);
        updateSpy.wait(10000);
    
        compare(update.status, Operation.Finished);
    
        compare(workSeq.details.length, 1);
        compare(workSeq.details[0].element.pattern, workPath);

        release.run(releaseSeq, context);
        releaseSpy.wait(10000);
        
        compare(update.status, Operation.Finished);
        compare(cook.status, Operation.Finished);
        compare(release.status, Operation.Finished);
        
        compare(releaseSeq.details.length, 1);
        
        compare(sg_releaseSeqFile.details.length, 1);
        compare(compRelease.details.length, 1);
        
        compare(sg_compRelease.details.length, 1);
        compare(sg_releaseSeqFile.details.length, 1);
    }
    
    function skip_test_pullFields() {
        var context = {ROOT: tmpDir + "shotguntests",
                       PROJECT: "888_test",
                       SEQUENCE: "DEV",
                       SHOT: "001",
                       USER: "mvisser"};
        update.run(shot, context);
        updateSpy.wait(5000);
        
        compare(shot.details.length, 1);
        compare(sg_shot.details.length, 1);
        compare(sg_shot.details[0].entity.sg_head_in, 1);                
        compare(sg_shot.details[0].entity.sg_tail_out, 100);                
    }
}
