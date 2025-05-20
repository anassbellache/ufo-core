classdef TestTaskGraph < matlab.unittest.TestCase
    methods(Test)
        function addNodeReturnsZero(testCase)
            % TG-01: Adding the first node should return id 0
            pm = ufo.PluginManager();
            tg = ufo.TaskGraph();
            readTask = pm.getTask("read");
            nodeId = tg.addNode(readTask);
            testCase.verifyEqual(nodeId, 0);
        end

        function connectValidNodes(testCase)
            % TG-02: Connecting two existing nodes succeeds
            pm = ufo.PluginManager();
            tg = ufo.TaskGraph();
            id0 = tg.addNode(pm.getTask("read"));
            id1 = tg.addNode(pm.getTask("write"));
            tg.connect(id0, id1); % should not error
        end

        function connectSelfEdgeError(testCase)
            % TG-03: Connecting a node to itself must raise an error
            pm = ufo.PluginManager();
            tg = ufo.TaskGraph();
            id0 = tg.addNode(pm.getTask("read"));
            testCase.verifyError(@() tg.connect(id0, id0), ...
                'ufo:TaskGraph:SelfEdge');
        end

        function connectBadSource(testCase)
            % TG-04: Non-existent source node should trigger BadSrc
            pm = ufo.PluginManager();
            tg = ufo.TaskGraph();
            id1 = tg.addNode(pm.getTask("write"));
            testCase.verifyError(@() tg.connect(99, id1), ...
                'ufo:TaskGraph:BadSrc');
        end

        function validateDetectsCycle(testCase)
            % TG-06: validate() fails on cyclic graphs
            pm = ufo.PluginManager();
            tg = ufo.TaskGraph();
            a = tg.addNode(pm.getTask("read"));
            b = tg.addNode(pm.getTask("write"));
            tg.connect(a, b);
            tg.connect(b, a); % introduce cycle
            valid = tg.validate();
            testCase.verifyFalse(valid);
        end
    end
end
