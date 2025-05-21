import matlab.unittest.TestSuite;
addpath(fullfile(fileparts(mfilename('fullpath')), '..'));  % add +ufo
results = run(TestSuite.fromPackage('ufoTest','IncludeSubpackages',true));
assertSuccess(results);
