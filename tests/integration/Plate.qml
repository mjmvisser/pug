import QtQuick 2.0
import Pug 1.0

Branch {
    constraints: fields.DEPARTMENT === "test" && fields.VARIATION == "pl" && fields.RES == "2k" && fields.FORMAT == "dpx" && fields.EXT == "dpx"
    
    contents: Network {
        File {
            id: workPlate
            objectName: "workPlate"
            pattern: "work/{USER}/{SHOT}_{DEPARTMENT}_{RES}_{FORMAT}.{FRAMESPEC}.{EXT}"
        }

        Release {
            objectName: "releasePlate"        
            pattern: "release/{VARIATION}/plate/{RES}/{FORMAT}/{SHOT}_{DEPARTMENT}_{RES}_{FORMAT}_v{VERSION}.{FRAMESPEC}.{EXT}"
            input: workPlate
        }
    }
}
