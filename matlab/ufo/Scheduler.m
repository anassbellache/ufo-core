classdef Scheduler < handle
    %SCHEDULER Manages hardware resources and runs TaskGraphs in UFO-Core
    %   The Scheduler class is responsible for managing hardware resources (CPUs,
    %   GPUs) and executing ufo.TaskGraph objects within the UFO framework.
    %   It provides methods for synchronous and asynchronous graph execution.
    %
    %   Scheduler Methods:
    %     Scheduler      - Create a new scheduler instance.
    %     setResources   - Assign a ufo.Resources object to the scheduler.
    %     getResources   - Retrieve the ufo.Resources object currently used by the scheduler.
    %     run            - Execute a ufo.TaskGraph synchronously.
    %     runAsync       - Execute a ufo.TaskGraph asynchronously, returning a Future object.
    %     stop           - Abort any tasks currently running on this scheduler.
    %     delete         - Release the underlying UFO-Core scheduler resources.
    %
    %   Example:
    %     % Create a plugin manager, a task graph, and a scheduler
    %     pm = ufo.PluginManager();
    %     tg = ufo.TaskGraph();
    %     sched = ufo.Scheduler();
    %
    %     % (Build the task graph tg using pm and tg.addNode/connect)
    %     % For example:
    %     % reader = pm.getTask('read'); reader.setProperties('path', 'input.tif');
    %     % writer = pm.getTask('write'); writer.setProperties('filename', 'output.tif');
    %     % tg.addNode(reader); tg.addNode(writer);
    %     % tg.connect(reader, writer);
    %
    %     % Allocate resources (optional, default resources are used otherwise)
    %     res = ufo.Resources();
    %     % res.setCPUThreads(4); % Example: configure resources
    %     % res.setGPU('0');      % Example: configure resources
    %     sched.setResources(res);
    %
    %     % Run the graph synchronously
    %     results = sched.run(tg);
    %     disp('Synchronous run completed.');
    %
    %     % Alternatively, run asynchronously (requires a Future class if not just a handle)
    %     % future_handle = sched.runAsync(tg);
    %     % disp('Asynchronous run started.');
    %     % % ... do other work ...
    %     % % results_async = future_handle.getResult(); % Assuming Future has getResult
    %
    %     % Clean up
    %     delete(sched);
    %     delete(tg);
    %     delete(pm);
    %     % delete(res); % If res was created

    properties (Access = private)
        Handle uint64 = 0 % Opaque pointer to UfoBaseScheduler
    end

    methods
        function obj = Scheduler()
            %SCHEDULER Create a new scheduler instance.
            %   SCHED = ufo.Scheduler() creates a new scheduler that can be
            %   used to manage resources and execute task graphs.
            obj.Handle = ufo_mex('Scheduler_new');
            if obj.Handle == 0
                error('ufo:Scheduler:InitFailed', ...
                      'Failed to create UFO scheduler.');
            end
        end

        function setResources(obj, res)
            %SETRESOURCES Assign a ufo.Resources object to the scheduler.
            %   SCHED.setResources(RES) sets the hardware resources to be
            %   used by the scheduler.
            %
            %   Inputs:
            %     RES - A ufo.Resources object.
            if obj.Handle == 0
                error('ufo:Scheduler:InvalidHandle', ...
                      'Scheduler has been deleted or not initialized.');
            end
            if ~isa(res, 'ufo.Resources')
                error('ufo:Scheduler:InvalidArgument', ...
                      'Input must be a ufo.Resources object.');
            end
            if res.Handle == 0
                 error('ufo:Scheduler:InvalidResourcesHandle', ...
                      'The provided ufo.Resources object has an invalid handle.');
            end
            ufo_mex('Scheduler_setResources', obj.Handle, res.Handle);
        end

        function res = getResources(obj)
            %GETRESOURCES Retrieve the ufo.Resources object currently used by the scheduler.
            %   RES = SCHED.getResources() returns the ufo.Resources object
            %   that defines the hardware resources available to this scheduler.
            %
            %   Returns:
            %     RES - A ufo.Resources object.
            if obj.Handle == 0
                error('ufo:Scheduler:InvalidHandle', ...
                      'Scheduler has been deleted or not initialized.');
            end
            resHandle = ufo_mex('Scheduler_getResources', obj.Handle);
            if resHandle == 0
                error('ufo:Scheduler:GetResourcesFailed', ...
                      'Failed to retrieve resources from scheduler. Perhaps none are set.');
            end
            res = ufo.Resources(resHandle); % Assumes ufo.Resources can be constructed with a handle
        end

        function results = run(obj, graph)
            %RUN Execute a ufo.TaskGraph synchronously.
            %   RESULTS = SCHED.run(GRAPH) executes the given ufo.TaskGraph
            %   synchronously. The call blocks until execution is complete.
            %   The nature of RESULTS depends on the graph.
            %
            %   Inputs:
            %     GRAPH - A ufo.TaskGraph object.
            %
            %   Returns:
            %     RESULTS - Output from the graph execution, type varies.
            if obj.Handle == 0
                error('ufo:Scheduler:InvalidHandle', ...
                      'Scheduler has been deleted or not initialized.');
            end
            if ~isa(graph, 'ufo.TaskGraph')
                error('ufo:Scheduler:InvalidArgument', ...
                      'Input must be a ufo.TaskGraph object.');
            end
            if graph.Handle == 0
                 error('ufo:Scheduler:InvalidGraphHandle', ...
                      'The provided ufo.TaskGraph object has an invalid handle.');
            end
            results = ufo_mex('Scheduler_run', obj.Handle, graph.Handle);
        end

        function future = runAsync(obj, graph)
            %RUNASYNC Execute a ufo.TaskGraph asynchronously.
            %   FUTURE = SCHED.runAsync(GRAPH) starts the execution of the
            %   given ufo.TaskGraph asynchronously and returns a handle
            %   representing the future result. This handle might be for a
            %   ufo.Future object if such a class exists and is implemented.
            %
            %   Inputs:
            %     GRAPH - A ufo.TaskGraph object.
            %
            %   Returns:
            %     FUTURE - A handle or object representing the asynchronous operation's future result.
            %              The exact type depends on the MEX implementation (e.g., uint64 handle to a GTask).
            if obj.Handle == 0
                error('ufo:Scheduler:InvalidHandle', ...
                      'Scheduler has been deleted or not initialized.');
            end
            if ~isa(graph, 'ufo.TaskGraph')
                error('ufo:Scheduler:InvalidArgument', ...
                      'Input must be a ufo.TaskGraph object.');
            end
            if graph.Handle == 0
                 error('ufo:Scheduler:InvalidGraphHandle', ...
                      'The provided ufo.TaskGraph object has an invalid handle.');
            end
            % Assuming ufo_mex returns a handle that can be used with a potential ufo.Future class
            futureHandle = ufo_mex('Scheduler_runAsync', obj.Handle, graph.Handle);
            % If a ufo.Future class is intended to wrap this handle:
            % future = ufo.Future(futureHandle);
            % Otherwise, just return the handle:
            future = futureHandle;
        end

        function stop(obj)
            %STOP Abort any tasks currently running on this scheduler.
            %   SCHED.stop() attempts to stop all processing tasks currently
            %   being executed by this scheduler.
            if obj.Handle == 0
                error('ufo:Scheduler:InvalidHandle', ...
                      'Scheduler has been deleted or not initialized. Cannot stop.');
            end
            ufo_mex('Scheduler_stop', obj.Handle);
        end

        function delete(obj)
            %DELETE Release the underlying UFO-Core scheduler resources.
            %   delete(SCHED) is called automatically when the Scheduler object
            %   is destroyed. It can also be called manually. This method
            %   releases the resources associated with the underlying
            %   UfoBaseScheduler GObject.
            if obj.Handle ~= 0
                ufo_mex('Scheduler_free', obj.Handle);
                obj.Handle = 0; % Mark as deleted
            end
        end
    end
end
