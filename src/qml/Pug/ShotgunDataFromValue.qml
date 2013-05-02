import Pug 1.0

Node {
    id: self
    property string shotgunField
    property var value
    
    params: [
        Param { name: "shotgunField" },
        Param { name: "value" }
    ]f
    
    CookOperation.onCook: {
        count = 1;
        details[0].data = {};
        details[0].data[shotgunField] = value;
        detailsChanged();
        CookOperation.cookFinished();
    }
}
