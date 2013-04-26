import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "ShotgunUpdateTests"

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

        ShotgunFind {
            id: findPublishEvent
            name: "findPublishEvent"
            
            property var project: findProject
            property var delivery: findDelivery

            inputs: [
                Input { name: "project" },
                Input { name: "delivery" }
            ]

            entityType: "PublishEvent"
            filters: [["project", "is", project.details[0].entity], ["publish_event_links", "is", delivery.details[0].entity]]
            fields: ["code"]
            limit: 1
        }
        
        ShotgunUpdate {
            id: updatePublishEvent

            property var publishEvent: findPublishEvent

            inputs: [
                Input { name: "publishEvent" }
            ]
            
            entityType: "PublishEvent"
            entityId: publishEvent.details[0].entity.id
            data: {"code": publishEvent.details[0].entity.code + "A"}
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
        update.run(updatePublishEvent, {});
        updateSpy.wait(5000);
        
        verify(updatePublishEvent.data);
        compare(updatePublishEvent.data.code, findPublishEvent.details[0].entity.code + "A");
    }
    
    function test_shotgunUpdate() {
        release.run(updatePublishEvent, {});
        releaseSpy.wait(5000);
        
        compare(updatePublishEvent.details.length, 1);
        compare(updatePublishEvent.details[0].context, {});
        compare(updatePublishEvent.details[0].entity.type, "PublishEvent");
        compare(updatePublishEvent.details[0].entity.code, findPublishEvent.details[0].entity.code + "A");
    }
}
