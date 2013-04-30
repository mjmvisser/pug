import Pug 1.0

ShotgunDataFromValue {
    id: self
    property Node element

    property var __elementView: Util.elementsView(element)    
    
    value: __elementView.element[index].pattern()
}
