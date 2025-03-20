# pegcalc

command line calculator for experimenting with PEG grammar


testing environment:

    Using taocpp/pegtl installation (see https://taocpp.github.io)  
    -- for testing, chose to install in /opt/taocpp/pegtl ...)  
    Using WSL2 linux ubuntu 2022  
   
ubuntu> cat /etc/lsb-release  
DISTRIB_ID=Ubuntu  
DISTRIB_RELEASE=22.04  
DISTRIB_CODENAME=jammy  
DISTRIB_DESCRIPTION="Ubuntu 22.04.5 LTS"  

ubuntu> clang --version   
clang version 17.0.6 (https://github.com/llvm/llvm-project 6009708b4367171ccdbf4b5905cb6a803753fe18)  
Target: x86_64-unknown-linux-gnu  
Thread model: posix  
InstalledDir: /opt/llvm/bin  

ubuntu> cmake --version  
cmake version 3.30.1  

CMake suite maintained and supported by Kitware (kitware.com/cmake).  

ubuntu> ninja --version  
1.11.1

linux> mkdir build  
linux> cd build  
linux> cmake -G Ninja ..  
linux> ninja  
linux> cd ..  
linux> build/pegcalc  

1> 45 * (3 / 15)  

pegcalc: 45 * (3 / 15) = 2.8  

2> exit  



No state other than line counter for prompt is maintained between lines.  
