import Pug 1.0

ShotgunEntity {
    id: userEntity
    name: "userEntity"
    shotgunEntity: "HumanUser"
    shotgunFields: [
        loginField
    ]

    output: (action === ShotgunEntity.Create)

    count: parent.count

    ShotgunField {
        id: loginField
        name: "loginField"
        shotgunField: "login"
        required: true
        field: "USER"
        source: userEntity.parent
    }
}