import QtQuick 2.0
import Pug 1.0

import "js/field.js" as Field 

Property {
    id: self
    property string regexp: "\\w+" 
    property string format: "%s"
    property string type: "string" 
    property var values
    
    function convert(str) {
        return Field.convert.call(self, str);
    }
}
