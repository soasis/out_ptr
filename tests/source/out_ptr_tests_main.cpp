#define CATCH_CONFIG_RUNNER 1
#include <catch2/catch.hpp>

int main(int argc, char* argv[]) {
	int r = Catch::Session().run(argc, argv);
	return r;
}
