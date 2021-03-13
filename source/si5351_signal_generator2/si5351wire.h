/*
 * si5351wire.h - Si5351wire library for Arduino
 *
 * Copyright (C) 2015 - 2016 Jason Milldrum <milldrum@gmail.com>
 *                           Dana H. Myers <k6jq@comcast.net>
 *
 * Many defines derived from clk-si5351wire.h in the Linux kernel.
 * Sebastian Hesselbarth <sebastian.hesselbarth@gmail.com>
 * Rabeeh Khoury <rabeeh@solid-run.com>
 *
 * do_div() macro derived from /include/asm-generic/div64.h in
 * the Linux kernel.
 * Copyright (C) 2003 Bernardo Innocenti <bernie@develer.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SI5351wire_H_
#define SI5351wire_H_

#include "Arduino.h"
#include <stdint.h>
#include <Wire.h>

/* Define definitions */
#define SI_SDA    21
#define SI_SCL    22

#define SI5351wire_BUS_BASE_ADDR            0x60
#define SI5351wire_XTAL_FREQ                25000000
#define SI5351wire_PLL_FIXED                80000000000ULL
#define SI5351wire_FREQ_MULT                100ULL
#define SI5351wire_DEFAULT_CLK              1000000000ULL

#define SI5351wire_PLL_VCO_MIN              600000000
#define SI5351wire_PLL_VCO_MAX              900000000
#define SI5351wire_MULTISYNTH_MIN_FREQ      500000
#define SI5351wire_MULTISYNTH_DIVBY4_FREQ   150000000
#define SI5351wire_MULTISYNTH_MAX_FREQ      225000000
#define SI5351wire_MULTISYNTH_SHARE_MAX     100000000
#define SI5351wire_MULTISYNTH_SHARE_MIN     1024000
#define SI5351wire_MULTISYNTH67_MAX_FREQ    SI5351wire_MULTISYNTH_DIVBY4_FREQ
#define SI5351wire_CLKOUT_MIN_FREQ          4000
#define SI5351wire_CLKOUT_MAX_FREQ          SI5351wire_MULTISYNTH_MAX_FREQ
#define SI5351wire_CLKOUT67_MS_MIN          SI5351wire_PLL_VCO_MIN / SI5351wire_MULTISYNTH67_A_MAX
#define SI5351wire_CLKOUT67_MIN_FREQ        SI5351wire_CLKOUT67_MS_MIN / 128
#define SI5351wire_CLKOUT67_MAX_FREQ        SI5351wire_MULTISYNTH67_MAX_FREQ

#define SI5351wire_PLL_A_MIN                15
#define SI5351wire_PLL_A_MAX                90
#define SI5351wire_PLL_B_MAX                (SI5351wire_PLL_C_MAX-1)
#define SI5351wire_PLL_C_MAX                1048575
#define SI5351wire_MULTISYNTH_A_MIN         6
#define SI5351wire_MULTISYNTH_A_MAX         1800
#define SI5351wire_MULTISYNTH67_A_MAX       254
#define SI5351wire_MULTISYNTH_B_MAX         (SI5351wire_MULTISYNTH_C_MAX-1)
#define SI5351wire_MULTISYNTH_C_MAX         1048575
#define SI5351wire_MULTISYNTH_P1_MAX        ((1<<18)-1)
#define SI5351wire_MULTISYNTH_P2_MAX        ((1<<20)-1)
#define SI5351wire_MULTISYNTH_P3_MAX        ((1<<20)-1)
#define SI5351wire_VCXO_PULL_MIN            30
#define SI5351wire_VCXO_PULL_MAX            240
#define SI5351wire_VCXO_MARGIN              103

#define SI5351wire_DEVICE_STATUS            0
#define SI5351wire_INTERRUPT_STATUS         1
#define SI5351wire_INTERRUPT_MASK           2
#define SI5351wire_STATUS_SYS_INIT          (1<<7)
#define SI5351wire_STATUS_LOL_B             (1<<6)
#define SI5351wire_STATUS_LOL_A             (1<<5)
#define SI5351wire_STATUS_LOS               (1<<4)
#define SI5351wire_OUTPUT_ENABLE_CTRL       3
#define SI5351wire_OEB_PIN_ENABLE_CTRL      9
#define SI5351wire_PLL_INPUT_SOURCE         15
#define SI5351wire_CLKIN_DIV_MASK           (3<<6)
#define SI5351wire_CLKIN_DIV_1              (0<<6)
#define SI5351wire_CLKIN_DIV_2              (1<<6)
#define SI5351wire_CLKIN_DIV_4              (2<<6)
#define SI5351wire_CLKIN_DIV_8              (3<<6)
#define SI5351wire_PLLB_SOURCE              (1<<3)
#define SI5351wire_PLLA_SOURCE              (1<<2)

