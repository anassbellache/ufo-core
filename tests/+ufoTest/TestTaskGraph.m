classdef TestTaskGraph < matlab.unittest.TestCase
    %TESTTASKGRAPH Validate TaskGraph topology rules
    properties
        PM  % plugin manager used to create tasks
    end
    methods(TestMethodSetup)
        function createPluginManager(testCase)
            testCase.PM = ufo.PluginManager();
        end
    end
    methods(TestMethodTeardown)
        function deletePluginManager(testCase)
            delete(testCase.PM);
        end
    end
    methods(Test)
        function addNodeReturnsZero(testCase)
            % TG-01: empty graph -> addNode returns node id 0
            tg = ufo.TaskGraph();
            readTask = testCase.PM.getTask("read");
            id = tg.addNode(readTask);
            testCase.verifyEqual(id, uint64(0));
        end
        function connectValidNodes(testCase)
            % TG-02: connecting existing nodes succeeds
            tg = ufo.TaskGraph();
            a = testCase.PM.getTask("read");
            b = testCase.PM.getTask("write");
            idA = tg.addNode(a);
            idB = tg.addNode(b);
            testCase.verifyWarningFree(@() tg.connect(idA, idB));
        end
        function connectSelfEdgeFails(testCase)
            % TG-03: connecting a node to itself must raise an error
            tg = ufo.TaskGraph();
            a = testCase.PM.getTask("read");
            id = tg.addNode(a);
            testCase.verifyError(@() tg.connect(id, id), ...
                'ufo:TaskGraph:SelfEdge');
        end
        function connectBadSrcFails(testCase)
            % TG-04: source id must exist
            tg = ufo.TaskGraph();
            a = testCase.PM.getTask("read");
            tg.addNode(a);
            testCase.verifyError(@() tg.connect(99, 0), ...
                'ufo:TaskGraph:BadSrc');
        end
        function runNoCycle(testCase)
            % TG-05: scheduler run should succeed for acyclic graph
            tg = ufo.TaskGraph();
            a = testCase.PM.getTask("read");
            b = testCase.PM.getTask("write");
            idA = tg.addNode(a);
            idB = tg.addNode(b);
            tg.connect(idA, idB);
            sched = ufo.Scheduler();
            testCase.verifyWarningFree(@() sched.run(tg));
        end
        function detectCycle(testCase)
            % TG-06: cycle detection should fail validation/run
            tg = ufo.TaskGraph();
            a = testCase.PM.getTask("read");
            b = testCase.PM.getTask("write");
            idA = tg.addNode(a);
            idB = tg.addNode(b);
            tg.connect(idA, idB);
            tg.connect(idB, idA); % creates cycle
            if ismethod(tg, 'validate')
                testCase.verifyError(@() tg.validate(), ...
                    'ufo:TaskGraph:Cyclic');
            else
                sched = ufo.Scheduler();
                testCase.verifyError(@() sched.run(tg), ...
                    'ufo:TaskGraph:Cyclic');
            end
        end
    end
end
