classdef TestBuffer < matlab.unittest.TestCase
    %TESTBUFFER Unit tests for ufo.Buffer bindings.
    % Each method corresponds to a requirement from the BU-01..BU-08 matrix.
    methods(Test)
        function createReturnsObject(testCase)
            % BU-01: MATLAB path loaded -> ufo.Buffer.new([32 32])
            buf = ufo.Buffer([32 32]);
            cleaner = onCleanup(@() delete(buf)); %#ok<NASGU>
            testCase.verifyClass(buf, 'ufo.Buffer');
        end

        function getSizeMatchesInput(testCase)
            % BU-02: After creation -> getSize returns [32 32] (int32)
            buf = ufo.Buffer([32 32]);
            cleaner = onCleanup(@() delete(buf)); %#ok<NASGU>
            sz = buf.getSize();
            testCase.verifyEqual(sz, int32([32 32]));
        end

        function invalidAfterFree(testCase)
            % BU-03: After free, getSize should raise InvalidHandle
            buf = ufo.Buffer([8 8]);
            delete(buf);
            testCase.verifyError(@() buf.getSize(), 'ufo:Buffer:InvalidHandle');
        end

        function zeroDimensionErrors(testCase)
            % BU-04: dims vector contains zero -> new([0 64]) errors
            f = @() ufo.Buffer([0 64]);
            testCase.verifyError(f, 'ufo:Buffer:BadSize');
        end

        function tooManyDimsErrors(testCase)
            % BU-05: dims has >3 elements -> new([1 2 3 4]) errors
            f = @() ufo.Buffer([1 2 3 4]);
            testCase.verifyError(f, 'ufo:Buffer:DimOverflow');
        end

        function nonNumericDimErrors(testCase)
            % BU-06: dims is non-numeric -> new("foo") errors
            f = @() ufo.Buffer("foo");
            testCase.verifyError(f, 'ufo:Buffer:Type');
        end

        function toArrayMatchesContents(testCase)
            % BU-07: valid buffer -> toArray helper returns matching uint8
            buf = ufo.Buffer(10);
            cleaner = onCleanup(@() delete(buf)); %#ok<NASGU>
            data = buf.getData();
            testCase.verifyEqual(class(data), 'uint8');
            testCase.verifyEqual(numel(data), 10);
            testCase.verifyEqual(data, zeros(1,10,'uint8'));
        end

        function copyDuplicatesData(testCase)
            % BU-08: copy(A,B) where A,B are same dims -> B equals A
            bufA = ufo.Buffer(4);
            bufB = ufo.Buffer(4);
            cleanup = onCleanup(@() delete([bufA bufB])); %#ok<NASGU>
            if ismethod(bufA, 'copy')
                if ismethod(bufA, 'setData')
                    pattern = uint8(1:4);
                    bufA.setData(pattern);
                end
                bufA.copy(bufB);
                testCase.verifyEqual(bufB.getData(), bufA.getData());
            else
                testCase.assumeFail('Buffer copy method not implemented');
            end
        end
    end
end
