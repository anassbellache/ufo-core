classdef TestBuffer < matlab.unittest.TestCase
    %TESTBUFFER Unit tests exercising the ufo.Buffer MATLAB wrapper.
    % These tests correspond to BU-01 .. BU-08 in the specification.

    methods (Test)
        function BU01_new_returns_buffer(testCase)
            buf = ufo.Buffer.new([32 32]);
            testCase.assertClass(buf, 'ufo.Buffer');
        end

        function BU02_getSize_returns_dims(testCase)
            buf = ufo.Buffer.new([32 32]);
            sz = buf.getSize();
            testCase.verifyEqual(sz, int32([32 32]));
        end

        function BU03_handle_invalid_after_free(testCase)
            buf = ufo.Buffer.new([32 32]);
            buf.free();
            testCase.verifyError(@() buf.getSize(), 'ufo:Buffer:InvalidHandle');
        end

        function BU04_zero_dimension_raises(testCase)
            testCase.verifyError(@() ufo.Buffer.new([0 64]), 'ufo:Buffer:BadSize');
        end

        function BU05_too_many_dims_raises(testCase)
            testCase.verifyError(@() ufo.Buffer.new([1 2 3 4]), ...
                'ufo:Buffer:DimOverflow');
        end

        function BU06_non_numeric_dims_raises(testCase)
            testCase.verifyError(@() ufo.Buffer.new("foo"), 'ufo:Buffer:Type');
        end

        function BU07_toArray_matches_contents(testCase)
            buf = ufo.Buffer.new([8 8]);
            arr = buf.toArray();
            testCase.verifyClass(arr, 'uint8');
            testCase.verifySize(arr, [8 8]);
        end

        function BU08_copy_buffer_contents(testCase)
            A = ufo.Buffer.new([8 8]);
            B = ufo.Buffer.new([8 8]);
            ufo.Buffer.copy(A, B);
            testCase.verifyEqual(B.toArray(), A.toArray());
        end
    end
end
