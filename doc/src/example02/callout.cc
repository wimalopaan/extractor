#include <iostream>

//[main
int main()
{
    //[a
    int x = 0; // <> Initialization of _variable_ `x` with value `0`
    //]
	std::cout << __PRETTY_FUNCTION__ << std::endl;
    //[b
	x = 42; // <> Copy-assignment
    //]
	std::cout << __cplusplus << std::endl; 
    //[c
    return x; // <> returning the value to the caller
    //]
}
//]