#define SI5351wire_CLK0_CTRL                16
#define SI5351wire_CLK1_CTRL                17
#define SI5351wire_CLK2_CTRL                18
#define SI5351wire_CLK3_CTRL                19
#define SI5351wire_CLK4_CTRL                20
#define SI5351wire_CLK5_CTRL                21
#define SI5351wire_CLK6_CTRL                22
#define SI5351wire_CLK7_CTRL                23
#define SI5351wire_CLK_POWERDOWN            (1<<7)
#define SI5351wire_CLK_INTEGER_MODE         (1<<6)
#define SI5351wire_CLK_PLL_SELECT           (1<<5)
#define SI5351wire_CLK_INVERT               (1<<4)
#define SI5351wire_CLK_INPUT_MASK           (3<<2)
#define SI5351wire_CLK_INPUT_XTAL           (0<<2)
#define SI5351wire_CLK_INPUT_CLKIN          (1<<2)
#define SI5351wire_CLK_INPUT_MULTISYNTH_0_4 (2<<2)
#define SI5351wire_CLK_INPUT_MULTISYNTH_N   (3<<2)
#define SI5351wire_CLK_DRIVE_STRENGTH_MASK  (3<<0)
#define SI5351wire_CLK_DRIVE_STRENGTH_2MA   (0<<0)
#define SI5351wire_CLK_DRIVE_STRENGTH_4MA   (1<<0)
#define SI5351wire_CLK_DRIVE_STRENGTH_6MA   (2<<0)
#define SI5351wire_CLK_DRIVE_STRENGTH_8MA   (3<<0)

#define SI5351wire_CLK3_0_DISABLE_STATE     24
#define SI5351wire_CLK7_4_DISABLE_STATE     25
#define SI5351wire_CLK_DISABLE_STATE_MASK   3
#define SI5351wire_CLK_DISABLE_STATE_LOW    0
#define SI5351wire_CLK_DISABLE_STATE_HIGH   1
#define SI5351wire_CLK_DISABLE_STATE_FLOAT  2
#define SI5351wire_CLK_DISABLE_STATE_NEVER  3

#define SI5351wire_PARAMETERS_LENGTH        8
#define SI5351wire_PLLA_PARAMETERS          26
#define SI5351wire_PLLB_PARAMETERS          34
#define SI5351wire_CLK0_PARAMETERS          42
#define SI5351wire_CLK1_PARAMETERS          50
#define SI5351wire_CLK2_PARAMETERS          58
#define SI5351wire_CLK3_PARAMETERS          66
#define SI5351wire_CLK4_PARAMETERS          74
#define SI5351wire_CLK5_PARAMETERS          82
#define SI5351wire_CLK6_PARAMETERS          90
#define SI5351wire_CLK7_PARAMETERS          91
#define SI5351wire_CLK6_7_OUTPUT_DIVIDER    92
#define SI5351wire_OUTPUT_CLK_DIV_MASK      (7 << 4)
#define SI5351wire_OUTPUT_CLK6_DIV_MASK     (7 << 0)
#define SI5351wire_OUTPUT_CLK_DIV_SHIFT     4
#define SI5351wire_OUTPUT_CLK_DIV6_SHIFT    0
#define SI5351wire_OUTPUT_CLK_DIV_1         0
#define SI5351wire_OUTPUT_CLK_DIV_2         1
#define SI5351wire_OUTPUT_CLK_DIV_4         2
#define SI5351wire_OUTPUT_CLK_DIV_8         3
#define SI5351wire_OUTPUT_CLK_DIV_16        4
#define SI5351wire_OUTPUT_CLK_DIV_32        5
#define SI5351wire_OUTPUT_CLK_DIV_64        6
#define SI5351wire_OUTPUT_CLK_DIV_128       7
#define SI5351wire_OUTPUT_CLK_DIVBY4       (3<<2)

#define SI5351wire_SSC_PARAM0               149
#define SI5351wire_SSC_PARAM1               150
#define SI5351wire_SSC_PARAM2               151
#define SI5351wire_SSC_PARAM3               152
#define SI5351wire_SSC_PARAM4               153
#define SI5351wire_SSC_PARAM5               154
#define SI5351wire_SSC_PARAM6               155
#define SI5351wire_SSC_PARAM7               156
#define SI5351wire_SSC_PARAM8               157
#define SI5351wire_SSC_PARAM9               158
#define SI5351wire_SSC_PARAM10              159
#define SI5351wire_SSC_PARAM11              160
#define SI5351wire_SSC_PARAM12              161

