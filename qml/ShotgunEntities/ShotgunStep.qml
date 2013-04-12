import Pug 1.0

ShotgunEntity {
    id: stepEntity
    name: "stepEntity"
    shotgunEntity: "Step"
    TractorOperation.flatten: true
    shotgunFields: [
        codeField
    ]
    
    count: parent.count

    property string step

    params: [
        Param { name: "step" }
    ]
   
    ShotgunField {
        id: codeField
        name: "code"
        shotgunField: "short_name"
        required: true
        value: step
    }
}
