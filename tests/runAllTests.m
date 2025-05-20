import matlab.unittest.TestSuite;
testRoot = fileparts(mfilename('fullpath'));
addpath(fullfile(testRoot, '..', 'matlab'));    % add +ufo package
results = run(TestSuite.fromPackage('ufoTest','IncludeSubpackages',true));
assertSuccess(results);
