import QtQuick 2.0
import Pug 1.0

Node {
    id: self
    property int count: 0
    property Component component
    property var __items: []

    signal itemAdded(int index, var item)
    signal itemRemoved(int index, var item)
    
    onCountChanged: {
        copious("onCountChanged");
        __regenerate();
    }
    
    onComponentChanged: {
        copious("onComponentChanged");
        for (var i = count; i < __items.length; i++) {
            itemRemoved(i, __items[i]);
            __items[i].destroy();
        }
        
        __items = [];
        
        __regenerate();
    }
    
    function __regenerate() {
        for (var i = count; i < __items.length; i++) {
            itemRemoved(i, __items[i]);
            __items[i].destroy();
        }
            
        var newItems = [];
        for (var i = __items.length; i < count; i++)
            newItems.push(component.createObject(self, {index: i}));

        // this werebeast removes the old items and inserts the new items in
        // one go
        Array.prototype.splice.apply(__items, 
            [Math.min(count, __items.length), 
             Math.max(__items.length-count, 0)].concat(newItems));
        
        for (var i = __items.length-newItems.length; i < __items.length; i++)
            itemAdded(i, __items[i]);
    }
    
    function itemAt(index) {
        if (index >= 0 && index < __items.length)
            return __items[index];
        else
            return null;
    }
}
