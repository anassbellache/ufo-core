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
    end
end
