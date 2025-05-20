classdef TestBuffer < matlab.unittest.TestCase
    %TESTBUFFER Unit tests for ufo.Buffer basic behaviour

    methods(Test)
        function newGetSizeFree(testCase)
            % Create buffer and check size is numeric
            buf = ufo.Buffer();
            sz = buf.getSize();
            testCase.verifyTrue(isnumeric(sz) && isscalar(sz), ...
                'Size should be numeric scalar');

            % After deletion the handle should be invalid
            delete(buf);
            testCase.verifyError(@() buf.getSize(), 'ufo:Buffer:InvalidHandle');
        end
    end
end
