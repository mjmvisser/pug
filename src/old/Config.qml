import QtQuick 2.0
import Pug 1.0

import "js/config.js" as Config

Node {
    id: self
    property var __fields
    
    function parse(node, path) {
        return Config.parse.call(self, node, path);
    }
    
    function map(node, env) {
        return Config.map.call(self, node, env);
    }
    
    function init() {
        return Config.init.call(self);
    }
    
    function getField(fieldName) {
        return Config.getField.call(self, fieldName);
    }
    
    function getNode(nodeName) {
        return Config.getNode.call(self, nodeName);
    }
    
    function mkpath(node, env) {
        return Config.mkpath.call(self, node, env);
    }
}