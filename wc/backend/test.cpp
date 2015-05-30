#include <iostream>



class A {
    public:
	template <typename ...T>
	A(T... args) {
	    std::cout << sizeof...(args) <<std::endl;
	}
};


int main()
{
    A(1);
return 0;
}