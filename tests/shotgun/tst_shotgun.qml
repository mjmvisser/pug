import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "ShotgunTests"

    function init() {
        //Shotgun.logLevel = Log.Warning;
        Shotgun.baseUrl = "https://mokko.shotgunstudio.com";
        Shotgun.apiKey = "ef0623879e54c9f4f2d80f9502a7adea09bfcf8f";
        Shotgun.scriptName = "test";
    }
        
    SignalSpy {
        id: spy
    }

    function test_shotgunInfo() {
        var reply = Shotgun.info();
        spy.target = reply;
        spy.signalName = "finished";
        spy.wait(5000);
        verify(reply.results.version);
    }
    
    function test_shotgunFindOne() {
        var reply = Shotgun.findOne("Project", [["name", "is", "888_test"]]);
        spy.target = reply;
        spy.signalName = "finished";
        spy.wait(5000);
        compare(reply.results.type, "Project");
    }
    
    function test_shotgunFindOneNotFound() {
        var reply = Shotgun.findOne("Project", [["name", "is", "SLsdf8k3"]]);
        spy.target = reply;
        spy.signalName = "finished";
        spy.wait(5000);
        compare(reply.results, undefined);
    }
    
    function test_shotgunFindOneInvalidFilter() {
        var reply = Shotgun.findOne("Project", [["prodfsd", "is", "SLsdf8k3"]]);
        spy.target = reply;
        spy.signalName = "error";
        spy.wait(5000);
        compare(reply.errorCode, ShotgunReply.ProtocolFailure);
    }
    
    function test_shotgunFind() {
        var reply = Shotgun.find("Project", []);
        spy.target = reply;
        spy.signalName = "finished";
        spy.wait(5000);
        for (var i = 0; i < reply.results.length; i++) {
            var entity = reply.results[i];
            compare(entity.type, "Project");
        }
    }
    
    function test_shotgunCreate() {
        var reply;
        reply = Shotgun.findOne("Project", [["name", "is", "888_test"]]);
        spy.target = reply;
        spy.signalName = "finished";
        spy.wait(5000);
        var project = reply.results;
        compare(project.type, "Project");
       
        reply = Shotgun.findOne("Delivery", [["title", "is", "test"]]);
        spy.target = reply;
        spy.signalName = "finished";
        spy.wait(5000);
        var delivery = reply.results;
        compare(delivery.type, "Delivery");
       
        var publishEvent = {"code": "test",
                            "project": project,
                            "publish_event_links": [delivery]};
        var reply = Shotgun.create("PublishEvent", publishEvent, ["created_at"]);
        spy.target = reply;
        spy.signalName = "finished";
        spy.wait(5000);
       
        compare(reply.results.code, "test");
        compare(reply.results.project.type, project.type);
        compare(reply.results.project.id, project.id);
        compare(reply.results.publish_event_links[0].type, delivery.type);
        compare(reply.results.publish_event_links[0].id, delivery.id);
        
        return reply.results;
    }

    function test_shotgunBatchCreate() {
        var reply;
        reply = Shotgun.findOne("Project", [["name", "is", "888_test"]]);
        spy.target = reply;
        spy.signalName = "finished";
        spy.wait(5000);
        var project = reply.results;
        compare(project.type, "Project");
       
        reply = Shotgun.findOne("Delivery", [["title", "is", "test"]]);
        spy.target = reply;
        spy.signalName = "finished";
        spy.wait(5000);
        var delivery = reply.results;
        compare(delivery.type, "Delivery");
       
        
        var requests = [{"request_type": "create",
                         "entity_type": "PublishEvent",
                         "data": {"code": "test1",
                                  "project": project,
                                  "publish_event_links": [delivery]}},
                        {"request_type": "create",
                         "entity_type": "PublishEvent",
                         "data": {"code": "test2",
                                  "project": project,
                                  "publish_event_links": [delivery]}}];
        var reply = Shotgun.batch(requests);
        spy.target = reply;
        spy.signalName = "finished";
        spy.wait(5000);
        
        compare(reply.results.length, 2);
        compare(reply.results[0].type, "PublishEvent");
        compare(reply.results[1].type, "PublishEvent");
    }
    
    function test_shotgunUploadThumbnail() {
        var entity = test_shotgunCreate();

        var reply = Shotgun.uploadThumbnail(entity.type, entity.id, "thumbnail.jpg");
        spy.target = reply;
        spy.signalName = "finished";
        spy.wait(10000);
    }
}
