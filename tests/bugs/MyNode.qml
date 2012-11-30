import QtQuick 2.0
import Pug 1.0

Process {
    property string foo
    
    onCook: {
        foo = env.FOO;
        cooked(Node.Cooked);
    }
}
