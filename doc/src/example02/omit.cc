#include <iostream>

//[main
int main()
{
	int x = 0;

	std::cout << __PRETTY_FUNCTION__ << std::endl; //-
	x = 42;
	std::cout << __cplusplus << std::endl; //-

	return x;
}
//]