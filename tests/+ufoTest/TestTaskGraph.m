classdef TestTaskGraph < matlab.unittest.TestCase
    %TESTTASKGRAPH Basic validation for ufo.TaskGraph

    methods(Test)
        function addNodeInvalid(testCase)
            tg = ufo.TaskGraph();
            % wrong type should error
            testCase.verifyError(@() tg.addNode(42), 'MATLAB:validators:mustBeA');
        end

        function connectBadInput(testCase)
            tg = ufo.TaskGraph();
            % connect with invalid endpoints should raise custom error
            testCase.verifyError(@() tg.connect(42, 99), 'ufo:TaskGraph:BadEndpoint');
        end
    end
end
