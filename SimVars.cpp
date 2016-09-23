/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/
#include "PCH.h"

using namespace swan2;
using namespace swan2::lib;
using namespace swan2::lib::fsx;

bool XMLDimUnits::enumsInitialized = false;

ENUM XMLDimUnits::boolean			= 0;
ENUM XMLDimUnits::celsius			= 0;
ENUM XMLDimUnits::degrees			= 0;
ENUM XMLDimUnits::feet           	= 0;
ENUM XMLDimUnits::feetPerSecond		= 0;
ENUM XMLDimUnits::kg					= 0;
ENUM XMLDimUnits::kgPerSecond		= 0;
ENUM XMLDimUnits::km					= 0;
ENUM XMLDimUnits::kmPerHour			= 0;
ENUM XMLDimUnits::knots				= 0;
ENUM XMLDimUnits::meters				= 0;
ENUM XMLDimUnits::meterPerSecond		= 0;	
ENUM XMLDimUnits::number				= 0;
ENUM XMLDimUnits::percent			= 0;
ENUM XMLDimUnits::radians			= 0;
ENUM XMLDimUnits::fahrenheit			= 0;

void XMLDimUnits::Init()
{
	if( enumsInitialized ) 
		return;

	boolean			= get_units_enum( "bool"                );
	celsius			= get_units_enum( "celsius"             );
	degrees			= get_units_enum( "degrees"             );
	feet           	= get_units_enum( "feet"                ); 
	feetPerSecond	= get_units_enum( "feet per second"     ); 
	kg				= get_units_enum( "kilogram"            );
	kgPerSecond		= get_units_enum( "kilogram per second" );
	km				= get_units_enum( "kilometer"           );
	kmPerHour		= get_units_enum( "kilometer per hour"  );
	knots			= get_units_enum( "knots"               );
	meters			= get_units_enum( "meters"              );
	meterPerSecond	= get_units_enum( "meters per second"   );
	number			= get_units_enum( "number"              );
	percent			= get_units_enum( "percent"             );
	radians			= get_units_enum( "radian"              );
	fahrenheit		= get_units_enum( "fahrenheit"          );

	enumsInitialized = true;
}

XMLNamedVar::XMLNamedVar(const char* name, bool unregister_after_use) : unregisterAfterUse( unregister_after_use )
{
	if((varID=check_named_variable(name))==-1) 
		varID=register_named_variable(name);

	XMLDimUnits::Init();
}

XMLNamedVar::XMLNamedVar(const std::string& name, bool unregister_after_use) : unregisterAfterUse( unregister_after_use )
{
	if((varID=check_named_variable(name.c_str()))==-1) 
		varID=register_named_variable(name.c_str());

	XMLDimUnits::Init();
}

XMLNamedVar::~XMLNamedVar(void)
{
	if(unregisterAfterUse&&varID!=-1) 
		unregister_var_by_name((PSTRINGZ)get_name_of_named_variable(varID));
}

SimVar::SimVar(GAUGE_TOKEN id) : varID(id)
{ 
	XMLDimUnits::Init(); 

	token.id=(GAUGE_TOKEN)varID;
	initialize_var(&token);
}

