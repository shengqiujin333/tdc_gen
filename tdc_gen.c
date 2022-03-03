//used to generate tdc configuration
//input the temperature, transducers length,transducer frequency and crystal frequency.
//output the tdc1000 and tdc7200 configuration
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <lua.h>
#include <lauxlib.h>

int tdc1000_gen(uint8_t *output_bf,uint8_t output_bf_len
                ,float transducer_length,uint32_t transducer_frequency
                ,uint8_t transducer_voltage,uint32_t crystal_frequency
                )
{
    uint16_t i = 0,j = 0;
    uint8_t threshold = 0;
    const uint8_t config0_number_tx = 0x14;
    const uint8_t config1_measure_cycle = 0x40;
    const uint8_t config1_number_rx = 0x7;
    const uint8_t config2 = 0x2;
    const uint8_t config3 = 0x8;
    const uint8_t config4 = 0x11;
    const float length_base_5_vol[] = {0.39,0.51,0.76,1.0,1.2,1.5,1.7};
    const float length_base_24_vol[] = {1.9,2.0,2.2};

    if ((output_bf == NULL) || (output_bf_len < 10)){
        return -1;
    }

    //config_0 register generator output
    i = crystal_frequency / transducer_frequency;
    switch(i){
        case 2:
            j = 0x00;
            break;
        case 4:
            j = 0x01;
            break;
        case 8:
            j = 0x02;
            break;
        case 16:
            j = 0x03;
            break;
        case 32:
            j = 0x04;
            break;
        case 64:
            j = 0x05;
            break;
        case 128:
            j = 0x06;
            break;
        case 256:
            j = 0x07;
            break;
        default:
            return -1;
    }
    output_bf[0] = (j << 5) | config0_number_tx;

    //config_1 register generator output
    output_bf[1] = config1_measure_cycle | config1_number_rx;
    
    //cofngi_2 register generator output
    output_bf[2] = config2;

    //config_3 register generator output
    threshold = 0x3;//maybe need to change
    output_bf[3] = threshold | config3;

    //config_4 register generator output
    output_bf[4] = config4;

    //tof_1 register generator output
    //the parameters which relative tof_1 configruation is 
    //transducer_length, transducer_voltage,transducer_frequency.
    if(transducer_voltage == 5){
        j = 3; 
        for(i = 0;i < sizeof(length_base_5_vol)/sizeof(length_base_5_vol[0]) - 1;i++){
            if((transducer_length > length_base_5_vol[i]) && (transducer_length < length_base_5_vol[i+1])){
                break;
            }
            j += 3;
        }
    }else if(transducer_voltage == 24){
        j = 15;
        for(i = 0; i < sizeof(length_base_24_vol)/sizeof(length_base_24_vol[0]) - 1;i++){
            if((transducer_length > length_base_24_vol[i]) && (transducer_length < length_base_24_vol[i+1])){
                break;
            }
            j += 3;
        }
    }
    i = j / 3;
    //we need calculate the time of every partment.
    output_bf[5] = i<<5;

    //tof_0 register generator output
    output_bf[6] = 0x00;

    //error_flags register generator output
    output_bf[7] = 0x03;

    //timeout register generator output
    output_bf[8] = 0x03;

    //clock rate register generator output
    output_bf[9] = 0x03;
    return 0;
}

int tdc7200_gen(uint8_t *output_bf,uint8_t output_bf_len)
{
    if ((output_bf == NULL) || (output_bf_len < 10)){
        return -1;
    }

    //config1 register generator output
    output_bf[0] = 0x82;

    //config2 register generator output
    output_bf[1] = 0x44;    

    //int_status register generator output
    output_bf[2] = 0x07;

    //int_mask register generator output
    output_bf[3] = 0x07;

    //coarse_cntr_ovf_h register generator output
    output_bf[4] = 0xff;

    //coarse_cntr_ovf_l register generator output
    output_bf[5] = 0xff;

    //clock_cntr_ovf_h register generator output
    output_bf[6] = 0x00;

    //clock_cntr_ovf_l register generator output
    output_bf[7] = 0x00;

    //clock_cntr_stop_mask_h register generator output
    output_bf[8] = 0x00;

    //clock_cntr_stop_mask_l register generator output
    output_bf[9] = 0x00;
    return 0;
}

