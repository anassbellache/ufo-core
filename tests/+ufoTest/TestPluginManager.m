classdef TestPluginManager < matlab.unittest.TestCase
    %TESTPLUGINMANAGER Unit tests for ufo.PluginManager behaviour.
    % Each method refers to requirements PM-01..PM-05.
    methods(Test)
        function listNonEmpty(testCase)
            % PM-01: Plugin paths valid -> listPlugins returns non-empty cellstr
            % TODO: instantiate PluginManager and verify listPlugins returns a
            % non-empty cell array of character vectors.
        end

        function getTaskReturnsObject(testCase)
            % PM-02: valid manager -> getTask("read") returns ufo.Task
            % TODO: retrieve 'read' task and check its class.
        end

        function getTaskNotFound(testCase)
            % PM-03: valid manager -> getTask("NOTEXIST") should throw
            % 'ufo:PluginManager:NotFound'.
            % TODO: verify error is raised.
        end

        function customPathIncluded(testCase)
            % PM-04: manager search path overridden -> list includes custom plugin
            % TODO: set environment variable to path containing one plugin and
            % ensure listPlugins includes it.
        end

        function invalidAfterDelete(testCase)
            % PM-05: manager deleted -> subsequent method call errors with
            % 'MATLAB:class:InvalidHandle'.
            % TODO: delete manager then call listPlugins and verify error.
        end
    end
end
