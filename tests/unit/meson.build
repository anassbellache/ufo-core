sources = [
    'test-suite.c',
    'test-buffer.c',
    'test-graph.c',
    'test-node.c',
    'test-profiler.c',
    'test-max-input-nodes.cpp',
    'test-handle-concurrency.c'
    'test-opencl.c'
    'test-mex-handle.c'
]

test('unit tests',
    executable('test-suite',
        sources: sources + [enums_h],
        include_directories: [include_dir, '../matlab/include'],
        dependencies: deps,
        link_with: lib,
        override_options : ['cpp_std=c++11']
    )
)
