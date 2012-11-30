import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "TestBranch"
 
    Branch {
        id: branch
        name: "testBranch"
        pattern: "/abc/{TEST}/def"
        Field {
            name: "TEST"
        }
    }
    
    function test_branch() {
        verify(branch.inherits("Branch"));
        compare(branch.className.indexOf("Branch"), 0);
        verify(branch.node);
        compare(branch.pattern, "/abc/{TEST}/def");
        compare(JSON.stringify(branch.parse("/abc/foo/def")), JSON.stringify({TEST: "foo"}));
        compare(branch.map({TEST: "foo"}), "/abc/foo/def");
        compare(branch.map({}), "");
    }
    
    Root {
        id: root
        name: "testConfig"
        Field {
            name: "FOO"
        }
        
        Field {
            name: "BAZ"
            type: Field.Integer
        }
        
        Branch {
            id: branch1
            name: "branch1"
            pattern: "/abc/{FOO}/def/{BAR}/"
            
            Field {
                name: "BAR"
            }
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