import Pug 1.0

Node {
    id: self
    property var requests
    
    signal release(var context)
    signal releaseFinished(int status)
    
    signal __batchFinished(var results)
    signal __batchError()
    
    property var __context;
    property var __reply;
    
    onRelease: {
        addTrace("onRelease(" + JSON.stringify(context) + ")");
        __context = context;
        var reply = Shotgun.batch(requests);
        __reply = reply;
        reply.finished.connect(__batchFinished);
        reply.error.connect(__batchError);
    }
    
    on__BatchFinished: {
        addTrace("onBatchFinished(" + JSON.stringify(results) + ")");

        count = results.length;
        for (index = 0; index < results.length; index++) {
            details[index].context = __context;
            if (requests[index].request_type == "delete") {
                details[index].result = results[index];   
            } else {
                details[index].entity = results[index];
            }
        }
        
        detailsChanged();
        
        releaseFinished(Operation.Finished);
    }
    
    on__BatchError: {
        addError(__reply.errorString);
        releaseFinished(Operation.Error);
    }
}
