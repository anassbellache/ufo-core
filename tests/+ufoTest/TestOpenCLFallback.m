classdef TestOpenCLFallback < matlab.unittest.TestCase
    %TESTOPENCLFALLBACK Validate OpenCL CPU fallback logic
    methods(Test)
        function envCpuFallback(testCase)
            % OC-01: forcing CPU mode via environment should still allow
            % execution of OpenCL tasks

            oldType = getenv('UFO_DEVICE_TYPE');
            cleanup = onCleanup(@() setenv('UFO_DEVICE_TYPE', oldType)); %#ok<NASGU>
            setenv('UFO_DEVICE_TYPE', 'CPU');

            pm = ufo.PluginManager();
            names = pm.listPlugins();

            % assume an FFT plugin is available, otherwise this test cannot
            % verify the fallback behaviour
            if ~ismember('fft', names)
                testCase.assumeFail('Required ''fft'' plugin not available');
            end

            reader = pm.getTask('read');
            fft = pm.getTask('fft');
            writer = pm.getTask('write');

            tg = ufo.TaskGraph();
            idR = tg.addNode(reader);
            idF = tg.addNode(fft);
            idW = tg.addNode(writer);
            tg.connect(idR, idF);
            tg.connect(idF, idW);

            sched = ufo.Scheduler();
            res = ufo.Resources();
            sched.setResources(res);

            % simply running without error indicates that the OpenCL kernel
            % was executed on the CPU backend
            testCase.verifyWarningFree(@() sched.run(tg));
        end
        function malformedKernel(testCase)
            % OC-02: passing malformed OpenCL source must raise an error
            res = ufo.Resources();

            if ~ismethod(res, 'getKernelFromSource')
                testCase.assumeFail('Resources.getKernelFromSource not implemented');
            end

            badSrc = '__kernel void bad() { syntax error }';
            testCase.verifyError(@() res.getKernelFromSource(badSrc, '', ''), ...
                'ufo:Resources:LoadProgram');
        end
    end
end

