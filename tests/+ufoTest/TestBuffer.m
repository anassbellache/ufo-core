classdef TestBuffer < matlab.unittest.TestCase
    %TESTBUFFER Unit tests for ufo.Buffer basic behaviour
    methods(Test)
        function newGetSizeFree(testCase)
            % Create a fresh buffer and ensure it reports a numeric size.
            buf = ufo.Buffer();
            c = onCleanup(@()delete(buf)); %#ok<NASGU>

            sz = buf.getSize();
            testCase.verifyTrue(isnumeric(sz) && isscalar(sz), ...
                'Size should be a numeric scalar');
            testCase.verifyGreaterThanOrEqual(sz, 0, ...
                'Size must be non-negative');

            % After explicit deletion the handle becomes invalid and any
            % method must raise the InvalidHandle error.
            delete(buf);
            testCase.verifyError(@() buf.getSize(), ...
                'ufo:Buffer:InvalidHandle');
        end
    end
end
