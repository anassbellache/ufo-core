classdef TestErrorMapping < matlab.unittest.TestCase
    %TESTERRORMAPPING Verify error identifiers emitted by ufo\_mex
    methods(Test)
        function unknownCommand(testCase)
            % ER-01: call ufo_mex with an unknown command
            f = @() ufo_mex('definitely_not_a_command');
            testCase.verifyError(f, 'ufo_mex:UnknownCommand');
        end

        function tooManyOutputs(testCase)
            % ER-02: request multiple outputs from Buffer_new
            f = @() evalc('[a,b] = ufo_mex(''Buffer_new'', [4 4]);');
            testCase.verifyError(f, 'MATLAB:maxlhs');
        end

        function missingArgument(testCase)
            % ER-03: call ufo_mex with no arguments
            testCase.verifyError(@() ufo_mex(), 'MATLAB:narginchk:notEnoughInputs');
        end
    end
end
