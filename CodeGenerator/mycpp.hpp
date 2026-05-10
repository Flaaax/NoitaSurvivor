#include<string>
#include<string_view>

enum MyEnum { MyEnum_0, MyEnum_1 };
void function0(int a, int b);
struct [[clang::annotate("my_attr")]] MyStruct { int myData; float yoooo; };
typedef MyStruct* MyStructPtr;
using MyInt = long long;

namespace MyName1 {
	using MYType = long long;
	using MyInt = MYType;
}

struct test1111 {
	std::string name = "heool, wood";
	int this_is_a_int;
	MyInt whatTheType;
	long test_this_type;
	unsigned char this_is_char_but_unsigned;
	char this_is_array[100];
};