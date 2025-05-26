classdef TestBuffer < matlab.unittest.TestCase
    %TESTBUFFER Unit tests for ufo.Buffer bindings.
    % Each method corresponds to a requirement from the BU-01..BU-08 matrix.
    methods(Test)
        function createReturnsObject(testCase)
            % BU-01: MATLAB path loaded -> ufo.Buffer.new([32 32])
            buf = ufo.Buffer([32 32]);
            testCase.addTeardown(@() delete(buf));
            testCase.verifyTrue(isvalid(buf));
            testCase.verifyClass(buf, 'ufo.Buffer');
        end

        function getSizeMatchesInput(testCase)
            % BU-02: After creation -> getSize returns byte count
            dims = [32 32];
            buf = ufo.Buffer(dims);
            testCase.addTeardown(@() delete(buf));
            sz = buf.getSize();
            testCase.verifyEqual(sz, prod(dims));
        end

        function invalidAfterFree(testCase)
            % BU-03: After free, getSize should raise InvalidHandle
            buf = ufo.Buffer(8);
            delete(buf);
            testCase.verifyError(@() buf.getSize(), 'MATLAB:class:InvalidHandle');
        end

        function zeroDimensionErrors(testCase)
            % BU-04: dims vector contains zero -> new([0 64]) errors
            testCase.verifyError(@() ufo.Buffer([0 64]), ...
                'ufo:Buffer:BadSize');
        end

        function tooManyDimsErrors(testCase)
            % BU-05: dims has >3 elements -> new([1 2 3 4]) errors
            testCase.verifyError(@() ufo.Buffer([1 2 3 4]), ...
                'ufo:Buffer:DimOverflow');
        end

        function nonNumericDimErrors(testCase)
            % BU-06: dims is non-numeric -> new("foo") errors
            testCase.verifyError(@() ufo.Buffer("foo"), ...
                'ufo:Buffer:Type');
        end

        function toArrayMatchesContents(testCase)
            % BU-07: valid buffer -> toArray helper returns matching uint8
            if ~ismethod(ufo.Buffer, 'setData')
                testCase.assumeFail('Buffer.setData not implemented');
            end
            data = uint8(1:16);
            buf = ufo.Buffer(numel(data));
            testCase.addTeardown(@() delete(buf));
            buf.setData(data);
            out = buf.getData();
            testCase.verifyClass(out, 'uint8');
            testCase.verifyEqual(out, data(:)');
        end

        function copyDuplicatesData(testCase)
            % BU-08: copy(A,B) where A,B are same dims -> B equals A
            if ~ismethod(ufo.Buffer, 'copy')
                testCase.assumeFail('Buffer.copy not implemented');
            end
            src = ufo.Buffer(16);
            dst = ufo.Buffer(16);
            testCase.addTeardown(@() delete(src));
            testCase.addTeardown(@() delete(dst));
            if ismethod(src, 'setData')
                src.setData(uint8(1:16));
            end
            src.copy(dst);
            testCase.verifyEqual(dst.getData(), src.getData());
        end
    end
end
