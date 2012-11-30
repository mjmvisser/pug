import QtQuick 2.0
import Pug 1.0

MyNode {
    signal release(var env)
    signal released()
    
    Component.onCompleted: {
        release.connect(cook);
        cooked.connect(released);
    }
}
