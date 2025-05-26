classdef TestClassBehavior < matlab.unittest.TestCase
    %TESTCLASSBEHAVIOR Verify basic MATLAB object behavior for ufo wrappers
    methods(Test)
        function copySharesHandle(testCase)
            % OO-01: Buffer object copied via assignment
            bufA = ufo.Buffer(uint64(64));
            bufB = bufA;        % handle copy should reference same object

            testCase.verifyTrue(bufA == bufB, ...
                'Handle copy should produce the same underlying object');

            % Deleting one reference invalidates the other
            delete(bufA);
            testCase.verifyFalse(isvalid(bufB));
        end
        function deleteMarksInvalid(testCase)
            % OO-02: explicit delete sets isvalid to false
            pm = ufo.PluginManager();
            delete(pm);
            testCase.verifyFalse(isvalid(pm));
        end
        function dispShowsNodeCount(testCase)
            % OO-03: disp on TaskGraph prints node count
            tg = ufo.TaskGraph();
            pm = ufo.PluginManager();
            tg.addNode(pm.getTask("read"));
            tg.addNode(pm.getTask("write"));

            out = evalc('disp(tg)');
            testCase.verifyNotEmpty(strfind(out, '2'), ...
                'Expected disp output to mention two nodes');
        end
        function saveLoadRestoresHandle(testCase)
            % OO-04: object save/load via MAT-file should restore handle
            buf = ufo.Buffer(uint64(32));
            tmp = [tempname '.mat'];
            cleanup = onCleanup(@() delete(tmp));

            save(tmp, 'buf');
            clear buf
            S = load(tmp, 'buf');
            buf2 = S.buf;

            testCase.verifyEqual(buf2.getSize(), uint64(32));
            delete(cleanup);
        end
    end
end