int tdc_1000_7200_timegen_by_temperature(uint8_t *tdc1000_bf,uint8_t tdc1000_bf_len
                                    ,uint8_t *tdc7200_bf,uint8_t tdc7200_bf_len
                                    ,float temperature,uint32_t crystal_frequency
                                    ,float transducer_length,uint32_t transducer_frequency
                                    ,uint8_t tdc1000_config0_number_tx
                                    )
{
    float speed_standard_min = 0.0f,speed_standard_max = 0.0f,tof_standard_min = 0.0f,tof_standard_max = 0.0f;
    const float temperature_adjust = 10.0f;
    double t0 = 0.0,total_tof = 0.0;
    uint8_t clk_div = 0;
    uint32_t timeout = 0,timemask = 0;
    uint16_t timing_reg = 0;

    if ((tdc1000_bf == NULL) || (tdc1000_bf_len < 10) || (tdc7200_bf == NULL) || (tdc7200_bf_len < 10)){
        return -1;
    }
    
    //calculate the stand tof.
    //speed_standard_min = 1468 + 3.68 *( (temperature - temperature_adjust) - 10) - 0.0279 * ((temperature - temperature_adjust)-10) * ((temperature - temperature_adjust)-10);
    //speed_standard_max = 1468 + 3.68 * ((temperature + temperature_adjust)-10) - 0.0279 * ((temperature + temperature_adjust)-10) * ((temperature + temperature_adjust)-10);
    speed_standard_min = 1404.3 + 4.7 * (temperature - temperature_adjust) - 0.04 * (temperature - temperature_adjust) * (temperature - temperature_adjust);
    speed_standard_max = 1404.3 + 4.7 * (temperature + temperature_adjust) - 0.04 * (temperature + temperature_adjust) * (temperature + temperature_adjust);
    tof_standard_max = transducer_length / speed_standard_min;
    tof_standard_min = transducer_length / speed_standard_max;

    //calculate if we need divied the frequency.
    t0 = 1.0/crystal_frequency;
    //printf("t0 = %lf %d\r\n",t0,crystal_frequency);
    total_tof = tdc1000_config0_number_tx * (1.0f/transducer_frequency) + (0x3ff - 30)*8*t0+128*t0 + 512*t0 + 1024 *t0;
    if (total_tof < tof_standard_max){
        t0 = 2.0/crystal_frequency;
        clk_div = 0x4;
    }
    total_tof = tdc1000_config0_number_tx * (1.0f/transducer_frequency) + (0x3ff - 30)*8*t0+128*t0 + 512*t0 + 1024 *t0;
    //printf("xx:%f %f %f %lf\r\n",total_tof,tof_standard_max,speed_standard_min,t0);
    if (total_tof < tof_standard_max){
        return -1;
    }
    timing_reg = (uint32_t)((tof_standard_max - tdc1000_config0_number_tx * (1.0f/transducer_frequency) - 128*t0 - 512*t0 - 1024 *t0 ) /(8*t0)) + 30;     
    tdc1000_bf[5] = (uint8_t)((timing_reg >> 8) & 0xff);//be carefull ,you need or the result with source buffer
    tdc1000_bf[6] = (uint8_t)(timing_reg & 0xff);
    tdc1000_bf[9] = clk_div;//be carefull ,you need or the result with source buffer

    //printf("1000:%x %x %x\r\n",tdc1000_bf[5],tdc1000_bf[6],tdc1000_bf[9]);
    //calculate the time register of tdc7200.
    timeout = (uint32_t)(tof_standard_max * crystal_frequency);
    timemask = (uint32_t)(tof_standard_min * crystal_frequency);
    tdc7200_bf[6] = (uint8_t)(timeout >> 8);
    tdc7200_bf[7] = (uint8_t)(timeout & 0xff);
    tdc7200_bf[8] = (uint8_t)(timemask >> 8);
    tdc7200_bf[9] = (uint8_t)(timemask & 0xff);
    return 0;
}

