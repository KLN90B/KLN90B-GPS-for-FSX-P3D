/*
    This file is part of swan2 project.

    Copyright (C) 2013 by Nick Sharmanzhinov 
    except134@gmail.com
*/

#pragma once

namespace swan2
{
	namespace lib
	{
		namespace fsx
		{
			class XMLDimUnits 
			{
			public:
				static void Init();

				static ENUM boolean			;
				static ENUM celsius			;
				static ENUM degrees			;
				static ENUM feet			;
				static ENUM feetPerSecond	;
				static ENUM kg				;
				static ENUM kgPerSecond		;
				static ENUM km				;
				static ENUM kmPerHour		;
				static ENUM knots			;
				static ENUM meters			;
				static ENUM meterPerSecond	;
				static ENUM number			;
				static ENUM percent			;
				static ENUM radians			;
				static ENUM fahrenheit		;

			protected:
				static bool enumsInitialized;
			};

			class XMLNamedVar
			{
			protected:
				ID   varID;
				bool unregisterAfterUse;

			public:
				XMLNamedVar(const char* name,bool unregister_after_use=false);
				XMLNamedVar(const std::string& name,bool unregister_after_use=false);
				~XMLNamedVar();

				bool   GetBoolean		() { return (unsigned)get_named_variable_typed_value( varID, XMLDimUnits::number          ) != 0; };
				double GetCelsius		() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::celsius         );      };
				double GetDegrees		() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::degrees         );      };
				double GetFeetPerSec	() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::feetPerSecond	);      };
				double GetKm			() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::km              );      };
				double GetKmPerHour		() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::kmPerHour		);      };
				double GetKnots			() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::knots           );      };
				double GetMeters		() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::meters          );      };
				double GetMeterPerSec	() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::meterPerSecond	);      };
				double GetNumber		() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::number          );      };
				double GetPercent		() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::percent         );      };
				double GetRadians		() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::radians			);      };
				double GetFahrenheit	() { return (double)  get_named_variable_typed_value( varID, XMLDimUnits::fahrenheit		);      };

				void SetBoolean			(bool     value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::number				); };
				void SetCelsius			(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::celsius			); };
				void SetDegrees			(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::degrees			); };
				void SetFeetPerSec		(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::feetPerSecond		); };
				void SetKg				(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::kg					); };
				void SetKgPerSec		(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::kgPerSecond		); };
				void SetKm				(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::km					); };
				void SetKmPerHour		(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::kmPerHour			); };
				void SetMeterPerSec		(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::meterPerSecond		); };
				void SetKnots			(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::kmPerHour			); };
				void SetMeters			(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::knots				); };
				void SetNumber			(int      value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::number				); };
				void SetNumber			(unsigned value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::number				); };
				void SetNumber			(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::number				); };
				void SetPercent			(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::percent			); };
				void SetRadians			(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::radians			); };
				void SetFahrenheit		(double   value) { set_named_variable_typed_value( varID, (FLOAT64)value, XMLDimUnits::fahrenheit			); };
			};

			class SimVar
			{
			protected:
				GAUGE_TOKEN varID;
				MODULE_VAR	token;

			public:
				SimVar(GAUGE_TOKEN id);

				inline double GetN()	{ lookup_var(&token); return token.var_value.n;	}
				inline ENUM   GetE()	{ lookup_var(&token); return token.var_value.e;	}
				inline FLAGS  GetF()	{ lookup_var(&token); return token.var_value.f;	}
				inline VAR32  GetD()	{ lookup_var(&token); return token.var_value.d;	}
				inline BOOL   GetB()	{ lookup_var(&token); return token.var_value.b;	}
				inline VAR32  GetO()	{ lookup_var(&token); return token.var_value.o;	}
				inline PVOID  GetP()	{ lookup_var(&token); return token.var_value.p;	}

				bool   GetBoolean		() { return (unsigned)GetN() != 0; };
				double GetDegrees		() { return (double)  GetN();      };
				double GetFeet			() { return (double)  GetN();      };
				double GetFeetPerSec	() { return (double)  GetN();      };
				double GetKg			() { return (double)  GetN();      };
				double GetKgPerSec		() { return (double)  GetN();      };
				double GetKm			() { return (double)  GetN();      };
				double GetKmPerHour		() { return (double)  GetN();      };
				double GetKnots			() { return (double)  GetN();      };
				double GetMeters		() { return (double)  GetN();      };
				double GetMeterPerSec	() { return (double)  GetN();      };
				double GetNumber		() { return (double)  GetN();      };
				double GetRadians		() { return (double)  GetN();      };
				double GetPercent		() { return (double)  GetN();      };
				double GetCelsius		() { return (double)  GetN();      };
				double GetFahrenheit	() { return (double)  CelsiusToFahrenheit(GetCelsius()); };

				double Get(ENUM e) { 
					if(e==XMLDimUnits::boolean		) return GetBoolean();
					if(e==XMLDimUnits::celsius		) return GetCelsius();		
					if(e==XMLDimUnits::degrees		) return GetDegrees();		
					if(e==XMLDimUnits::feet			) return GetFeet();			
					if(e==XMLDimUnits::feetPerSecond) return GetFeetPerSec();	
					if(e==XMLDimUnits::kg			) return GetKg();			
					if(e==XMLDimUnits::kgPerSecond	) return GetKgPerSec();		
					if(e==XMLDimUnits::km			) return GetKm();			
					if(e==XMLDimUnits::kmPerHour	) return GetKmPerHour();		
					if(e==XMLDimUnits::knots		) return GetKnots();			
					if(e==XMLDimUnits::meters		) return GetMeters();		
					if(e==XMLDimUnits::meterPerSecond)return GetMeterPerSec();	
					if(e==XMLDimUnits::number		) return GetNumber();		
					if(e==XMLDimUnits::percent		) return GetPercent();		
					if(e==XMLDimUnits::radians		) return GetRadians();		
					if(e==XMLDimUnits::fahrenheit	) return GetFahrenheit();		
					return 0;
				};
			};

			class SimVars
			{
			private:
				bool						stdTokensInited;
				std::auto_ptr<SimVar>		stdTokens[C_GAUGE_TOKEN];

			public:
				SimVars() {
					stdTokensInited=false;
					InitVars();
				}

				void InitVars() {
					if(stdTokensInited)
						return;

					for(int i=MODULE_VAR_NONE+1;i<C_GAUGE_TOKEN;i++) {
						stdTokens[i]=std::auto_ptr<SimVar>(new SimVar((GAUGE_TOKEN)i));
					}

					stdTokensInited=true;
				}

				inline double GetC(GAUGE_TOKEN var,ENUM e=XMLDimUnits::number)	{ return stdTokens[var]->Get(e);		}
				inline FLAGS  GetCF(GAUGE_TOKEN var)							{ return stdTokens[var]->GetF();		}
				inline PVOID  GetCP(GAUGE_TOKEN var)							{ return stdTokens[var]->GetP();		}
			};
		}
	}
}



