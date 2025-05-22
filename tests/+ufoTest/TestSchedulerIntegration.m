classdef TestSchedulerIntegration < matlab.unittest.TestCase
    %TESTSCHEDULERINTEGRATION Integration tests for ufo.Scheduler

    methods(Test)
        function readWritePipeline(testCase)
            % SC-01: read->write graph should produce a file with content
            pm = ufo.PluginManager();
            reader = pm.getTask("read");
            writer = pm.getTask("write");

            % property setter may not exist in minimal build
            if ~ismethod(reader, 'setProperties') || ~ismethod(writer, 'setProperties')
                testCase.assumeFail('Task property setters not implemented');
            end

            tmpIn = which('README.md');
            tmpOut = [tempname '.dat'];
            reader.setProperties('path', tmpIn);
            writer.setProperties('filename', tmpOut);
            testCase.addTeardown(@() (exist(tmpOut,'file') && delete(tmpOut)));

            tg = ufo.TaskGraph();
            idR = tg.addNode(reader);
            idW = tg.addNode(writer);
            tg.connect(idR, idW);

            sched = ufo.Scheduler();
            cleanup = onCleanup(@() delete(sched)); %#ok<NASGU>
            sched.run(tg);

            info = dir(tmpOut);
            testCase.verifyTrue(~isempty(info) && info.bytes > 0);
        end

        function emptyGraphError(testCase)
            % SC-02: running an empty graph should raise an error
            sched = ufo.Scheduler();
            tg = ufo.TaskGraph();
            testCase.addTeardown(@() delete(sched));
            testCase.addTeardown(@() delete(tg));
            testCase.verifyError(@() sched.run(tg), 'ufo:Scheduler:EmptyGraph');
        end

        function stopAfterLimit(testCase)
            % SC-03: run graph with stop-after=10 parameter
            pm = ufo.PluginManager();
            src = pm.getTask('read');
            if ~ismethod(src, 'setProperties')
                testCase.assumeFail('Task property setters not implemented');
            end
            src.setProperties('path', which('README.md'), 'stop-after', 10);
            sink = pm.getTask('write');
            sink.setProperties('filename', [tempname '.dat']);
            tg = ufo.TaskGraph();
            tg.connect(tg.addNode(src), tg.addNode(sink));
            sched = ufo.Scheduler();
            tic;
            sched.run(tg);
            elapsed = toc;
            testCase.verifyLessThan(elapsed, 10);
        end

        function invalidTaskProperty(testCase)
            % SC-04: invalid task property should produce an error on run
            pm = ufo.PluginManager();
            t = pm.getTask('read');
            if ~ismethod(t, 'setProperties')
                testCase.assumeFail('Task property setters not implemented');
            end
            t.setProperties('does_not_exist', 1);
            tg = ufo.TaskGraph();
            tg.addNode(t);
            sched = ufo.Scheduler();
            testCase.verifyError(@() sched.run(tg), 'ufo:Task:Property');
        end

        function schedulerDeletedMidRun(testCase)
            % SC-05: deleting scheduler mid-run must not crash MATLAB
            if exist('parfeval','file') ~= 2
                testCase.assumeFail('Parallel toolbox required');
            end
            sched = ufo.Scheduler();
            tg = ufo.TaskGraph();
            pm = ufo.PluginManager();
            tg.addNode(pm.getTask('read'));
            f = parfeval(@() sched.run(tg), 0);
            delete(sched); %# cleanup will invalidate handle
            testCase.verifyError(@() sched.run(tg), 'ufo:Scheduler:InvalidHandle');
            cancel(f); %# stop worker if still running
        end
    end
end
