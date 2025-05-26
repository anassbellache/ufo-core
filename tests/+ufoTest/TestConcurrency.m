classdef TestConcurrency < matlab.unittest.TestCase
    %TESTCONCURRENCY Exercises multi-worker behaviour of bindings
    methods(Test)
        function twoPluginManagers(testCase)
            % CC-01 two MATLAB workers create PluginManager in parallel
            pool = gcp('nocreate');
            hasPCT = ~isempty(ver('parallel'));
            testCase.assumeTrue(hasPCT, 'Parallel Computing Toolbox required');

            if isempty(pool)
                pool = parpool(2);
                cleanup = onCleanup(@() delete(pool)); %#ok<NASGU>
            elseif pool.NumWorkers < 2
                delete(pool);
                pool = parpool(2);
                cleanup = onCleanup(@() delete(pool)); %#ok<NASGU>
            end

            f1 = parfeval(pool, @TestConcurrency.createPMOnWorker, 1);
            f2 = parfeval(pool, @TestConcurrency.createPMOnWorker, 1);

            ok1 = fetchOutputs(f1);
            ok2 = fetchOutputs(f2);

            testCase.verifyTrue(ok1 && ok2, ...
                'PluginManager could not be created concurrently');
        end
        function schedulerBusy(testCase)
            % CC-03 ensure Scheduler reports busy when run is called twice
            pm = ufo.PluginManager();
            tg = ufo.TaskGraph();
            a = pm.getTask('read');
            b = pm.getTask('write');
            idA = tg.addNode(a);
            idB = tg.addNode(b);
            tg.connect(idA, idB);
            sched = ufo.Scheduler();

            try
                sched.runAsync(tg);
            catch ME
                if strcmp(ME.identifier, 'ufo_mex:NotImplemented')
                    testCase.assumeFail('Scheduler_runAsync not implemented');
                else
                    rethrow(ME);
                end
            end

            testCase.verifyError(@() sched.run(tg), 'ufo:Scheduler:Busy');

            if ismethod(sched, 'stop')
                sched.stop();
            end
            delete(sched);
            delete(tg);
            delete(pm);
        end
    end

    methods(Static, Access = private)
        function ok = createPMOnWorker
            try
                pm = ufo.PluginManager();
                ok = isvalid(pm);
                delete(pm);
            catch
                ok = false;
            end
        end
    end

    methods(Static, Access = private)
        function ok = createPMOnWorker
            try
                pm = ufo.PluginManager();
                ok = isvalid(pm);
                delete(pm);
            catch
                ok = false;
            end
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
