classdef TestPluginManager < matlab.unittest.TestCase
    %TESTPLUGINMANAGER Validate ufo.PluginManager behaviour

    methods(Test)
        function listPlugins(testCase)
            pm = ufo.PluginManager();
            names = pm.listPlugins();
            testCase.verifyTrue(iscell(names) && ~isempty(names), ...
                'Plugin list should be non-empty cell array');
            testCase.verifyTrue(all(cellfun(@(x) ischar(x) || isstring(x), names)), ...
                'All entries should be character vectors');
        end
    end
end
