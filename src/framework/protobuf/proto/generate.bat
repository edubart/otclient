protoc --cpp_out=./ appearances.proto shared.proto
move "%cd%\*.cc" "%cd%\..\"
move "%cd%\*.h" "%cd%\..\"
PAUSE