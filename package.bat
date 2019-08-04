@echo off

cmake --build build --config Release --target install -- /verbosity:m || goto :EOF
