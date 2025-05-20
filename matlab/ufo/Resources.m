classdef Resources < handle
%RESOURCES  Wrapper for a UFO-Core UfoResources handle
%   This class manages a UfoResources* which holds OpenCL contexts and related
%   GPU/CPU resource information used by the Scheduler.

    properties (Access = private)
        Handle uint64 = 0   % Opaque pointer to UfoResources*
    end

    methods
        function obj = Resources(handle)
            %RESOURCES Create a new or wrap an existing UfoResources handle
            if nargin == 0
                h = ufo_mex('Resources_new');
            else
                arguments
                    handle (1,1) {mustBeNumeric, mustBeFinite, mustBePositive}
                end
                h = uint64(handle);
            end
            if h == 0
                error('ufo:Resources:InitFailed', ...
                      'Failed to obtain a valid UfoResources handle.');
            end
            obj.Handle = h;
            cleaner = onCleanup(@()obj.safeFree()); %#ok<NASGU>
        end

        function delete(obj)
            %DELETE Release underlying UfoResources handle
            obj.safeFree();
        end
    end

    methods (Access = {?ufo.Scheduler})
        function assertValid(obj)
            if obj.Handle == 0
                error('ufo:Resources:InvalidHandle', ...
                      'This Resources object has been deleted or was never valid.');
            end
        end
    end

    methods (Access = private)
        function safeFree(obj)
            if obj.Handle ~= 0
                try
                    ufo_mex('Resources_free', obj.Handle);
                catch ME
                    warning('ufo:Resources:FreeFailed', ...
                            'Failed to free Resources handle %d: %s', ...
                            obj.Handle, ME.message);
                end
                obj.Handle = 0;
            end
        end
    end
end
