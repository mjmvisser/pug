import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    name: "FrameListTests"
    
    function init() {
            
    }
    
    function cleanUp() {
        
    }
    
    FrameList {
        id: consecutiveIncreasing
        frames: [1,2,3,4,5,6,7]
    }
    
    function test_consecutiveIncreasing() {
        compare(consecutiveIncreasing.pattern, "1-7");
    }
    
    FrameList {
        id: consecutiveIncreasingWithNegative
        frames: [-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7]
    }
    
    function test_consecutiveIncreasingWithNegative() {
        compare(consecutiveIncreasingWithNegative.pattern, "-5-7");
    }
    
    FrameList {
        id: consecutiveDecreasing
        frames: [8, 7, 6, 5, 4, 3, 2]
    }
    
    function test_consecutiveDecreasing()
    {
        compare(consecutiveDecreasing.pattern, "2-8");
    }

    FrameList {
        id: consecutiveDecreasingWithNegative
        frames: [5, 4, 3, 2, 1, 0, -1, -2, -3, -4]
    }
    
    function test_consecutiveDecreasingWithNegative() {
        compare(consecutiveDecreasingWithNegative.pattern, "-4-5");
    }
    
    FrameList {
        id: mixedFrames
        frames: [5, 4, 3, 10, 11, 12, 25, 33, 17, 19, 21]
    }
    
    function test_mixedFrames() {
        compare(String(mixedFrames.pattern), "3-5,10-12,17-21x2,25,33");
    }
    
    FrameList {
        id: singleFrame
        frames: [23]
    }
    
    function test_singleFrame() {
        compare(singleFrame.pattern, "23");
    }
    
    FrameList {
        id: mixedPattern
        pattern: "-22--16x3,3-5,10-12,17-21x2,25,33"
    }
    
    function test_mixedPattern() {
        compare(mixedPattern.frames, [-22,-19,-16,3,4,5,10,11,12,17,19,21,25,33]);
    }
}