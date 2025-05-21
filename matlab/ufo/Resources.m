classdef Resources < handle
    properties (Access = private)
        Handle uint64 = 0
    end
    methods
        function obj = Resources()
            obj.Handle = ufo_mex('Resources_new');
            if obj.Handle == 0
                error('ufo:Resources:InitFailed','Failed to create resources.');
            end
        end
        function delete(obj)
            if obj.Handle ~= 0
                try
                    ufo_mex('Resources_delete', obj.Handle);
                catch ME
                    warning('ufo:Resources:FreeFailed','Failed to free resources handle %d: %s',obj.Handle,ME.message);
                end
                obj.Handle = 0;
            end
        end
        function assertValid(obj)
            if obj.Handle == 0
                error('ufo:Resources:InvalidHandle','Resources handle invalid or deleted.');
            end
        end
    end
end
