import Pug 1.0

ShotgunDataFromValue {
    id: self
    property Node element

    value: element.File.element[index].pattern()
}
