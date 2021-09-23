#include <module/module.test.hpp>
#include <module/module.hpp>

TEST_CASE("Properties of additions", "[module_add]") 
{
    REQUIRE(module_add(0, 0) == 0);

    SECTION("Addition is commutative") 
    {
        REQUIRE(module_add(1, 2) == module_add(2, 1));
    }

    SECTION("Addition is associative") 
    {
        REQUIRE(module_add(1, module_add(2, 3)) == module_add(3, module_add(2, 1)));
    }
}
