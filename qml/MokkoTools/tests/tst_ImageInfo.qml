import QtQuick 2.0
import QtTest 1.0
import Pug 1.0
import MokkoTools 1.0

TestCase {
    id: self
    name: "ImageInfoTests"

    property string testImagePath: Qt.resolvedUrl("data/test.png").replace("file://", "")
    
    Root {
        operations: [
            UpdateOperation {
                id: update
            },
            CookOperation {
                id: cook
                dependencies: update
            }
        ]
        
        File {
            id: file
            pattern: testImagePath
        }
        
        ImageInfo {
            id: info
            input: file
        }
    }
    
    SignalSpy {
        id: spy
        target: cook
        signalName: "finished"
    }
    
    function test_ImageInfo() {
        cook.run(info, {});
        spy.wait(500);
        
        compare(file.details.length, 1);
        compare(info.CookOperation.status, Operation.Finished);
        compare(info.details.length, 1);
        compare(info.details[0].element, file.details[0].element);
        compare(info.details[0].context.RESOLUTION, "82x104");
        compare(info.details[0].context.CODEC, "PNG");
        compare(info.details[0].context.DEPTH, "8");
        compare(info.details[0].context.COLORSPACE, "sRGB");
    }
}