classdef Buffer < handle
%BUFFER  MATLAB wrapper of a UFO-Core UfoBuffer
%
%   buf = ufo.Buffer()                % create an empty GPU/host buffer
%   buf = ufo.Buffer(nBytes)          % allocate buffer of given byte size
%   buf = ufo.Buffer(dims)            % allocate buffer with product(dims) bytes
%   buf = ufo.Buffer(h)               % wrap existing uint64 handle
%   sz  = buf.getSize()               % bytes in buffer
%   A   = buf.getData()               % MATLAB array copy of the data
%
%   All resources are freed automatically when the object is deleted or
%   goes out of scope.

    properties (Access = private)
        Handle uint64 = 0   % Opaque pointer to UfoBuffer*
    end

    %% Construction / destruction
    methods
        function obj = Buffer(arg)
            %BUFFER  Construct new or wrap existing UfoBuffer
            %
            %   ufo.Buffer()          – allocate an empty buffer
            %   ufo.Buffer(N)         – allocate buffer with N bytes
            %   ufo.Buffer(DIMS)      – allocate buffer of prod(DIMS) bytes
            %   ufo.Buffer(uint64 H)  – wrap an existing native handle

            if nargin == 0
                % create an empty buffer
                nbytes = 0;
                obj.Handle = ufo_mex('Buffer_new', nbytes);
            elseif isa(arg, 'uint64')
                % wrap existing handle
                arguments
                    arg (1,1) {mustBeNumeric, mustBeFinite, mustBePositive}
                end
                obj.Handle = uint64(arg);
            else
                % treat argument as byte count or dimension vector
                arguments
                    arg {mustBeNumeric, mustBeFinite, mustBeNonnegative}
                end

                if isscalar(arg)
                    nbytes = double(arg);
                else
                    nbytes = prod(double(arg));
                end
                obj.Handle = ufo_mex('Buffer_new', nbytes);
            end

            if obj.Handle == 0
                error('ufo:Buffer:InitFailed','Failed to obtain UfoBuffer handle.');
            end

            % If a later error happens before constructor ends, be sure to free:
            cleaner = onCleanup(@()obj.safeFree());
            % … additional init steps could go here …
            clear cleaner   % successful construction, cancel automatic free.
        end

        function delete(obj)
            %DELETE  Release the underlying UfoBuffer*
            obj.safeFree();
        end
    end

    %% Public API
    methods
        function A = getData(obj)
        %GETDATA  Retrieve buffer contents as MATLAB array
            obj.assertValid();

            % The MEX side must marshal the buffer into a MATLAB array.
            % Ideally it returns the correct numeric class (single, uint16, …).
            A = ufo_mex('Buffer_getData', obj.Handle);
        end

        function n = getSize(obj)
        %GETSIZE  Number of bytes held by this buffer
            obj.assertValid();
            n = ufo_mex('Buffer_getSize', obj.Handle);
        end
    end

    %% Helpers
    methods (Access = private)
        function assertValid(obj)
            if obj.Handle == 0
                error('ufo:Buffer:InvalidHandle',...
                      'This Buffer has been deleted or was never valid.');
            end
        end

        function safeFree(obj)
            %SAFEFREE  Internal guard that never throws
            if obj.Handle ~= 0
                try
                    ufo_mex('Buffer_free', obj.Handle);
                catch ME
                    warning('ufo:Buffer:FreeFailed',...
                            'Failed to free UfoBuffer (handle %d): %s',...
                            obj.Handle, ME.message);
                end
                obj.Handle = 0;
            end
        end
    end
end
