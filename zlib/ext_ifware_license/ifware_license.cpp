/*
ifware_license

This module exposes one method to Lua, ifware_license.getLicense,
which returns Iguana and Chameleon licenses generated from scratch
using the DRG and RGN libraries, on the basis of license details
passed in as parameters.

See also the usage info on the wiki for the Licensing Module: http://fogbugz.ifware.dynip.com/default.asp?W5170
*/

#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>
#include <COL/COLdateTime.h>
#include <COL/COLerror.h>

#include <DRG/DRGserialization.h>
#include <DRG/DRGinformation.h>

#include <RGN/RGNmachineId.h>

#include "math.h"

#include <LUA/LUAheader.h>

#define COUNT_OF_DAY_PER_MONTH 31

#define PROJECT_TABLENAME "ifware_license"

#undef LUA_API    // Quiet CPP warnings about redefinitions.
#if defined _WIN32 || defined _WIN32
#define LUA_API __declspec(dllexport)
#else
#define LUA_API
#endif

//Load a parameter from a table that is the first Lua method argument.
static int loadParam (lua_State* L, const char* ParamName, const int ParamType, bool Required = false, bool CheckType = true) {
   lua_pushstring(L, ParamName);
   lua_gettable(L, 1);

   if (lua_isnil(L, -1)) {
      if (Required) {
         return luaL_error(L, "Required parameter missing: '%s'.", ParamName);
      } else {
         return 0;
      }
   }
   bool TypeMatch = true;
   switch(ParamType) {      
      case LUA_TNUMBER:
         if (!lua_isnumber(L, -1)) {
            TypeMatch = false;
         }
         break;
      case LUA_TSTRING:
         if (!lua_isstring(L, -1)) {
            TypeMatch = false;
         }
         break;
      default:
         if (lua_type(L, -1) != ParamType) {
            TypeMatch = false;
         }         
   }
   
   if (CheckType && !TypeMatch) {
      return luaL_error(L, "Expected parameter '%s' of type '%s'.",
         ParamName, lua_typename(L, ParamType));
   }

   return 1;
}

static COLstring getStringParam(lua_State* L, const char* ParamName, bool Required=false, COLstring DefaultValue="", bool CheckType = true) {   
   if (loadParam(L, ParamName, LUA_TSTRING, Required, CheckType)) {
      COLstring Param = lua_tostring(L, -1);
      lua_pop(L,1);
      return Param;
   } else {
      return DefaultValue;
   }
}

static int getIntegerParam(lua_State* L, const char* ParamName, bool Required=false, int DefaultValue=-1, bool CheckType = true) {   
   if (loadParam(L, ParamName, LUA_TNUMBER, Required, CheckType)) {
      int Param = int(lua_tointeger(L, -1));
      lua_pop(L,1);
      return Param;
   } else {
      return DefaultValue;
   }
}

//Round MaintenanceExpiryDate to an even number of "months" from the BaseDate.
static int roundedMonthsMaintenance(lua_State* L, COLdateTime MaintenanceExpiryDate, COLdateTime BaseDate, COLstring Rounding) {
   COLdateTimeSpan Difference = MaintenanceExpiryDate - BaseDate;
   int NewMonthsDifference;
   //The tiny amount added/subtracted is to prevent issues due to floating point precision in Difference
   //when Difference should be exactly a whole number of days. E.g. 31.0000000001 should round to 1 month, not 2.
   if (Rounding == "up") {                  
      NewMonthsDifference = int(ceil((Difference.totalDays() - 0.000001)/double(COUNT_OF_DAY_PER_MONTH)));
   } else if (Rounding == "down") {
      NewMonthsDifference = int(floor((Difference.totalDays() + 0.000001)/double(COUNT_OF_DAY_PER_MONTH)));
   } else {
      luaL_error(L, "If provided, maintenance_rounding must be either 'up' or 'down'.");
   }
   return NewMonthsDifference;
}

