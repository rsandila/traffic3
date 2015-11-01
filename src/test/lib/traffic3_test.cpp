#include "logging.h"
INITIALIZE_EASYLOGGINGPP

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main( int argc, char* argv[] )
{
    START_EASYLOGGINGPP(argc, argv);
    
    return Catch::Session().run( argc, argv );
}
