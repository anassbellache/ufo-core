classdef TestOpenCLFallback < matlab.unittest.TestCase
    %TESTOPENCLFALLBACK Validate OpenCL CPU fallback logic
    methods(Test)
        function envCpuFallback(testCase)
            % OC-01: Setting UFO_DEVICE_TYPE=CPU should allow running GPU tasks
            %        in CPU fallback mode without error.

            oldType = getenv('UFO_DEVICE_TYPE');
            cleanup = onCleanup(@() setenv('UFO_DEVICE_TYPE', oldType));
            setenv('UFO_DEVICE_TYPE', 'CPU');

            pm  = ufo.PluginManager();
            tg  = ufo.TaskGraph();

            % Many installations provide an FFT plugin which normally
            % executes on the GPU.  Skip the test if unavailable.
            plugins = pm.listPlugins();
            testCase.assumeTrue(ismember('fft', plugins), ...
                'FFT plugin not found; skipping CPU fallback test');

            reader = pm.getTask('read');
            fft    = pm.getTask('fft');
            writer = pm.getTask('write');

            tg.addNode(reader);
            idFft   = tg.addNode(fft);
            idWrite = tg.addNode(writer);
            tg.connect(0, idFft);
            tg.connect(idFft, idWrite);

            sched = ufo.Scheduler();
            testCase.verifyWarningFree(@() sched.run(tg));
        end

        function malformedKernel(testCase)
            % OC-02: Loading malformed OpenCL source should raise
            %        ufo:Resources:LoadProgram.

            res = ufo.Resources();
            if ~ismethod(res, 'getKernelFromSource')
                testCase.assumeFail('getKernelFromSource not implemented');
            end

            badSrc = 'kernel void bad(__global float* f) { this is invalid }';

            testCase.verifyError(@() res.getKernelFromSource(badSrc), ...
                'ufo:Resources:LoadProgram');
        end
    end
end

