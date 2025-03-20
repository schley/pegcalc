// I guess it is licensed by Bill Schley 2024
// MIT license ... I heard that was good.
//
// The goal is to have 
// a module that accepts user input and has 
// no additional overhead.
//
module;

#include <iostream>
#include <sstream>

// read, evaluate, print, loop
//
export module Repl;

// Tb or Testbed
//
namespace Tb
{
export class ReplEcho
{
    public:
        ReplEcho();
        ~ReplEcho() = default;

        // main use ... get next line from std::cin
        //
        std::string echoInput();

        // keepEchoing set to false if exit entered
        //
        bool keepEchoing() const;

        // number of "non-exit" commands entered
        //
        int count() const;

        // duplicate convenience access provides last line entered
        //
        std::string echoed() const;

    private:
        int count_;
        bool keepEchoing_;
        const std::string PROMPT;
        const std::string EXIT;
        std::string echoed_;
};  // class ReplEcho

// ctor .. note EXIT constant string to terminate REPL
//
ReplEcho::ReplEcho() : count_(0), keepEchoing_(true), PROMPT("> "), EXIT("exit")
{
}

// get the string from std::cin
//
std::string ReplEcho::echoInput()
{
    std::ostringstream promptstream;
    promptstream << count_ + 1 << PROMPT;
    std::cout << promptstream.str().c_str();
    std::string response;
    std::getline(std::cin, response);
    if (response != EXIT)
    {
        // Keep track of number of lines inputted
        ++count_;
        echoed_ = response;
    }
    else
    {
        keepEchoing_ = false;
    }
    return response;
}

// Loop until user types exit
//
bool ReplEcho::keepEchoing() const 
{
    return keepEchoing_; 
}


int ReplEcho::count() const 
{ 
    return count_; 
}

std::string ReplEcho::echoed() const 
{ 
    return echoed_; 
}

}  // namespace Tb
