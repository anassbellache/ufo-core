classdef TestTaskGraph < matlab.unittest.TestCase
    %TESTTASKGRAPH Unit tests for ufo.TaskGraph.
    % Contains placeholders for TG-01..TG-06 specifications.
    methods(Test)
        function addNodeReturnsId(testCase)
            % TG-01: empty graph -> addNode(readTask) returns node id 0
            % TODO: create TaskGraph, add 'read' task and verify returned id.
        end

        function connectValid(testCase)
            % TG-02: existing nodes -> connect(0,0,1,0) succeeds
            % TODO: connect two nodes and ensure no error is thrown.
        end

        function connectSelfEdgeError(testCase)
            % TG-03: same nodes -> connect(0,0,0,0) raises SelfEdge
            % TODO: verify the correct error identifier is thrown.
        end

        function connectBadSource(testCase)
            % TG-04: node count >0 -> connect(99,0,0,0) -> BadSrc
            % TODO: attempt invalid connection and check error id.
        end

        function validateDetectsCycle(testCase)
            % TG-06: graph with cycle -> validate() returns false or errors
            % TODO: build cyclic graph and ensure validate detects cycle.
        end
    end
end