static int getIguanaLicense (lua_State* L) {
   COLstring IguanaId = getStringParam(L, "instance_id", true);
   
   int CountOfChannels;
   if (getStringParam(L, "num_channels", true, "", false) == "unlimited") {
      CountOfChannels = 0;
   } else {
      CountOfChannels = getIntegerParam(L, "num_channels", true);
      if (CountOfChannels < 1) {
         luaL_error(L, "num_channels must be either a positive integer or 'unlimited'.");
      }
   }

   DRGinformation LicenseInformation;
   LicenseInformation.onInitialized();
   LicenseInformation.setMaxCountOfConnection(CountOfChannels);

   COLdateTime StartDate;
   COLstring OverrideDate = getStringParam(L, "date_stamp", false, "", true);   
   if (OverrideDate.is_null()) {
      StartDate = COLdateTime::currentTime();
      //Already set in LicenseInformation by onInitialized().
   } else {
      try {
         StartDate.fromString(OverrideDate);
      } catch(COLerror& Error) {
         luaL_error(L, Error.description().c_str());
      }
      LicenseInformation.setLicenseTimeStamp(StartDate);
   }

   COLstring ExpiryType = getStringParam(L, "expiry_type", true);   

   bool NoExpiry = false, NoMaintenanceExpiry = false;
   int LicenseExpiry;

   if (ExpiryType == "months") {
      if (getStringParam(L, "license_expiry", true, "", false) == "none") {
         NoExpiry = true;
         LicenseInformation.setCountOfMonthToLicenseExpiry(0);
      } else {
         LicenseExpiry = getIntegerParam(L, "license_expiry", true);
         if (LicenseExpiry < 1) {
            luaL_error(L, "license_expiry must be either a positive integer or 'none'.");
         }
         LicenseInformation.setCountOfMonthToLicenseExpiry(LicenseExpiry);
      }
      if (getStringParam(L, "maintenance_expiry", true, "", false) == "none") {
         NoMaintenanceExpiry = true;
         LicenseInformation.setCountOfMonthToMaintenanceExpiry(0);
      } else if (getStringParam(L, "maintenance_rounding", false, "", false) != "") { //I.e. getting the maintenance as a date.    
         COLstring MaintenanceExpiryString = getStringParam(L, "maintenance_expiry", true, "", false);
         COLdateTime MaintenanceExpiryDate;
         MaintenanceExpiryDate.fromString(MaintenanceExpiryString);
                  
         //Since maintenance expiry is specified as a date, it will need rounding to work with the license timestamp.
         COLstring Rounding = getStringParam(L, "maintenance_rounding", true, "none", true);
         int NewMonthsMaintenance = roundedMonthsMaintenance(L,MaintenanceExpiryDate,StartDate,Rounding);

         if (NewMonthsMaintenance < 1) {
            //Need to move the license time stamp back to make this maintenance date be at least 1.
            int MonthsAdjustment = 1 - NewMonthsMaintenance;            
            StartDate = StartDate - COLdateTimeSpan(COUNT_OF_DAY_PER_MONTH * MonthsAdjustment);
            LicenseInformation.setLicenseTimeStamp(StartDate);

            if (!NoExpiry) {
               LicenseExpiry += MonthsAdjustment;
               LicenseInformation.setCountOfMonthToLicenseExpiry(LicenseExpiry);               
            }
            
            NewMonthsMaintenance += MonthsAdjustment;
         }
         LicenseInformation.setCountOfMonthToMaintenanceExpiry(NewMonthsMaintenance);         
      } else {
         int MaintenanceExpiry = getIntegerParam(L, "maintenance_expiry", true);
         if (MaintenanceExpiry < 1) {
            luaL_error(L, "maintenance_expiry must be either a positive integer or 'none'.");
         }         
         LicenseInformation.setCountOfMonthToMaintenanceExpiry(MaintenanceExpiry);       
      }
   } else if (ExpiryType == "days") {
      int LicenseDays = getIntegerParam(L, "license_expiry", true);
      if (LicenseDays < 0) {
         luaL_error(L, "license_expiry must be a non-negative integer.");
      }
      int MaintenanceDays = getIntegerParam(L, "maintenance_expiry", true);
      if (MaintenanceDays < 0) {
         luaL_error(L, "maintenance_expiry must be a non-negative integer.");
      }
   
      if ((MaintenanceDays - LicenseDays)%COUNT_OF_DAY_PER_MONTH != 0) {
         return luaL_error(L, "The difference between expiry and maintenance must be divisible by %d.", COUNT_OF_DAY_PER_MONTH);
      }

      //Find the next whole "month" after the desired number of days, and
      //pretend the license was generated earlier to make it an even multiple of months.
      int DaysToFullMonth = COUNT_OF_DAY_PER_MONTH - LicenseDays%COUNT_OF_DAY_PER_MONTH;
      LicenseInformation.setLicenseTimeStamp(
         StartDate - COLdateTimeSpan(DaysToFullMonth)
         );
      LicenseInformation.setCountOfMonthToLicenseExpiry(
         (LicenseDays + DaysToFullMonth) / COUNT_OF_DAY_PER_MONTH
         );
      LicenseInformation.setCountOfMonthToMaintenanceExpiry(
         (MaintenanceDays + DaysToFullMonth) / COUNT_OF_DAY_PER_MONTH
         );
   } else if (ExpiryType == "date") {      
      COLstring LicenseExpiryString = getStringParam(L, "license_expiry", true, "", true);
      COLstring MaintenanceExpiryString = getStringParam(L, "maintenance_expiry", true, "", true);
      COLdateTime LicenseExpiryDate, MaintenanceExpiryDate, NewStartDate;
      try { //Catching date formatting errors.
         if (LicenseExpiryString == "none") {
            NoExpiry = true;
         } else {
            LicenseExpiryDate.fromString(LicenseExpiryString);
            NewStartDate = LicenseExpiryDate - COLdateTimeSpan(COUNT_OF_DAY_PER_MONTH);
         }
         if (MaintenanceExpiryString == "none") {
            NoMaintenanceExpiry = true;
         } else {
            MaintenanceExpiryDate.fromString(MaintenanceExpiryString);

            //Round 'maintenance_expiry' date to make it work with the specified license date.
            COLstring Rounding = getStringParam(L, "maintenance_rounding", false, "none", true);
            if (Rounding != "none" && !LicenseExpiryDate.isNull()) { //No need to round if there's no license expiry.               
               MaintenanceExpiryDate = LicenseExpiryDate +
                  COLdateTimeSpan(COUNT_OF_DAY_PER_MONTH * roundedMonthsMaintenance(L,MaintenanceExpiryDate,LicenseExpiryDate,Rounding));
            }

            if (NewStartDate.isNull() || MaintenanceExpiryDate < LicenseExpiryDate) {
               NewStartDate = MaintenanceExpiryDate - COLdateTimeSpan(COUNT_OF_DAY_PER_MONTH);
            }
         }
      } catch(COLerror& Error) {
         luaL_error(L, Error.description().c_str());
      }
    
      //Note that NewStartDate is a month earlier than either license or maintenance expiry,
      //as the minimum number of months for the license is 1. (0 is unlimited.)  
      if (!NewStartDate.isNull()) {
         LicenseInformation.setLicenseTimeStamp(NewStartDate);
      }
        
      if (NoExpiry) {
         LicenseInformation.setCountOfMonthToLicenseExpiry(0);
      } else {
         //Adding a fraction of a second so we don't get floating point precision errors in rounding down.
         COLdateTimeSpan Difference = LicenseExpiryDate - NewStartDate + COLdateTimeSpan(0.000001);         
         if (Difference.days()%COUNT_OF_DAY_PER_MONTH != 0) {
            return luaL_error(L, "The number of days between expiry and maintenance must be divisible by %d.", COUNT_OF_DAY_PER_MONTH);
         } else if (Difference.totalDays() - Difference.days() > 0.00001) { //I.e. difference is not exactly the number of days.
            return luaL_error(L, "Time of day must match up between expiry and maintenance.");
         }        
         LicenseInformation.setCountOfMonthToLicenseExpiry(
            int(Difference.days() / COUNT_OF_DAY_PER_MONTH)
         );
      }
      if (NoMaintenanceExpiry) {
         LicenseInformation.setCountOfMonthToMaintenanceExpiry(0);
      } else {
         //Adding a fraction of a second so we don't get floating point precision errors in rounding down.
         COLdateTimeSpan Difference = MaintenanceExpiryDate - NewStartDate + COLdateTimeSpan(0.000001);         
         if (Difference.days()%COUNT_OF_DAY_PER_MONTH != 0) {
            return luaL_error(L, "The number of days between expiry and maintenance must be divisible by %d.", COUNT_OF_DAY_PER_MONTH);
         } else if (Difference.totalDays() - Difference.days() > 0.00001) { //I.e. difference is not exactly the number of days.
            return luaL_error(L, "Time of day must match up between expiry and maintenance.");
         }
         LicenseInformation.setCountOfMonthToMaintenanceExpiry(
            int(Difference.days() / COUNT_OF_DAY_PER_MONTH)
            );
      }
   } else {
      return luaL_error(L, "expiry_type must be 'months', 'days', or 'date'.");
   }
   
   COLstring LicenseString;
   DRGiguanaLicenseToString(&LicenseString, LicenseInformation, IguanaId);      
   
   lua_newtable(L); //license info to be returned

   lua_pushstring(L, LicenseString.c_str());
   lua_setfield(L, -2, "Code");

   lua_pushstring(L, LicenseInformation.licenseTimeStamp().format("%Y-%m-%d %H:%M:%S").c_str());
   lua_setfield(L, -2, "LicenseTimeStamp");

   if (NoExpiry) {
      lua_pushstring(L, "none");      
   } else {
      lua_pushstring(L, LicenseInformation.licenseExpiryDate().format("%Y-%m-%d %H:%M:%S").c_str());
   }
   lua_setfield(L, -2, "LicenseExpiry");

   if (NoMaintenanceExpiry) {
      lua_pushstring(L, "none");
   } else {      
      lua_pushstring(L, LicenseInformation.maintenanceExpiryDate().format("%Y-%m-%d %H:%M:%S").c_str());
   }
   lua_setfield(L, -2, "MaintenanceExpiry");
   
   if (LicenseInformation.maxCountOfConnection() == 0) {
      lua_pushstring(L, "unlimited");
   } else {
      lua_pushinteger(L, LicenseInformation.maxCountOfConnection());
   }
   lua_setfield(L, -2, "NumChannels");

   return 1;
}

