#ifndef _GBEMG_REGMACRODEF_H_
#define _GBEMG_REGMACRODEF_H_


/* 
copyright reserved by 
celtech company Co Ltd,
*/
 


#ifdef __cplusplus
extern "C" {
#endif



#define GBEMG_REG_SECTION_TABLE_ID_OFFSET (0)
#define GBEMG_REG_SECTION_LENGTH_OFFSET (1)

#define GBEMG_REG_VERSION_NUM_OFFSET (5)
#define GBEMG_REG_SECTION_NUM_OFFSET (6)
#define GBEMG_REG_LAST_SECTION_NUM_OFFSET (7)
#define GBEMG_REG_SIGNATURE_LEN_OFFSET (8)

#define GBEMG_REG_CUSTOMER_NUM_OFFSET (10)


#define GBEMG_REG_TABLE_HEADER_LENGTH (12)




#define GBEMG_REG_SECTION_TABLE(buf) (U8)((buf[GBEMG_REG_SECTION_TABLE_ID_OFFSET]&0xFF))
#define GBEMG_REG_SECTION_LENGTH(buf) (TS_READ_16(&buf[GBEMG_REG_SECTION_LENGTH_OFFSET])&0xFFF)


#define GBEMG_REG_VERSION_NUM(buf)   (U8)((buf[GBEMG_REG_VERSION_NUM_OFFSET]>>1)&0x1F)

#define GBEMG_REG_SECTION_NUM(buf)   (U8)(buf[GBEMG_REG_SECTION_NUM_OFFSET]&0xFF)


#define GBEMG_REG_LAST_SECTION_NUM(buf) (U8)(buf[GBEMG_REG_LAST_SECTION_NUM_OFFSET]&0xFF)

#define GBEMG_REG_SIGNATURE_LEN(buf) (TS_READ_16(buf[GBEMG_REG_SIGNATURE_LEN_OFFSET])&0xFFFF)

#define GBEMG_REG_CUSTOMER_NUM(buf) (TS_READ_16(buf[GBEMG_REG_CUSTOMER_NUM_OFFSET])&0xFFFF)


/*the following macro based on the begining of the loop of the customer register information  */

#define GBEMG_REG_CUSTOMER_BOX_NUM_OFFSET (0)

#define GBEMG_REG_CUSTOMER_BOX_COVER_AREACODE_OFFSET  (8)







#ifdef __cplusplus
}
#endif

#else
#error "Multiple inclusions of gbemg_registermacro.h"

#endif


