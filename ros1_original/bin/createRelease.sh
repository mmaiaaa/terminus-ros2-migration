#! /bin/sh

find   -name *.c -exec rm {} \;
find   -name *.cpp -exec rm {} \;
find   -name *.cc -exec rm {} \;
find   -name *.cxx -exec rm {} \;
find   -name *.c++ -exec rm {} \;


find   -name *.h -exec rm {} \;
find   -name *.hpp -exec rm {} \;
find   -name *.hxx -exec rm {} \;

cd  /home/anbot/.local/share/Trash/files
if [ $? -eq 0 ]; then
	rm   -rf   *
fi