classdef TaskGraph < handle
%TASKGRAPH  MATLAB wrapper of a UFO-Core UfoTaskGraph (directed acyclic graph)
%
%   g   = ufo.TaskGraph();                 % empty graph
%   id  = g.addNode(taskObj);              % returns node-id
%   g.connect(src, dst);                   % by id / handle / name
%   g.loadFromFile("pipeline.json");       % import DAG
%   g.saveToFile("out.json");              % export DAG
%   names = g.listNodes();                 % cellstr of node names
%   result = g.run(sched);                 % execute synchronously
%
%  All native resources are released automatically.

    properties (Access = private)
        Handle uint64 = 0    % Opaque pointer to UfoTaskGraph*
    end

    %% Construction / destruction
    methods
        function obj = TaskGraph
            obj.Handle = ufo_mex('TaskGraph_new');
            if obj.Handle == 0
                error('ufo:TaskGraph:InitFailed','Unable to create UfoTaskGraph.');
            end
            cleaner = onCleanup(@()obj.safeFree()); %#ok<NASGU>
        end

        function delete(obj)
            obj.safeFree();
        end
    end

    %% Public API -------------------------------------------------------
    methods
        function loadFromFile(obj, file)
            obj.assertValid();
            arguments
                obj
                file (1,1) string {mustBeFile(file)}
            end
            ufo_mex('TaskGraph_loadFromFile', obj.Handle, char(file));
        end

        function nodeId = addNode(obj, task)
            obj.assertValid();
            arguments
                obj
                task {mustBeA(task,'ufo.Task')}
            end
            task.assertValid();           % uses method we add to ufo.Task
            nodeId = ufo_mex('TaskGraph_addNode', obj.Handle, task.Handle);
        end

        function connect(obj, src, dst)
            %CONNECT  Wire node output→input.
            %
            %   src / dst may be:
            %     • ufo.Task      (wraps handle)
            %     • uint64        (native handle)
            %     • double int    (node-id)
            %     • string/char   (unique node name)
            %
            obj.assertValid();
            srcVal = obj.normaliseEndpoint(src,'SRCNODE');
            dstVal = obj.normaliseEndpoint(dst,'DSTNODE');
            ufo_mex('TaskGraph_connect', obj.Handle, srcVal, dstVal);
        end

        function names = listNodes(obj)
            obj.assertValid();
            names = ufo_mex('TaskGraph_listNodes', obj.Handle);
        end

        function result = run(obj, scheduler)
            obj.assertValid();
            arguments
                obj
                scheduler {mustBeA(scheduler,'ufo.Scheduler')}
            end
            scheduler.assertValid();
            result = ufo_mex('TaskGraph_run', obj.Handle, scheduler.Handle);
        end

        function saveToFile(obj, file)
            obj.assertValid();
            arguments
                obj
                file (1,1) string
            end
            ufo_mex('TaskGraph_saveToFile', obj.Handle, char(file));
        end
    end

    %% Helper utilities -------------------------------------------------
    methods (Access = private)
        function v = normaliseEndpoint(~, x, argName)
            % Convert different endpoint representations into something the
            % MEX layer accepts (either uint64 handle or char name).
            if isa(x,'ufo.Task')
                x.assertValid();
                v = x.Handle;
            elseif isnumeric(x) && isscalar(x)
                v = uint64(x);
            elseif isstring(x) || ischar(x)
                if strlength(x)==0
                    error('ufo:TaskGraph:EmptyName','%s name cannot be empty.',argName);
                end
                v = char(x);
            else
                error('ufo:TaskGraph:BadEndpoint',...
                      '%s must be ufo.Task, scalar id, or node name',argName);
            end
        end

        function assertValid(obj)
            if obj.Handle == 0
                error('ufo:TaskGraph:InvalidHandle','Graph already freed / not valid.');
            end
        end

        function safeFree(obj)
            if obj.Handle ~= 0
                try
                    ufo_mex('TaskGraph_free', obj.Handle);
                catch ME
                    warning('ufo:TaskGraph:FreeFailed', ...
                        'Failed to free TaskGraph handle %d: %s', obj.Handle, ME.message);
                end
                obj.Handle = 0;
            end
        end
    end
end

% ---------- local validation helper ----------
function mustBeFile(f)
if exist(f,'file')~=2
    error('ufo:TaskGraph:FileNotFound','File "%s" not found.',f);
end
end
