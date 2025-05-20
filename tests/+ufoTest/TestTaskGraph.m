classdef TestTaskGraph < matlab.unittest.TestCase
    %TESTTASKGRAPH Basic validation for ufo.TaskGraph
    methods(Test)
        function addNodeInvalid(testCase)
            tg = ufo.TaskGraph(); %#ok<NASGU>
            %TODO: calling addNode with wrong type should raise an error
        end
        function connectBadInput(testCase)
            tg = ufo.TaskGraph();
            %TODO: verify connect complains on invalid endpoints
        end
        function saveLoadRoundTrip(testCase)
            %MF-01: graph saved to MAT-file and loaded retains connections
            pm = ufo.PluginManager();
            tg = ufo.TaskGraph();
            reader = pm.getTask('read');
            writer = pm.getTask('write');
            idA = tg.addNode(reader);
            idB = tg.addNode(writer);
            tg.connect(idA, idB);
            tmpMat = [tempname '.mat'];
            save(tmpMat, 'tg');
            clear tg reader writer pm idA idB
            S = load(tmpMat, 'tg');
            tg2 = S.tg;
            validateResult = tg2.validate(); %#ok<NASGU>
            %TODO: verify validateResult is true and connections intact
            delete(tmpMat);
        end
    end
end
