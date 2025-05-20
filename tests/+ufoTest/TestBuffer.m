classdef TestBuffer < matlab.unittest.TestCase
    %TESTBUFFER Unit tests for ufo.Buffer bindings.
    % Each method corresponds to a requirement from the BU-01..BU-08 matrix.
    methods(Test)
        function createReturnsObject(testCase)
            % BU-01: MATLAB path loaded -> ufo.Buffer.new([32 32])
            % TODO: allocate buffer of size [32 32] and verify it returns a
            % valid ufo.Buffer instance.
        end

        function getSizeMatchesInput(testCase)
            % BU-02: After creation -> getSize returns [32 32] (int32)
            % TODO: call getSize on buffer created in BU-01 and verify the
            % dimensions are returned as int32 vector [32 32].
        end

        function invalidAfterFree(testCase)
            % BU-03: After free, getSize should raise InvalidHandle
            % TODO: free the buffer then verify getSize errors with the
            % identifier 'ufo:Buffer:InvalidHandle'.
        end

        function zeroDimensionErrors(testCase)
            % BU-04: dims vector contains zero -> new([0 64]) errors
            % TODO: verify Buffer.new([0 64]) throws 'ufo:Buffer:BadSize'.
        end

        function tooManyDimsErrors(testCase)
            % BU-05: dims has >3 elements -> new([1 2 3 4]) errors
            % TODO: verify Buffer.new with four elements raises
            % 'ufo:Buffer:DimOverflow'.
        end

        function nonNumericDimErrors(testCase)
            % BU-06: dims is non-numeric -> new("foo") errors
            % TODO: verify Buffer.new('foo') raises 'ufo:Buffer:Type'.
        end

        function toArrayMatchesContents(testCase)
            % BU-07: valid buffer -> toArray helper returns matching uint8
            % TODO: fill buffer with known pattern and verify toArray
            % produces a uint8 array of the same size and contents.
        end

        function copyDuplicatesData(testCase)
            % BU-08: copy(A,B) where A,B are same dims -> B equals A
            % TODO: allocate two buffers and verify that after invoking
            % copy, buffer B contains the same data as buffer A.
        end
    end
end
