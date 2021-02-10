#!  /bin/sh
#   cleanup.py
#
#       Remove all build files.

echo '[ Cleanup ]'
cd ..

rm -rf ./include/*
rm -rf ./lib/*
rm -rf ./bin/*
rm -rf ./build/*
rm -rf ./source/generated/gl/*.h
rm -rf ./source/generated/*.cpp
rm -rf ./thirdparty/*/
rm -f ./thirdparty/gl.xml
rm -f ./data/lcd.h
