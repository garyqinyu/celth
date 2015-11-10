/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// ltoa.cpp - utility function to convert long data value to a string.
//
// Author: Kenneth G. Maxwell
//         Jim DeLisle
//
// Copyright (c) 1997-2002 Swell Software 
//              All Rights Reserved.
//
// Unauthorized redistribution of this source code, in whole or part,
// without the express written permission of Swell Software
// is strictly prohibited.
//
// Notes:
// This function is a modified form of the same function found in several
// commercial compiler libraries. We have reduced and simplified the
// original implementations to create our own version, using a mix of
// other implementations, primarily the GNU version, as a base.
// 
// If PEG_UNICODE is defined, a second, overloaded _ltoa is defined
// to allow for plain char* usage.
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#include "peg.hpp"

#ifdef USE_PEG_LTOA

PEGCHAR *_ltoa(long val, PEGCHAR *s, int rad)
{
   int sign;		    
   unsigned long v; 	
   PEGCHAR *build; 		   
   PEGCHAR *cRev;
   PEGCHAR hold;
   int q; 		        

   v=val; 			

   sign=0; 			
   if (rad==10) 	
   if (val<0)
   { 		
      sign=-1; 			
      v=-val; 			
   }

   build=s; 			

   do
   { 			
      q=v%rad; 			
      v=v/rad; 			
      if (q>9)
      	  q=q+'a'-10; 		
      else
          q=q|'0'; 		
      *build++ = q; 		
      
   } while (v!=0); 		


   if (sign!=0) *build++='-'; 	

   *build=0; 			
   build--;
   cRev = s;

   /* reverse the string: */

   while(cRev < build)
   {
        hold = *build;
        *build-- = *cRev;
        *cRev++ = hold;
   }
   return(s);
}

// If Unicode is turned on, include this version of ltoa as well,
// except when using the Microsoft compiler
#if defined(PEG_UNICODE) && !defined(_MSC_VER)

char* _ltoa(long val, char* s, int rad)
{
   int sign;		    
   unsigned long v; 	
   char *build; 		   
   char *cRev;
   char hold;
   int q; 		        

   v=val; 			

   sign=0; 			
   if (rad==10) 	
   if (val<0)
   { 		
      sign=-1; 			
      v=-val; 			
   }

   build=s; 			

   do
   { 			
      q=v%rad; 			
      v=v/rad; 			
      if (q>9)
      	  q=q+'a'-10; 		
      else
          q=q|'0'; 		
      *build++ = q; 		
      
   } while (v!=0); 		


   if (sign!=0) *build++='-'; 	

   *build=0; 			
   build--;
   cRev = s;

   /* reverse the string: */

   while(cRev < build)
   {
        hold = *build;
        *build-- = *cRev;
        *cRev++ = hold;
   }
   return(s);
}

#endif  // PEG_UNICODE

#endif  // USE_PEG_LTOA



