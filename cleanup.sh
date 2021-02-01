#!  /bin/sh
#   cleanup.py
#
#       Remove all build files.

echo '[ Cleanup ]'

rm -rf ./include/*
rm -rf ./lib/*
rm -rf ./bin/*
rm -rf ./build/*
rm -rf ./source/generated/gl/*.h
rm -rf ./source/generated/*.cpp
rm -rf ./thirdparty/*/
rm -f ./thirdparty/gl.xml
rm -f ./data/lcd.h

touch ./include/.placeholder
touch ./lib/.placeholder
touch ./bin/.placeholder
touch ./build/.placeholder
touch ./source/generated/gl/.placeholder
