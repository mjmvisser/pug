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
    
    signal __findFinished(var results)
    signal __findError()
    property var __reply
    
    UpdateOperation.onCook: {
        addTrace("UpdateOperation.onCook(" + JSON.stringify(context) + ")");
        index = 0;
        __reply = Shotgun.find(entityType, filters, fields, order, filterOperator, limit, retiredOnly, page);
        if (__reply) {
            __reply.finished.connect(__findFinished);
            __reply.error.connect(__findError);
        } else {
            addError("Shotgun.create failed");
            UpdateOperation.cookFinished();
        }
    }
    
    on__FindFinished: {
        addTrace("onFindFinished(" + JSON.stringify(results) + ")");

        count = results.length;
        for (index = 0; index < results.length; index++) {
            details[index].context = context; 
            details[index].entity = results[index];
        }
        
        detailsChanged();
        
        UpdateOperation.cookFinished();
    }
    
    on__FindError: {
        addError(__reply.errorString);
        UpdateOperation.cookFinished();
    }
}