#define SI5351wire_VXCO_PARAMETERS_LOW      162
#define SI5351wire_VXCO_PARAMETERS_MID      163
#define SI5351wire_VXCO_PARAMETERS_HIGH     164

#define SI5351wire_CLK0_PHASE_OFFSET        165
#define SI5351wire_CLK1_PHASE_OFFSET        166
#define SI5351wire_CLK2_PHASE_OFFSET        167
#define SI5351wire_CLK3_PHASE_OFFSET        168
#define SI5351wire_CLK4_PHASE_OFFSET        169
#define SI5351wire_CLK5_PHASE_OFFSET        170

#define SI5351wire_PLL_RESET                177
#define SI5351wire_PLL_RESET_B              (1<<7)
#define SI5351wire_PLL_RESET_A              (1<<5)

#define SI5351wire_CRYSTAL_LOAD             183
#define SI5351wire_CRYSTAL_LOAD_MASK        (3<<6)
#define SI5351wire_CRYSTAL_LOAD_0PF         (0<<6)
#define SI5351wire_CRYSTAL_LOAD_6PF         (1<<6)
#define SI5351wire_CRYSTAL_LOAD_8PF         (2<<6)
#define SI5351wire_CRYSTAL_LOAD_10PF        (3<<6)

#define SI5351wire_FANOUT_ENABLE            187
#define SI5351wire_CLKIN_ENABLE             (1<<7)
#define SI5351wire_XTAL_ENABLE              (1<<6)
#define SI5351wire_MULTISYNTH_ENABLE        (1<<4)


/* Macro definitions */

//#define RFRAC_DENOM ((1L << 20) - 1)
#define RFRAC_DENOM 1000000ULL

/*
 * Based on former asm-ppc/div64.h and asm-m68knommu/div64.h
 *
 * The semantics of do_div() are:
 *
 * uint32_t do_div(uint64_t *n, uint32_t base)
 * {
 *      uint32_t remainder = *n % base;
 *      *n = *n / base;
 *      return remainder;
 * }
 *
 * NOTE: macro parameter n is evaluated multiple times,
 *       beware of side effects!
 */

# define do_div(n,base) ({                                      \
        uint64_t __base = (base);                               \
        uint64_t __rem;                                         \
        __rem = ((uint64_t)(n)) % __base;                       \
        (n) = ((uint64_t)(n)) / __base;                         \
        __rem;                                                  \
 })

/* Enum definitions */

/*
 * enum si5351wire_variant - SiLabs Si5351wire chip variant
 * @SI5351wire_VARIANT_A: Si5351wireA (8 output clocks, XTAL input)
 * @SI5351wire_VARIANT_A3: Si5351wireA MSOP10 (3 output clocks, XTAL input)
 * @SI5351wire_VARIANT_B: Si5351wireB (8 output clocks, XTAL/VXCO input)
 * @SI5351wire_VARIANT_C: Si5351wireC (8 output clocks, XTAL/CLKIN input)
 */
/*
enum si5351wire_variant {
	SI5351wire_VARIANT_A = 1,
	SI5351wire_VARIANT_A3 = 2,
	SI5351wire_VARIANT_B = 3,
	SI5351wire_VARIANT_C = 4,
};
*/

enum si5351wire_clock {SI5351wire_CLK0, SI5351wire_CLK1, SI5351wire_CLK2, SI5351wire_CLK3,
	SI5351wire_CLK4, SI5351wire_CLK5, SI5351wire_CLK6, SI5351wire_CLK7};

enum si5351wire_pll {SI5351wire_PLLA, SI5351wire_PLLB};

enum si5351wire_drive {SI5351wire_DRIVE_2MA, SI5351wire_DRIVE_4MA, SI5351wire_DRIVE_6MA, SI5351wire_DRIVE_8MA};

enum si5351wire_clock_source {SI5351wire_CLK_SRC_XTAL, SI5351wire_CLK_SRC_CLKIN, SI5351wire_CLK_SRC_MS0, SI5351wire_CLK_SRC_MS};

enum si5351wire_clock_disable {SI5351wire_CLK_DISABLE_LOW, SI5351wire_CLK_DISABLE_HIGH, SI5351wire_CLK_DISABLE_HI_Z, SI5351wire_CLK_DISABLE_NEVER};

enum si5351wire_clock_fanout {SI5351wire_FANOUT_CLKIN, SI5351wire_FANOUT_XO, SI5351wire_FANOUT_MS};

