import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

TestCase {
    id: self
    name: "RepeaterTests"
    
    Repeater {
        id: repeater
        component: Component {
            PugItem {
                property int index
                property string foo: "foo" + index
            }
        }
    }
    
    function test_initialization() {
        repeater.count = 5
        for (var i = 0; i < repeater.count; i++)
            compare(repeater.itemAt(i).foo, "foo" + i);
    }
    
    function test_changeCount() {
        repeater.count = 0;
        compare(repeater.itemAt(0), null);
        
        var inc = 0;
        for (var c = 0; c < 50; inc++, c+=inc) {
            repeater.count = c;
            
            for (var i = 0; i < c; i++)
                compare(repeater.itemAt(i).foo, "foo" + i);
            
            compare(repeater.itemAt(c), null);
        }
        
        inc = 0;
        for (var c = repeater.count; c >= 0; inc++, c-=inc) {
            repeater.count = c;
            
            for (var i = 0; i < c; i++)
                compare(repeater.itemAt(i).foo, "foo" + i);
            
            compare(repeater.itemAt(c), null);
        }
    }
}