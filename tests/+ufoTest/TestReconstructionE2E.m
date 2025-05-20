classdef TestReconstructionE2E < matlab.unittest.TestCase
    %TESTRECONSTRUCTIONE2E End-to-end regression tests for full pipeline
    properties(Constant)
        DataDir = fullfile(fileparts(mfilename('fullpath')), 'data');
    end
    methods(Test)
        function parallelBeam(testCase)
            %TODO: run reconstruction on parallel-beam sample and compare
        end
        function fanBeam(testCase)
            %TODO: run reconstruction on fan-beam sample and compare
        end
    end
end
