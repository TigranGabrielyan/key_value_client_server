cmake -S . -B ./build -G "Unix Makefiles"
if [[ $? -eq 0 ]]
then
    cd build
    make all
fi