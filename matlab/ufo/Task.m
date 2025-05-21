classdef Task < handle
    properties (Access = private)
        Handle uint64 = 0
    end
    methods
        function obj = Task(handle)
            arguments
                handle (1,1) uint64
            end
            obj.Handle = handle;
        end
        function delete(obj)
            if obj.Handle ~= 0
                try
                    ufo_mex('Task_delete', obj.Handle);
                catch ME
                    warning('ufo:Task:FreeFailed','Failed to free Task handle %d: %s',obj.Handle,ME.message);
                end
                obj.Handle = 0;
            end
        end
        function assertValid(obj)
            if obj.Handle == 0
                error('ufo:Task:InvalidHandle','Task handle is invalid or has been deleted.');
            end
        end
    end
end
