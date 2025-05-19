classdef PluginManager < handle
%PLUGINMANAGER  Wrapper for UFO-Core UfoPluginManager
%
%   pm  = ufo.PluginManager();
%   all = pm.listPlugins();          % cellstr of names
%   t   = pm.getTask("read");        % returns ufo.Task
%
%   The underlying GObject is released automatically when the MATLAB
%   object is deleted or goes out of scope.

    properties (Access = private)
        Handle uint64 = 0   % UfoPluginManager*
    end

    %% Construction / destruction
    methods
        function obj = PluginManager
            obj.Handle = ufo_mex('PluginManager_new');
            if obj.Handle == 0
                error('ufo:PluginManager:InitFailed', ...
                      'Unable to create UfoPluginManager.');
            end
            cleaner = onCleanup(@()obj.safeFree()); %#ok<NASGU>
        end

        function delete(obj)
            obj.safeFree();
        end
    end

    %% Public API -------------------------------------------------------
    methods
        function names = listPlugins(obj)
            %LISTPLUGINS  Return cell array of available plugin names
            obj.assertValid();
            names = ufo_mex('PluginManager_listPlugins', obj.Handle);
            if ischar(names)   % mex may return char vector for single entry
                names = {names};
            end
        end

        function task = getTask(obj, name)
            %GETTASK  Instantiate plugin by name -> ufo.Task
            obj.assertValid();
            arguments
                obj
                name {mustBeTextScalar}
            end
            hTask = ufo_mex('PluginManager_getTask', obj.Handle, char(name));
            if hTask == 0
                error('ufo:PluginManager:CreateTaskFailed', ...
                      'Plugin "%s" not found or failed to instantiate.', name);
            end
            task = ufo.Task(hTask);
        end
    end

    %% Helper utilities -------------------------------------------------
    methods (Access = private)
        function assertValid(obj)
            if obj.Handle == 0
                error('ufo:PluginManager:InvalidHandle', ...
                      'PluginManager already freed / not valid.');
            end
        end

        function safeFree(obj)
            if obj.Handle ~= 0
                try
                    ufo_mex('PluginManager_free', obj.Handle);
                catch ME
                    warning('ufo:PluginManager:FreeFailed', ...
                        'Failed to free PluginManager handle %d: %s', obj.Handle, ME.message);
                end
                obj.Handle = 0;
            end
        end
    end
end
