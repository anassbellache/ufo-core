classdef TestConcurrency < matlab.unittest.TestCase
    %TESTCONCURRENCY Exercises multi-worker behaviour of bindings
    methods(Test)
        function twoPluginManagers(testCase)
            % CC-01 two MATLAB workers create PluginManager in parallel
            if isempty(ver('parallel'))
                testCase.assumeFail('Parallel Computing Toolbox required');
            end

            pool = gcp('nocreate');
            needsClose = false;
            if isempty(pool) || pool.NumWorkers < 2
                pool = parpool(2); %#ok<NASGU>
                needsClose = true;
            end
            cleanup = onCleanup(@() conditionalDelete(pool, needsClose)); %#ok<NASGU>

            f = parallel.FevalFuture.empty(2,0);
            for idx = 1:2
                f(idx) = parfeval(@createPM, 1);
            end
            results = fetchOutputs(f);
            testCase.verifyTrue(all([results{:}]));
        end
        function schedulerBusy(testCase)
            % CC-03 ensure Scheduler reports busy when run is called twice
            pm = ufo.PluginManager();
            tg = ufo.TaskGraph();
            tg.addNode(pm.getTask("read"));
            sched = ufo.Scheduler();

            if ~ismethod(sched, 'runAsync')
                testCase.assumeFail('Scheduler.runAsync not available');
            end

            try
                fut = sched.runAsync(tg); %#ok<NASGU>
            catch ME
                if strcmp(ME.identifier, 'ufo_mex:NotImplemented')
                    testCase.assumeFail('runAsync MEX command not implemented');
                else
                    rethrow(ME);
                end
            end

            testCase.verifyError(@() sched.runAsync(tg), 'ufo:Scheduler:Busy');
        end
    end
end

function tf = createPM
pm = ufo.PluginManager();
tf = isa(pm, 'ufo.PluginManager');
delete(pm);
end

function conditionalDelete(poolObj, doDelete)
if doDelete
    delete(poolObj);
end
end
