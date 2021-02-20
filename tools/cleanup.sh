#!  /bin/sh
#   tools/cleanup.py
#
#       Remove all build files.
#
#   Copyright (c) 2021 Mitya Selivanov
#
#   This file is part of the Laplace project.
#
#   Laplace is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty
#   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
#   the MIT License for more details.

echo '[ Cleanup ]'

rm -rf ../include/*
rm -rf ../lib/*
rm -rf ../bin/*
rm -rf ../build/*
rm -rf ../source/generated/gl/*.h
rm -rf ../source/generated/*.cpp
rm -rf ../thirdparty/*
rm -f ../data/lcd.h
rm -f ../tools/update-deps.sh
rm -f ../tools/rebuild-deps.sh
