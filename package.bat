@echo off

cmake --build _build --config Release --target install -- /verbosity:m || goto :EOF
