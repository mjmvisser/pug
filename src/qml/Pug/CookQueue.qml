import QtQuick 2.0
import Pug 1.0

Repeater {
    id: self
    
    property int __limit: 4 // TODO: provide a Util.idealThreadCount or something
    property var __cooked
    property var __cooking
    property var __pending
    property var __env
    property var __cookStatus

    signal cook(var env)
    signal cooked(int status)
    signal itemCooked(int status)

    onItemCooked: {
        debug(sender() + " itemCooked " + status);
        // remove the item
        var item = __cooking.splice(__cooking.indexOf(sender()), 1);
        __cooked.push(item);

        if (status > __cookStatus)
            __cookStatus = status;
        
        self.continueRunning();          
    }   
    
    onItemAdded: {
        debug("itemAdded: " + item);
        item.cooked.connect(self.itemCooked);
    }
    
    onItemRemoved: {
        item.cooked.disconnect(self.itemCooked);
    }
        
    onCook: {
        debug("CookQueue.onCook, count=" + self.count);
        __env = env;
        __pending = [];
        __cooking = [];
        __cooked = [];
        __cookStatus = Operation.Invalid
        for (var i = 0; i < self.count; i++) {
            __pending.push(itemAt(i));
        }
        
        continueRunning();
    }
    
    function continueRunning() {
        debug("continueRunning status=" + self.CookOperation.status + " __pending: " + __pending + " __cooking: " + __cooking);
        if (self.CookOperation.status != Operation.Error) {
            while (__pending.length > 0 && __cooking.length < __limit) {
                var item = __pending.shift();
                __cooking.push(item);
                debug("cooking " + item);
                item.cook(__env);
            }
            
            if (__cooking.length === 0 && __pending.length === 0) {
                debug("cooked with status " + __cookStatus);
                self.cooked(__cookStatus != Operation.Invalid ? __cookStatus : Operation.Error);
            }
        }
    }
} 