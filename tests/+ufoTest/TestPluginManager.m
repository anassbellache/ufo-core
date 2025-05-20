classdef TestPluginManager < matlab.unittest.TestCase
    %TESTPLUGINMANAGER Validate ufo.PluginManager behaviour
    methods(Test)
        function listPlugins(testCase)
            pm = ufo.PluginManager(); %#ok<NASGU>
            %TODO: fetch plugin list via listPlugins and ensure it is a
            %non-empty cell array of character vectors
        end
    end
end
