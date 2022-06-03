protoc --cpp_out=./ appearances.proto
move "%cd%\*.cc" "%cd%\..\"
move "%cd%\*.h" "%cd%\..\"
PAUSE