enum si5351wire_pll_input {SI5351wire_PLL_INPUT_XO, SI5351wire_PLL_INPUT_CLKIN};

/* Struct definitions */

struct Si5351wireRegSet
{
	uint32_t p1;
	uint32_t p2;
	uint32_t p3;
};

struct Si5351wireStatus
{
	uint8_t SYS_INIT;
	uint8_t LOL_B;
	uint8_t LOL_A;
	uint8_t LOS;
	uint8_t REVID;
};

struct Si5351wireIntStatus
{
	uint8_t SYS_INIT_STKY;
	uint8_t LOL_B_STKY;
	uint8_t LOL_A_STKY;
	uint8_t LOS_STKY;
};

class Si5351wire
{
public:
	Si5351wire(uint8_t i2c_addr = SI5351wire_BUS_BASE_ADDR,uint8_t i2c_sda =  SI_SDA,uint8_t i2c_scl = SI_SCL);
	
	bool init(uint8_t, uint32_t, int32_t);
	void reset(void);
	uint8_t set_freq(uint64_t, enum si5351wire_clock, uint8_t reset_pll = 0);
	uint8_t set_freq_manual(uint64_t, uint64_t, enum si5351wire_clock);
	void set_pll(uint64_t, enum si5351wire_pll);
	void set_ms(enum si5351wire_clock, struct Si5351wireRegSet, uint8_t, uint8_t, uint8_t);
	void output_enable(enum si5351wire_clock, uint8_t);
	void drive_strength(enum si5351wire_clock, enum si5351wire_drive);
	void update_status(void);
	void set_correction(int32_t, enum si5351wire_pll_input);
	void set_phase(enum si5351wire_clock, uint8_t);
	int32_t get_correction(enum si5351wire_pll_input);
	void pll_reset(enum si5351wire_pll);
	void set_ms_source(enum si5351wire_clock, enum si5351wire_pll);
	void set_int(enum si5351wire_clock, uint8_t);
	void set_clock_pwr(enum si5351wire_clock, uint8_t);
	void set_clock_invert(enum si5351wire_clock, uint8_t);
	void set_clock_source(enum si5351wire_clock, enum si5351wire_clock_source);
	void set_clock_disable(enum si5351wire_clock, enum si5351wire_clock_disable);
	void set_clock_fanout(enum si5351wire_clock_fanout, uint8_t);
	void set_pll_input(enum si5351wire_pll, enum si5351wire_pll_input);
	void set_vcxo(uint64_t, uint8_t);
  void set_ref_freq(uint32_t, enum si5351wire_pll_input);
	uint8_t si5351wire_write_bulk(uint8_t, uint8_t, uint8_t *);
	uint8_t i2c_receive_byte_data(uint8_t);
	uint8_t si5351wire_write(uint8_t, uint8_t);
	uint8_t si5351wire_read(uint8_t);
	struct Si5351wireStatus dev_status = {.SYS_INIT = 0, .LOL_B = 0, .LOL_A = 0,
    .LOS = 0, .REVID = 0};
	struct Si5351wireIntStatus dev_int_status = {.SYS_INIT_STKY = 0, .LOL_B_STKY = 0,
    .LOL_A_STKY = 0, .LOS_STKY = 0};
	enum si5351wire_pll pll_assignment[8];
	uint64_t clk_freq[8];
	uint64_t plla_freq;
	uint64_t pllb_freq;
	enum si5351wire_pll_input plla_ref_osc;
	enum si5351wire_pll_input pllb_ref_osc;
	uint32_t xtal_freq[2];

private:
	uint64_t pll_calc(enum si5351wire_pll, uint64_t, struct Si5351wireRegSet *, int32_t, uint8_t);
	uint64_t multisynth_calc(uint64_t, uint64_t, struct Si5351wireRegSet *);
	uint64_t multisynth67_calc(uint64_t, uint64_t, struct Si5351wireRegSet *);
	void update_sys_status(struct Si5351wireStatus *);
	void update_int_status(struct Si5351wireIntStatus *);
	void ms_div(enum si5351wire_clock, uint8_t, uint8_t);
	uint8_t select_r_div(uint64_t *);
	
	//sI2c_direct			i2c;
	
	uint8_t		 		select_r_div_ms67(uint64_t *);
	int32_t 			ref_correction[2];
	uint8_t 			clkin_div;
	uint8_t 			i2c_bus_addr;
	bool 				clk_first_set[8];
	struct Si5351wireRegSet ms_reg_save[8];
	uint8_t				sda;
	uint8_t				scl;
};

#endif /* SI5351wire_H_ */
