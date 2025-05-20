classdef TestSchedulerIntegration < matlab.unittest.TestCase
    %TESTSCHEDULERINTEGRATION Integration tests for ufo.Scheduler

    methods(Test)
        function readWritePipeline(testCase)
            % SC-01: read→write graph should produce a file with content
            % TODO: build a simple pipeline using the PluginManager to
            % obtain 'read' and 'write' tasks. Configure them to operate on
            % a temporary file, execute via Scheduler.run, then verify that
            % the file exists and has non-zero size.
        end

        function emptyGraphError(testCase)
            % SC-02: running an empty graph should raise an error
            % TODO: create an empty TaskGraph and verify that running it
            % with a Scheduler throws the 'ufo:Scheduler:EmptyGraph' error.
        end

        function stopAfterLimit(testCase)
            % SC-03: run graph with stop-after=10 parameter
            % TODO: configure the pipeline such that exactly 10 buffers are
            % processed and ensure Scheduler.run returns within 10 seconds.
        end

        function invalidTaskProperty(testCase)
            % SC-04: invalid task property should produce an error on run
            % TODO: set an invalid property on one of the tasks and verify
            % that Scheduler.run fails with 'ufo:Task:Property'.
        end

        function schedulerDeletedMidRun(testCase)
            % SC-05: deleting scheduler mid-run must not crash MATLAB
            % TODO: start asynchronous execution (perhaps using parfeval),
            % delete the scheduler handle, then verify that a subsequent
            % graph operation fails with a handle error rather than causing
            % a crash.
        end
    end
end
