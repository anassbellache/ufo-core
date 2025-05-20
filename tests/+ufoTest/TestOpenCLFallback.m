classdef TestOpenCLFallback < matlab.unittest.TestCase
    %TESTOPENCLFALLBACK Validate OpenCL CPU fallback logic
    methods(Test)
        function envCpuFallback(testCase)
            % TODO: setenv('UFO_DEVICE_TYPE','CPU');
            % Build GPU filter graph and run via ufo.Scheduler
            % Verify execution succeeds on CPU
        end
        function malformedKernel(testCase)
            % TODO: load a malformed CL kernel via ufo.Resources
            % Expect error id 'ufo:Resources:LoadProgram'
        end
    end
end

