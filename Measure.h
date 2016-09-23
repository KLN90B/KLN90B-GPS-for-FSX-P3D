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
			const double Pi							= 3.1415926535897932384626433832795;
			const double TicksPerSecond				= 18;
			const double FsLatFactor				= 111130.555557;
			const double FsLonFactor				= 781874935307.40;
			const double LatDivider					= 40007000.0;
			const double LonDivider					= 281474976710656.0;
			const double FuelLevelPercent			= 83886.08;
			const double RadiansDegreeFactor		= 180.0/Pi;
			const double DegreeRadiansFactor		= Pi/180.0;
			const double MeterFeetFactor			= 3.28084;
			const double KilometerNmMileFactor		= 0.54;
			const double PoundKilogramFactor		= 0.453592;
			const double MeterPerSecondKnotFactor	= 1.944;
			const double GallonLitreFactor			= 3.785;
			const double InchHgPsiFactor			= 0.4912;
			const double LbsKilogramFactor			= 2.2046;
			const double PsiPsfFactor				= 144.0;
			const double RadiusEarth				= 6378.137; // km
			const double OneDegree					= 1.0/DegreeRadiansFactor;

			inline double AngleAlign(double a)						{ if(a<0.0) a+=2.0*Pi; return a; }

			inline double DegreeToRadians(double angle)				{ return angle*DegreeRadiansFactor; }
			inline double RadiansToDegree(double angle)				{ return angle*RadiansDegreeFactor; }

			inline double FeetToMeter(double val)					{ return val/MeterFeetFactor; }
			inline double MeterToFeet(double val)					{ return val*MeterFeetFactor; }

			inline double NauticMileToMeter(double val)				{ return val*(1000.0/KilometerNmMileFactor); } 
			inline double MeterToNauticMile(double val)				{ return val*(KilometerNmMileFactor/1000.0); }

			inline double PoundToKilogram(double pound)				{ return pound*PoundKilogramFactor; }
			inline double KilogramToPound(double kilogram)			{ return kilogram/PoundKilogramFactor; }

			inline double PoundToMetricTon(double pound)			{ return PoundKilogramFactor*pound/1000.0; } 
			inline double MetricTonToPound(double ton)				{ return (ton*1000.0)/PoundKilogramFactor; }

			inline double MeterPerSecondToKnot(double val)			{ return val*MeterPerSecondKnotFactor; }
			inline double KnotToMeterPerSecond(double val)			{ return val/MeterPerSecondKnotFactor; }

			inline double LitreToGallon(double litre)				{ return litre/GallonLitreFactor; }
			inline double GallonToLitre(double gallon)				{ return gallon*GallonLitreFactor; }

			inline double PsiToPsf(double press)					{ return PsiPsfFactor*press; }
			inline double PsfToPsi(double press)					{ return press/PsiPsfFactor; }

			inline double InchHgToPsi(double press)					{ return press*InchHgPsiFactor; }
			inline double PsiToInchHg(double press)					{ return press/InchHgPsiFactor; }

			inline double InchHgToPsf(double press)					{ return press*InchHgPsiFactor*PsiPsfFactor; }
			inline double PsfToInchHg(double press)					{ return press/InchHgPsiFactor/PsiPsfFactor; }

			inline double FahrenheitToCelsius(double fahrenheit)	{ return 5.0/9.0*(fahrenheit-32.0); }
			inline double CelsiusToFahrenheit(double celsius)		{ return 32.0+9.0/5.0*celsius; }

			inline double CelsiusToKelvin(double celsius)			{ return celsius+273.15; }
			inline double KelvinToCelsius(double kelvin)			{ return kelvin-273.15; }

			inline double RankineToKelvin(double rankine)			{ return 5.0/9.0*rankine; }
			inline double KelvinToRankine(double kelvin)			{ return 9.0/5.0*kelvin; }

			inline double RankineToFahrenheit(double rankine)		{ return rankine-459.67; }
			inline double FahrenheitToRankine(double fahrenheit)	{ return fahrenheit+459.67; }

			inline double CelsiusToRankine(double celsius)			{ return FahrenheitToRankine(CelsiusToFahrenheit(celsius)); } 
			inline double RankineToCelsius(double rankine)			{ return FahrenheitToCelsius(RankineToFahrenheit(rankine)); }

			inline double FsLatitudeDegrees(double val)				{ return val/FsLatFactor; }	//FS latitude conversion to degrees latitude (north positive,south negative)
			inline double FsLongitudeDegrees(double val)			{ return (val>140737488355332)?val/FsLonFactor-360.0:val/FsLonFactor; }	//FS longitude conversion to degrees longitude (east positive,west negative)

			inline double FsVerticalSpeedFeet(double val)			{ return MeterToFeet(val/256.0)*60.0; } //FS vertical speed in feet/min from 1/256 m/sec

			inline double FsGroundAltFeet(double val)				{ return MeterToFeet(val/256.0); } //ground altittude in feet
			inline double FsPlaneAltFeet(double val)				{ return MeterToFeet(val); } //plane altitude in feet
			inline double FsMach(double val)						{ return val*3.2/65536.0; }

			inline double DegreesSin(double val)					{ return sin(val/RadiansDegreeFactor); }			//sinus in degrees
			inline double DegreesCos(double val)					{ return cos(val/RadiansDegreeFactor); }			//cosine in degrees
			inline double DegreesTan(double val)					{ return tan(val/RadiansDegreeFactor); }			//tangens in degrees
			inline double DegreesAsin(double val)					{ return RadiansDegreeFactor*asin(val); }			//arcsinus in degrees
			inline double DegreesAcos(double val)					{ return RadiansDegreeFactor*acos(val); }			//arccosine in degrees
			inline double DegreesAtan(double val)					{ return RadiansDegreeFactor*atan(val); }			//arctangens in degrees
			inline double DegreesAtan2(double val1,double val2)		{ return RadiansDegreeFactor*atan2(val1,val2); }	//atan2 in degrees

			inline double LbsToKg(double val)						{ return PoundToKilogram(val); }
			inline double KgToLbs(double val)						{ return KilogramToPound(val); }
		}
	}
}

