import QtQuick 2.0
import Pug 1.0

Repeater {
    id: self
    
    property int __limit: 4 // TODO: provide a Util.idealThreadCount or something
    property var __cooking
    property var __pending
    property var __env

    signal cook(var env)
    signal cooked(int status)
    signal itemCooked(int status)

    onItemCooked: {
        debug(sender() + " itemCooked " + status);
        // remove the item
        __cooking.splice(__cooking.indexOf(sender()), 1);
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
        for (var i = 0; i < self.count; i++) {
            __pending.push(itemAt(i));
        }
        
        continueRunning();
    }
    
    function continueRunning() {
        debug("continueRunning status=" + self.CookOperation.status + " __pending: " + __pending + " __cooking: " + __cooking);
        if (self.CookOperation.status != Operation.Error) {
            while (__pending.length > 0 && __cooking.length < __limit) {
                var process = __pending.shift();
                __cooking.push(process);
                process.cook(__env);
            }
            
            if (__cooking.length === 0 && __pending.length == 0)
                self.cooked(Operation.Finished);
        }
    }
} 