classdef TestClassBehavior < matlab.unittest.TestCase
    %TESTCLASSBEHAVIOR Verify basic MATLAB object behavior for ufo wrappers
    methods(Test)
        function copySharesHandle(testCase)
            % OO-01: Buffer object copied via assignment
            % TODO: create buffer and copy it, verify both variables refer to
            % the same native handle.
        end
        function deleteMarksInvalid(testCase)
            % OO-02: explicit delete sets isvalid to false
            % TODO: create object, delete it, and check isvalid returns false.
        end
        function dispShowsNodeCount(testCase)
            % OO-03: disp on TaskGraph prints node count
            % TODO: create graph with some nodes and ensure disp includes the
            % node count in the printed output.
        end
        function saveLoadRestoresHandle(testCase)
            % OO-04: object save/load via MAT-file should restore handle
            % TODO: save Buffer to file and reload, then call a method to
            % ensure the handle is active.
        end
    end
end
