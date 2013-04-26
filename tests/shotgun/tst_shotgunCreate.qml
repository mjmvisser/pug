import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "ShotgunCreateTests"

    function init() {
        //Shotgun.logLevel = Log.Warning;
        Shotgun.baseUrl = "https://mokko.shotgunstudio.com";
        Shotgun.apiKey = "ef0623879e54c9f4f2d80f9502a7adea09bfcf8f";
        Shotgun.scriptName = "test";
    }
        
    Root {
        //logLevel: Log.Info
        id: root

        operations: [
            UpdateOperation {
                id: update
                name: "update"
            },
            ReleaseOperation {
                id: release
                name: "release"
                dependencies: update
            }
        ]
        
        ShotgunFind {
            id: findProject
            name: "findProject"
            entityType: "Project"
            filters: [["name", "is", "888_test"]]
            fields: ["name", "sg_description", "sg_full_name"]
        }
        
        ShotgunFind {
            id: findDelivery
            name: "findDelivery"

            property var project: findProject
            inputs: Input { name: "project" }

            entityType: "Delivery"
            filters: [["project", "is", project.details[0].entity], ["title", "is", "test"]]
        }
        
        ShotgunCreate {
            id: createPublishEvent

            property var project: findProject
            property var delivery: findDelivery

            inputs: [
                Input { name: "project" },
                Input { name: "delivery" }
            ]
            
            entityType: "PublishEvent"
            data: {
                if (project.count == 1 && delivery.count > 0) {
                    return {"code": "test", 
                     "project": project.details[0].entity, 
                     "publish_event_links": [delivery.details[0].entity]};                    
                } else {
                    return null;
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
    
    function test_shotgunFind() {
        // make sure the finds are working
        update.run(createPublishEvent, {});
        updateSpy.wait(5000);
        
        verify(createPublishEvent.data);
        compare(createPublishEvent.data.code, "test");
        verify(createPublishEvent.data.project);
        compare(createPublishEvent.data.project.type, "Project");
        compare(createPublishEvent.data.publish_event_links.length, 1);
        compare(createPublishEvent.data.publish_event_links[0].type, "Delivery");
    }
    
    function test_shotgunCreate() {
        release.run(createPublishEvent, {});
        releaseSpy.wait(5000);
        
        compare(createPublishEvent.details.length, 1);
        compare(createPublishEvent.details[0].context, {});
        verify(createPublishEvent.details[0].entity);
        verify(createPublishEvent.details[0].entity.type);
        compare(createPublishEvent.details[0].entity.type, "PublishEvent");
        compare(createPublishEvent.details[0].entity.code, "test");
        compare(createPublishEvent.details[0].entity.project.type, "Project");
        compare(createPublishEvent.details[0].entity.project.id, findProject.details[0].entity.id);
        verify(createPublishEvent.details[0].entity.publish_event_links, 1);
        compare(createPublishEvent.details[0].entity.publish_event_links.length, 1);
        verify(createPublishEvent.details[0].entity.publish_event_links[0]);
        compare(createPublishEvent.details[0].entity.publish_event_links[0].type, "Delivery");
        compare(createPublishEvent.details[0].entity.publish_event_links[0].id, findDelivery.details[0].entity.id);
    }
}
