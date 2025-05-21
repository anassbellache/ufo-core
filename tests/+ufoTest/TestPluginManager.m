classdef TestPluginManager < matlab.unittest.TestCase
    %TESTPLUGINMANAGER Validate ufo.PluginManager behaviour
    methods(Test)
        function listPlugins(testCase)

            % PM-01: Plugin paths valid -> listPlugins returns non-empty
            pm = ufo.PluginManager();
            names = pm.listPlugins();
            testCase.verifyFalse(isempty(names), 'Plugin list is empty');
            testCase.verifyTrue(iscellstr(names), 'Expected cell array of strings');
        end

        function getTaskValid(testCase)
            % PM-02: valid manager -> getTask("read") returns ufo.Task
            pm = ufo.PluginManager();
            task = pm.getTask("read");
            testCase.verifyTrue(isa(task, 'ufo.Task'));
        end

        function getTaskNotFound(testCase)
            % PM-03: requesting missing plugin raises specific error
            pm = ufo.PluginManager();
            testCase.verifyError(@() pm.getTask("NOTEXIST"), ...
                'ufo:PluginManager:NotFound');
        end

        function listIncludesEnvPlugin(testCase)
            % PM-04: override search path with custom plugin directory
            pluginDir = TestPluginManager.locateSystemPluginDir();
            testCase.assumeNotEmpty(pluginDir, 'System plugin directory not found');

            pluginFiles = dir(fullfile(pluginDir, 'libufofilter*.so'));
            testCase.assumeGreaterThan(numel(pluginFiles), 0, 'No plugins to copy');

            src = fullfile(pluginDir, pluginFiles(1).name);
            tmpDir = tempname;
            mkdir(tmpDir);
            copyfile(src, tmpDir);

            oldPath = getenv('UFO_PLUGIN_PATH');
            cleanup = onCleanup(@() setenv('UFO_PLUGIN_PATH', oldPath)); %#ok<NASGU>
            setenv('UFO_PLUGIN_PATH', tmpDir);

            pm = ufo.PluginManager();
            names = pm.listPlugins();
            token = regexp(pluginFiles(1).name, 'libufofilter(.+)\.so', 'tokens', 'once');
            expected = token{1};
            testCase.verifyTrue(ismember(expected, names));
        end

        function invalidAfterDelete(testCase)
            % PM-05: calling method after delete -> MATLAB:class:InvalidHandle
            pm = ufo.PluginManager();
            delete(pm);
            testCase.verifyError(@() pm.listPlugins(), 'MATLAB:class:InvalidHandle');
        end
    end

    methods(Static, Access = private)
        function dirPath = locateSystemPluginDir()
            % Helper to find one installed plugin directory on typical systems
            candidates = { ...
                '/usr/lib/ufo', '/usr/lib/ufo/plugins', ...
                '/usr/local/lib/ufo', '/usr/local/lib/ufo/plugins', ...
                '/usr/lib64/ufo', '/usr/lib64/ufo/plugins', ...
                '/usr/local/lib64/ufo', '/usr/local/lib64/ufo/plugins' };

            dirPath = '';
            for i = 1:numel(candidates)
                d = candidates{i};
                if isfolder(d) && ~isempty(dir(fullfile(d, 'libufofilter*.so')))
                    dirPath = d;
                    return;
                end
            end
        end
    end
end
