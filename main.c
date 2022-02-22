#include "tdc_gen.c"
#include "stdint.h"

int main(void)
{
    uint8_t tdc1000_bf[9] = {};
    uint8_t tdc7200_bf[9] = {};
    float transducer_len = 1.2;
    uint32_t transducer_freq = 1000000;
    uint8_t transducer_vol = 5;
    uint32_t crystal_freq = 8000000;
    float temp = 22.1;
    uint8_t i;

    i =  tdc1000_gen(tdc1000_bf,sizeof(tdc1000_bf)/sizeof(tdc1000_bf[0])
            ,transducer_len,transducer_freq,transducer_vol,crystal_freq);
    tdc7200_gen(tdc7200_bf,sizeof(tdc7200_bf)/sizeof(tdc7200_bf[0]));
    tdc_1000_7200_timegen_by_temperature(tdc1000_bf,
            sizeof(tdc1000_bf)/sizeof(tdc1000_bf[0]),tdc7200_bf,
            sizeof(tdc7200_bf)/sizeof(tdc7200_bf[0]),temp,
            crystal_freq,transducer_len,transducer_freq,0x14);

    for (i = 0;i<9;i++){
        printf("%x\r\n",tdc1000_bf[i]);
    }
    printf("----------------------------\r\n");
    for(i = 0;i<9;i++){
        printf("%x\r\n",tdc7200_bf[i]);
    }
    return 0;
}


















