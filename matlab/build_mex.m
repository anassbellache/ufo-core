cwd = pwd;
buildDir = fullfile(cwd,'build-matlab');
if ~isfolder(buildDir); mkdir(buildDir); end
system(sprintf('cmake -S"%s" -B"%s"', cwd, buildDir));
system(sprintf('cmake --build "%s" --target install', buildDir));
