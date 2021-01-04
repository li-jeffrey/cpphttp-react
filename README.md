# cpphttp-react
A whiteapp for building a React web app with C++17 http backend. Tested on WSL2 Debian 8.3 with gcc 8.3.

## Building the server
All third-party libraries should be included in the form of git submodules. Cmake is required to compile some of these dependencies.
```
git clone --recurse-submodules https://github.com/li-jeffrey/cpphttp-react.git
cd cpphttp-react
make
```

## Running server tests
```
make test
./src/cpphttp-react-test
```

## Building the webapp
Located in the `webapp/` folder of this repo, npm is required to build it. Only tested with npm version 6.14.8.
```
make webapp
```

## Starting the application
Currently the server is hardcoded to bind to port 6123, and serves from its working directory. Run the following commands to start the server:
```
cd webapp/build
../../src/cpphttp-react
```

And navigate to http://localhost:6123 to view the demo.
