# test task from YADRO
System prototype which is responsible for inner workings of computer club.

# initial setup
clone in prefered derictory 
```
git clone https://github.com/skyfaierr/yadro_club
```
goto root folder
```
cd yadro_club
```

# using and testing
build it
```
mkdir -p build
cd build
cmake ..
cmake --build .
```
test it
```
./main <path/text.txt>
```
programm will put result in <output.txt> file

some ambiguous conditions can be deducted, but the one i accepted is:
all events after closing are not outputed in our file.
