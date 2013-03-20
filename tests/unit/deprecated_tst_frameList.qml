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
        name: "consecutiveIncreasing"
        frames: [1,2,3,4,5,6,7]
    }
    
    function test_consecutiveIncreasing() {
        compare(consecutiveIncreasing.frames, [1,2,3,4,5,6,7]);
        compare(String(consecutiveIncreasing.pattern), "1-7");
    }
    
    FrameList {
        id: consecutiveIncreasingWithNegative
        name: "consecutiveIncreasingWithNegative"
        frames: [-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7]
    }
    
    function test_consecutiveIncreasingWithNegative() {
        compare(consecutiveIncreasingWithNegative.frames, [-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7]);
        compare(String(consecutiveIncreasingWithNegative.pattern), "-5-7");
    }
    
    FrameList {
        id: consecutiveDecreasing
        name: "consecutiveDecreasing"
        frames: [8, 7, 6, 5, 4, 3, 2]
    }
    
    function test_consecutiveDecreasing()
    {
        compare(consecutiveDecreasing.frames, [2,3,4,5,6,7,8]);
        compare(String(consecutiveDecreasing.pattern), "2-8");
    }

    FrameList {
        id: consecutiveDecreasingWithNegative
        name: "consecutiveDecreasingWithNegative"
        frames: [5, 4, 3, 2, 1, 0, -1, -2, -3, -4]
    }
    
    function test_consecutiveDecreasingWithNegative() {
        compare(consecutiveDecreasingWithNegative.frames, [-4,-3,-2,-1,0,1,2,3,4,5]);
        compare(String(consecutiveDecreasingWithNegative.pattern), "-4-5");
    }
    
    FrameList {
        id: mixedFrames
        name: "mixedFrames"
        frames: [5, 4, 3, 10, 11, 12, 25, 33, 17, 19, 21]
    }
    
    function test_mixedFrames() {
        compare(mixedFrames.frames, [3,4,5,10,11,12,17,19,21,25,33]);
        compare(String(mixedFrames.pattern), "3-5,10-12,17-21x2,25,33");
    }
    
    FrameList {
        id: singleFrame
        name: "singleFrame"
        frames: [23]
    }
    
    function test_singleFrame() {
        compare(singleFrame.pattern, "23");
        compare(singleFrame.frames, [23]);
    }
    
    FrameList {
        id: mixedPattern
        logLevel: Log.Debug
        name: "mixedPattern"
        pattern: "-22--16x3,3-5,10-12,17-21x2,25,33"
    }
    
    function test_mixedPattern() {
        compare(mixedPattern.frames, [-22,-19,-16,3,4,5,10,11,12,17,19,21,25,33]);
        compare(String(mixedPattern.pattern), "-22--16x3,3-5,10-12,17-21x2,25,33");
    }
    
    FrameList {
        id: duplicateFrames
        name: "duplicateFrames"
        frames: [1,1,1,1,2,3,4,4,4,4]
    }
    
    function test_duplicateFrames() {
        compare(duplicateFrames.frames, [1,2,3,4]);
        compare(String(duplicateFrames.pattern), "1-4");
    }
    
    FrameList {
        id: duplicatePattern
        name: "duplicatePattern"
        pattern: "1,1-4"
    }
    
    function test_duplicatePattern() {
        compare(duplicatePattern.frames, [1,2,3,4]);
        compare(String(duplicatePattern.pattern), "1-4");
    }
}