classdef TestConcurrency < matlab.unittest.TestCase
    %TESTCONCURRENCY Exercises multi-worker behaviour of bindings
    methods(Test)
        function twoPluginManagers(testCase)
            % CC-01 two MATLAB workers create PluginManager in parallel
            % TODO: start a parpool with 2 workers and ensure each can
            % create a ufo.PluginManager without race conditions.
        end
        function schedulerBusy(testCase)
            % CC-03 ensure Scheduler reports busy when run is called twice
            % TODO: run Scheduler.runAsync then immediately run again and
            % verify error id 'ufo:Scheduler:Busy'.
        end
    end
end
