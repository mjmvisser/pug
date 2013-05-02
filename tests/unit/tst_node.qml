import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "TestNode"
    
    Root {
        id: root
        fields: [
            Field { name: "FOO"; values: "foo"; defaultValue: "foo" },
            Field { name: "BAR" }
        ]

        Node {
            id: node
            fields: [
                Field { name: "BAZ"; defaultValue: "baz" },
                Field { name: "QUX" }
            ]
        }
    }
    
    function test_fields() {
        var context = {"BAR": "bar", "QUX": "qux"};
        compare(root.format("{FOO}_{BAR}_{BAZ}_{QUX}", context), ""); // NOTE: QString() converts to empty string, not null
        compare(root.format("{FOO}_{BAR}", context), "foo_bar");
        compare(node.format("{FOO}_{BAR}_{BAZ}_{QUX}", context), "foo_bar_baz_qux");
        compare(node.format("{FOO}_{BAR}", context), "foo_bar");
    }
}