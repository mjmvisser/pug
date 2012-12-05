import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "TestBranch"
 
    Branch {
        id: branch
        name: "testBranch"
        pattern: "{ROOT}/abc/{TEST}/def/{FOO}/"
        fields: [
            Field { name: "ROOT"; values: "/root"; defaultValue: "/root" },
            Field { name: "TEST" },
            Field { name: "FOO"; defaultValue: "foo" }
        ]
    }
    
    function test_branch() {
        verify(branch.inherits("Branch"));
        compare(branch.className.indexOf("Branch"), 0);
        verify(branch.node);
        compare(branch.pattern, "{ROOT}/abc/{TEST}/def/{FOO}/");
        compare(branch.parse("/root/abc/foo/def/foo/"), {ROOT: "/root", TEST: "foo", FOO: "foo"});
        compare(branch.map({TEST: "foo"}), "/root/abc/foo/def/foo/");
        compare(branch.map({}), "");
    }
    
    Root {
        id: root
        name: "testConfig"
        
        fields: [
            Field { name: "FOO" },
            Field { name: "BAZ"; type: Field.Integer }
        ]
        
        Branch {
            id: branch1
            name: "branch1"
            pattern: "/abc/{FOO}/def/{BAR}/"

            fields: Field { name: "BAR" }
        }
        
        Branch {
            id: branch2
            name: "branch2"
            pattern: "ghi/{BAZ}"
            root: branch1
        }
    }
    
    function test_node() {
        compare(branch2.node("..").name, "testConfig");
        compare(branch2.node("../branch1").name, "branch1");
        //compare(branch2.node("../.."), null);
        //compare(branch2.node("/").name, "testConfig");
        //compare(branch2.node("/branch1").name, "branch1");
        //compare(branch2.node("/branch2").name, "branch2");
        //compare(branch1.node("../branch2"), "branch2");
    }
    
    function test_root() {
        compare(root.name, "testConfig");
        compare(root.parse("/branch2", "/abc/foo/def/bar/ghi/23"), 
                {FOO: "foo", BAR: "bar", BAZ: 23});
        compare(root.map("/branch2", {FOO: "foo", BAR: "bar", BAZ: 18}), "/abc/foo/def/bar/ghi/18");
        compare(root.map("/branch1", {FOO: "foo", BAR: "bar"}), "/abc/foo/def/bar/");
    }
}