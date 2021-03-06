/*
 * ++C - C++ introduction
 * Copyright (C) 2013, 2014, 2015, 2016 Wilhelm Meier <wilhelm.meier@hs-kl.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

//[ test1
#include <algorithm>
#include <memory> //* Highlight
#include <sstream>
//]
#include <iostream>
//[ test2 -test3 -test4 -test3
#include <algorithm>
//[ test3
#include <memory> // <5> xx
//]
//]

//[ test1
int main1()
{
	bla(42, 42.42, 24.10321); 
    int x = 0; // <> Initialisierung

    int y = 0; //-

	std::string str{"The quick brown fox jumps over the lazy dog"};

    return x; //* Highlight
}
//]

//[ raz -ul
void raz(){
	int x; 
	int y;
	int z;

	//[ ul : Not shown for test purposes
	for(int i = 0; i < 100; ++i){
		std::cout << i << std::endl;
	}
	//]
}
//]

//[ test6 -test5
void bla(int a, double b, double c)
{
}

//[ test5 +*
void foo()
{
	std::stringstream ss;
	ss << "Julius ";
	ss << "kaufe eurer Friedel ";
	ss << "dieses Kleid aus Krefelder Seide"; 
		
	//[for -*
	for(int i = 0; i < 10; i++){
		std::cout << "Number" << std::endl;
		
		//[if +*
		if(i % 2 == 0){
			std::cout << "If --> Even number" << std::endl;
		}
		else{
			std::cout << "If --> Odd number" << std::endl;
		}
		//]
		std::cout << "Not highlighted section again" << std::endl;
	}
	//]

	std::cout << "Highlighted root section" << std::endl;
	std::cout << ss.str() << std::endl;
}
//]

//[baz
void baz(){

	//[baz::for
	for(int i = 0; i < 10; i++){
		std::cout << "Counting: " << i << std::endl;

		if(i == 2){
			std::cout << "We found number two" << std::endl;
		}
	}
	//]
	std::cout << "End of baz function" << std::endl;
}	
//]


//]


