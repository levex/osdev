#ifndef __FLOPPY_H_
#define __FLOPPY_H_

extern void fdc_init();
/* Controller I/O ports */
enum FLPYDSK_IO {

        FLPYDSK_DOR		=	0x3f2,
        FLPYDSK_MSR		=	0x3f4,
        FLPYDSK_FIFO	=	0x3f5,
        FLPYDSK_CTRL	=	0x3f7
};
/* b0-4 of command */
enum FLPYDSK_CMD {

        FDC_CMD_READ_TRACK		=	2,
		FDC_CMD_SPECIFY			=	3,
        FDC_CMD_CHECK_STAT		=	4,
        FDC_CMD_WRITE_SECT		=	5,
        FDC_CMD_READ_SECT		=	6,
        FDC_CMD_CALIBRATE		=	7,
        FDC_CMD_CHECK_INT		=	8,
        FDC_CMD_FORMAT_TRACK	=	0xd,
        FDC_CMD_SEEK			=	0xf
};

enum FLPYDSK_CMD_EXT {

        FDC_CMD_EXT_SKIP                =        0x20,        //00100000
        FDC_CMD_EXT_DENSITY                =        0x40,        //01000000
        FDC_CMD_EXT_MULTITRACK        =        0x80        //10000000
};

/* digital output */

enum FLPYDSK_DOR_MASK {

        FLPYDSK_DOR_MASK_DRIVE0                        =        0,        //00000000        = here for completeness sake
        FLPYDSK_DOR_MASK_DRIVE1                        =        1,        //00000001
        FLPYDSK_DOR_MASK_DRIVE2                        =        2,        //00000010
        FLPYDSK_DOR_MASK_DRIVE3                        =        3,        //00000011
        FLPYDSK_DOR_MASK_RESET                        =        4,        //00000100
        FLPYDSK_DOR_MASK_DMA                        =        8,        //00001000
        FLPYDSK_DOR_MASK_DRIVE0_MOTOR        =        16,        //00010000
        FLPYDSK_DOR_MASK_DRIVE1_MOTOR        =        32,        //00100000
        FLPYDSK_DOR_MASK_DRIVE2_MOTOR        =        64,        //01000000
        FLPYDSK_DOR_MASK_DRIVE3_MOTOR        =        128        //10000000
};

/* main status */


enum FLPYDSK_MSR_MASK {

        FLPYDSK_MSR_MASK_DRIVE1_POS_MODE        =        1,        //00000001
        FLPYDSK_MSR_MASK_DRIVE2_POS_MODE        =        2,        //00000010
        FLPYDSK_MSR_MASK_DRIVE3_POS_MODE        =        4,        //00000100
        FLPYDSK_MSR_MASK_DRIVE4_POS_MODE        =        8,        //00001000
        FLPYDSK_MSR_MASK_BUSY                                =        16,        //00010000
        FLPYDSK_MSR_MASK_DMA                                =        32,        //00100000
        FLPYDSK_MSR_MASK_DATAIO                                =        64, //01000000
        FLPYDSK_MSR_MASK_DATAREG                        =        128        //10000000
};

/* controller status port 0 */
enum FLPYDSK_ST0_MASK {

        FLPYDSK_ST0_MASK_DRIVE0                =        0,                //00000000
        FLPYDSK_ST0_MASK_DRIVE1                =        1,                //00000001
        FLPYDSK_ST0_MASK_DRIVE2                =        2,                //00000010
        FLPYDSK_ST0_MASK_DRIVE3                =        3,                //00000011
        FLPYDSK_ST0_MASK_HEADACTIVE        =        4,                //00000100
        FLPYDSK_ST0_MASK_NOTREADY        =        8,                //00001000
        FLPYDSK_ST0_MASK_UNITCHECK        =        16,                //00010000
        FLPYDSK_ST0_MASK_SEEKEND        =        32,                //00100000
        FLPYDSK_ST0_MASK_INTCODE        =        64                //11000000
};

/* INTCODE types */
enum FLPYDSK_ST0_INTCODE_TYP {

        FLPYDSK_ST0_TYP_NORMAL                =        0,
        FLPYDSK_ST0_TYP_ABNORMAL_ERR=        1,
        FLPYDSK_ST0_TYP_INVALID_ERR        =        2,
        FLPYDSK_ST0_TYP_NOTREADY        =        3
};

/* third gap sizes */
enum FLPYDSK_GAP3_LENGTH {

        FLPYDSK_GAP3_LENGTH_STD = 42,
        FLPYDSK_GAP3_LENGTH_5_14= 32,
        FLPYDSK_GAP3_LENGTH_3_5= 27
};

/* DTL size */
enum FLPYDSK_SECTOR_DTL {

        FLPYDSK_SECTOR_DTL_128        =        0,
        FLPYDSK_SECTOR_DTL_256        =        1,
        FLPYDSK_SECTOR_DTL_512        =        2,
        FLPYDSK_SECTOR_DTL_1024        =        4
};

#endif