static int getChameleonLicense (lua_State* L) {
   COLstring MachineId = getStringParam(L, "instance_id", true);

   COLdateTime ExpiryDateTime;

   bool NoExpiry = false;
   if (getStringParam(L, "license_expiry", true, "", false) == "none") {
      NoExpiry = true;
   } else {
      COLstring ExpiryType = getStringParam(L, "expiry_type", true);   

      int DaysToExpiry = 0;
      if (ExpiryType == "months") {
         DaysToExpiry = COUNT_OF_DAY_PER_MONTH * getIntegerParam(L, "license_expiry", true);
      } else if (ExpiryType == "days") {
         DaysToExpiry = getIntegerParam(L, "license_expiry", true);
      } else {
         return luaL_error(L, "expiry_type must be 'months' or 'days'.");
      }

      COLdateTime StartDate;
      COLstring OverrideDate = getStringParam(L, "date_stamp", false, "", true);   
      if (OverrideDate.is_null()) {
         StartDate = COLdateTime::currentTime();         
      } else {
         try {
            StartDate.fromString(OverrideDate);
         } catch(COLerror& Error) {
            luaL_error(L, Error.description().c_str());
         }         
      }

      ExpiryDateTime = StartDate + COLdateTimeSpan(DaysToExpiry);
   }

   COLstring LicenseString;
   if (NoExpiry) {
      LicenseString = RGNextensionKey(MachineId, "", true);
   } else {
      LicenseString = RGNextensionKey(MachineId, ExpiryDateTime.format("%Y%m%d"), false);
   }

   lua_newtable(L); //license info to be returned

   lua_pushstring(L, LicenseString.c_str());
   lua_setfield(L, -2, "Code");

   if (NoExpiry) {
      lua_pushstring(L, "none");      
   } else {
      lua_pushstring(L, ExpiryDateTime.format("%Y-%m-%d 00:00:00").c_str());
   }
   lua_setfield(L,-2,"LicenseExpiry");

   return 1;
}

//Generates a registration code, and returns a table with license information.
static int getLicense (lua_State* L) {
   if (!lua_istable(L, 1) || !lua_isnoneornil(L, 2)){            
      return luaL_error(L, "A parameter table is required as the only parameter.");
   }
   
   COLstring Product = getStringParam(L, "product", true);

   if (Product == "Iguana") {
      return getIguanaLicense(L); 
   } else if (Product == "Chameleon") {
      return getChameleonLicense(L); 
   } else {
      return luaL_error(L, "Product must be 'Iguana' or 'Chameleon'.");
   }   
}

extern "C" LUA_API int luaopen_ifware_license (lua_State* L) {
   struct luaL_reg driver[] = {
      {"getLicense", getLicense},
      {NULL, NULL},
   };
   luaL_register(L, PROJECT_TABLENAME, driver);
   return 1;
}
