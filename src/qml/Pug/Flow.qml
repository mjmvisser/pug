import QtQuick 2.0
import Pug 1.0

import "js/flow.js" as Flow

Node {
    function mkpath(env) {
        return Flow.mkpath.call(self, env);
    }

    function init() {
        // nothing to do
    }
}
