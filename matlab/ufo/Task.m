classdef Task < handle
%TASK  Wrapper for a UFO-Core UfoTask handle
%   Instances of this class manage the lifetime of a native UfoTask*. They are
%   usually created via ufo.PluginManager.getTask().

    properties (Access = private)
        Handle uint64 = 0   % Opaque pointer to UfoTask*
    end

    methods
        function obj = Task(handle)
            %TASK Construct by wrapping an existing handle
            if nargin == 0
                error('ufo:Task:NoConstructor', ...
                      'Use PluginManager.getTask to create Task objects.');
            end
            arguments
                handle (1,1) {mustBeNumeric, mustBeFinite, mustBePositive}
            end
            obj.Handle = uint64(handle);
            cleaner = onCleanup(@()obj.safeFree()); %#ok<NASGU>
        end

        function delete(obj)
            %DELETE Release underlying task handle
            obj.safeFree();
        end
    end

    methods (Access = ?ufo.TaskGraph)
        function assertValid(obj)
            if obj.Handle == 0
                error('ufo:Task:InvalidHandle', ...
                      'This Task has been deleted or was never valid.');
            end
        end
    end

    methods (Access = private)
        function safeFree(obj)
            if obj.Handle ~= 0
                try
                    ufo_mex('Task_free', obj.Handle);
                catch ME
                    warning('ufo:Task:FreeFailed', ...
                            'Failed to free Task handle %d: %s', ...
                            obj.Handle, ME.message);
                end
                obj.Handle = 0;
            end
        end
    end
end
