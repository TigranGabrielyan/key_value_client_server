cmake -S . -B ./build -G "Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug
if [[ $? -eq 0 ]]
then
    cd ./build
    make all
fi