//int mytest(uint8_t *bf,uint8_t bfl,float fl,uint32_t ffreq,uint8_t tv,uint32_t cff)
//{
//    bf[0] = 1;
//    bf[1] = 2;
//    printf("%f %d %d %d\r\n",fl,ffreq,tv,cff);
//
//}

static int tdc1000_gen_lua(lua_State* L)
{
    uint8_t tdc1000_bf[10] = {};
    float tlen = luaL_checknumber(L,1);
    uint32_t tfreq = luaL_checknumber(L,2);
    uint8_t tvol = luaL_checknumber(L,3);
    uint32_t cfreq = luaL_checknumber(L,4);
    
    //mytest(tdc1000_bf,9,tlen,tfreq,tvol,cfreq);
    tdc1000_gen(tdc1000_bf,10,tlen,tfreq,tvol,cfreq);
    lua_pushnumber(L,tdc1000_bf[0]);
    lua_pushnumber(L,tdc1000_bf[1]);
    lua_pushnumber(L,tdc1000_bf[2]);
    lua_pushnumber(L,tdc1000_bf[3]);
    lua_pushnumber(L,tdc1000_bf[4]);
    lua_pushnumber(L,tdc1000_bf[5]);
    lua_pushnumber(L,tdc1000_bf[6]);
    lua_pushnumber(L,tdc1000_bf[7]);
    lua_pushnumber(L,tdc1000_bf[8]);
    lua_pushnumber(L,tdc1000_bf[9]);
    return 10;
}

static int tdc7200_gen_lua(lua_State* L)
{
    uint8_t tdc7200_bf[10] = {};
    tdc7200_gen(tdc7200_bf,10);

    lua_pushnumber(L,tdc7200_bf[0]);
    lua_pushnumber(L,tdc7200_bf[1]);
    lua_pushnumber(L,tdc7200_bf[2]);
    lua_pushnumber(L,tdc7200_bf[3]);
    lua_pushnumber(L,tdc7200_bf[4]);
    lua_pushnumber(L,tdc7200_bf[5]);
    lua_pushnumber(L,tdc7200_bf[6]);
    lua_pushnumber(L,tdc7200_bf[7]);
    lua_pushnumber(L,tdc7200_bf[8]);
    lua_pushnumber(L,tdc7200_bf[9]);
    return 10;

}

static int tdc1000_7200_gen_lua(lua_State* L)
{
    uint8_t tdc1000_bf[10] = {};
    uint8_t tdc7200_bf[10] = {};

    float tmp = luaL_checknumber(L,1);
    uint32_t cfreq = luaL_checknumber(L,2);
    float tlen = luaL_checknumber(L,3);
    uint32_t tfreq = luaL_checknumber(L,4);
    uint8_t tdc1000_tx = luaL_checknumber(L,5);

    tdc_1000_7200_timegen_by_temperature(tdc1000_bf,10,tdc7200_bf,10,
            tmp,cfreq,tlen,tfreq,tdc1000_tx);

    //printf("%x %x %x\r\n",tdc1000_bf[5],tdc1000_bf[6],tdc1000_bf[9]);
    lua_pushnumber(L,tdc1000_bf[5]);
    lua_pushnumber(L,tdc1000_bf[6]);
    lua_pushnumber(L,tdc1000_bf[9]);
    lua_pushnumber(L,tdc7200_bf[6]);
    lua_pushnumber(L,tdc7200_bf[7]);
    lua_pushnumber(L,tdc7200_bf[8]);
    lua_pushnumber(L,tdc7200_bf[9]);

    return 7;
}

static const struct luaL_Reg _lib_tdc[] = {
    {"tdc1000_gen",tdc1000_gen_lua},
    {"tdc7200_gen",tdc7200_gen_lua},
    {"tdc_by_temperature",tdc1000_7200_gen_lua},
    {NULL,NULL}
};



int luaopen_tdc_gen(lua_State *L){
    luaL_newlib(L,_lib_tdc);
    return 1;
}


















