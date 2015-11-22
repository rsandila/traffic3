/*
 * Copyright (C) 2015 Robert Sandilands 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 USA.
 */
#include <fstream>
#include "logging.h"
INITIALIZE_EASYLOGGINGPP

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

static const char * TRAFFIC_TEST_CONF = "traffic3_test.logging.conf";

int main( int argc, char* argv[] )
{
    START_EASYLOGGINGPP(argc, argv);
	{
		std::ifstream test(TRAFFIC_TEST_CONF);
		if (test.is_open()) {
			el::Configurations conf(TRAFFIC_TEST_CONF);
			el::Loggers::reconfigureAllLoggers(conf);
		}
	}

    return Catch::Session().run( argc, argv );
}
