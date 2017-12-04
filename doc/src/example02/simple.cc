#include <iostream>

int main()
{
	int x = 0;
	
	//[out
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	//]
	
	x = 42;
	
	//[out
	std::cout << __cplusplus << std::endl;
	//]
	
	return x;
}
