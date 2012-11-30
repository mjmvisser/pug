import QtQuick 2.0
import QtTest 1.0
import Pug 1.0

PugTestCase {
    id: self
    name: "FileOpQueueTests"

    function init() {
        Util.mkpath(tmpDir + "fileop");
        Util.touch(tmpDir + "fileop/foo.txt");
    }
    
    function cleanup() {
        Util.remove(tmpDir + "fileop/foo.txt");
        Util.remove(tmpDir + "fileop/bar.txt");
        Util.remove(tmpDir + "fileop/foobar.txt");
        Util.remove(tmpDir + "fileop/bar/foobar.txt");
        Util.remove(tmpDir + "fileop/bar/bar.txt");
        Util.remove(tmpDir + "fileop/bar/foo.txt");
        Util.rmdir(tmpDir + "fileop/bar");
        Util.rmdir(tmpDir + "fileop");
    }

    FileOpQueue {
        id: fileOpQueue
    }    
    
    SignalSpy {
        id: spy
        target: fileOpQueue
        signalName: "finished"
    }

    function test_fileops() {
        fileOpQueue.mkdir(tmpDir + "fileop/bar");
        fileOpQueue.copy(tmpDir + "fileop/foo.txt", tmpDir + "fileop/bar.txt");
        fileOpQueue.copy(tmpDir + "fileop/bar.txt", tmpDir + "fileop/bar/barfoo.txt");
        fileOpQueue.move(tmpDir + "fileop/bar/barfoo.txt", tmpDir + "fileop/bar/foobar.txt");
        fileOpQueue.symlink(tmpDir + "fileop/foo.txt", tmpDir + "fileop/bar/foo.txt");
        fileOpQueue.hardlink(tmpDir + "fileop/bar/foo.txt", tmpDir + "fileop/foobar.txt");
        fileOpQueue.run({});
        spy.wait(1000);
        verify(Util.exists(tmpDir + "fileop/bar"));
        verify(Util.exists(tmpDir + "fileop/foo.txt"));
        verify(Util.exists(tmpDir + "fileop/bar.txt"));
        verify(Util.exists(tmpDir + "fileop/bar/foobar.txt"));
        verify(Util.exists(tmpDir + "fileop/bar/foo.txt"));
        verify(Util.exists(tmpDir + "fileop/foobar.txt"));
    }

}