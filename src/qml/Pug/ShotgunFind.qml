import Pug 1.0

Node {
    id: self
    property var entityType
    property var filters
    property var fields: null
    property var order: null
    property var filterOperator: null
    property var limit: 0
    property var retiredOnly: false
    property var page: 0
    
    signal update(var context)
    signal updateFinished(int status)
    
    signal __findFinished(var results)
    signal __findError()
    
    property var __context;
    
    onUpdate: {
        addTrace("onUpdate(" + JSON.stringify(context) + ")");
        __context = context;
        var reply = Shotgun.find(entityType, filters, fields, order, filterOperator, limit, retiredOnly, page);
        reply.finished.connect(__findFinished);
        reply.error.connect(__findError);
    }
    
    on__FindFinished: {
        addTrace("onFindFinished(" + JSON.stringify(results) + ")");

        count = results.length;
        for (index = 0; index < results.length; index++) {
            details[index].context = __context; 
            details[index].entity = results[index];
        }
        
        detailsChanged();
        
        updateFinished(Operation.Finished);
    }
    
    on__FindError: {
        addError(sender().errorString());
        updateFinished(Operation.Error);
    }
}
