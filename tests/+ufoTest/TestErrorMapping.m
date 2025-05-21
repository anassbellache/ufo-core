classdef TestErrorMapping < matlab.unittest.TestCase
    %TESTERRORMAPPING Verify error identifiers emitted by ufo\_mex
    methods(Test)
        function unknownCommand(testCase)
            % ER-01: call ufo\_mex with an unknown command
            % TODO: verify that the thrown MException has identifier
            % 'ufo_mex:UnknownCommand'
        end

        function tooManyOutputs(testCase)
            % ER-02: request multiple outputs from Buffer\_new
            % TODO: verify that MATLAB raises 'MATLAB:maxlhs'
        end

        function missingArgument(testCase)
            % ER-03: call ufo\_mex with no arguments
            % TODO: verify that MATLAB complains with identifier
            % 'MATLAB:narginchk:notEnoughInputs'
        end
    end
end
