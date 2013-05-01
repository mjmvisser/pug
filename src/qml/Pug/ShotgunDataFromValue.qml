import Pug 1.0

Node {
    id: self
    property string shotgunField
    property var value
    
    params: [
        Param { name: "shotgunField" },
        Param { name: "value" }
    ]
    
    CookOperation.onCook: {
        
        var newValue = value;
        if (toString.call(value) === "[object String]") {
            // find first branch parent
            var branch = self;
            while (branch && typeof branch.formatFields === 'undefined') {
                branch = branch.parent;
            }
            if (branch) {
                newValue = branch.formatFields(value, context);
            }
        }
        
        count = 1;
        details[0].data = {};
        details[0].data[shotgunField] = newValue;
        detailsChanged();
        CookOperation.cookFinished();
    }
}
