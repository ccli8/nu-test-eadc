/*
 * Copyright (c) 2006-2020 Arm Limited and affiliates.
 * SPDX-License-Identifier: Apache-2.0
 */
#include "mbed.h"
#include "PeripheralPins.h"

/* Structure for extending sampling time on per-pin basis */
struct nu_eadc_extsmpt {
    PinName     pin;
    uint32_t    value;
};

static struct nu_eadc_extsmpt eadc_extsmpt_arr[] = {
#if TARGET_NUMAKER_PFM_NANO130
    MBED_CONF_TARGET_ADC_SMPLCNT_LIST
#else
    MBED_CONF_TARGET_EADC_EXTSMPT_LIST
#endif
};

int main(void)
{
    struct nu_eadc_extsmpt *eadc_extsmpt_pos = eadc_extsmpt_arr;
    struct nu_eadc_extsmpt *eadc_extsmpt_end = eadc_extsmpt_arr + sizeof (eadc_extsmpt_arr) / sizeof (eadc_extsmpt_arr[0]);
    for (; eadc_extsmpt_pos != eadc_extsmpt_end; eadc_extsmpt_pos ++) {
        uint32_t port_index = NU_PINPORT(eadc_extsmpt_pos->pin);
        uint32_t pin_index = NU_PININDEX(eadc_extsmpt_pos->pin);

        ADCName adc = (ADCName) pinmap_peripheral(eadc_extsmpt_pos->pin, PinMap_ADC);
#if TARGET_NUMAKER_PFM_NANO130
        ADC_T *adc_base = (ADC_T *) NU_MODBASE(adc);
#else
        EADC_T *eadc_base = (EADC_T *) NU_MODBASE(adc);
#endif
        uint32_t smp_chn = NU_MODSUBINDEX(adc);
#if TARGET_NUMAKER_PFM_NANO130
        uint32_t smp_mod =  smp_chn;
#elif TARGET_NUMAKER_PFM_NUC472
        uint32_t smp_mod =  NU_MODINDEX(adc) * 8 + smp_chn;
#else
        uint32_t smp_mod =  smp_chn;
#endif

        uint32_t extsmpt_value = 0;

#if TARGET_NUMAKER_PFM_NANO130
        extsmpt_value = (smp_mod < 8) ?
            ((adc_base->SMPLCNT0 & (ADC_SMPLCNT0_CH0SAMPCNT_Msk << (smp_mod * 4))) >> (smp_mod * 4)) :
            ((adc_base->SMPLCNT1 & (ADC_SMPLCNT1_CH8SAMPCNT_Msk << ((smp_mod - 8) * 4))) >> ((smp_mod - 8) * 4));
#elif TARGET_NUMAKER_PFM_NUC472
        extsmpt_value = (smp_mod < EADC1_SAMPLE_MODULE0) ?
            ((eadc_base->EXTSMPT & EADC_EXTSMPT_EXTSMPT0_Msk) >> EADC_EXTSMPT_EXTSMPT0_Pos) :
            ((eadc_base->EXTSMPT & EADC_EXTSMPT_EXTSMPT1_Msk) >> EADC_EXTSMPT_EXTSMPT1_Pos);
#else
        extsmpt_value = (eadc_base->SCTL[smp_chn] & EADC_SCTL_EXTSMPT_Msk) >> EADC_SCTL_EXTSMPT_Pos;
#endif
        printf("Initial: PIN: %d.%d, (E)ADC: %d.%d, EXTSMPT: %d\n", port_index, pin_index, smp_mod, smp_chn, extsmpt_value);

        AnalogIn audio_in(eadc_extsmpt_pos->pin);

#if TARGET_NUMAKER_PFM_NANO130
        extsmpt_value = (smp_mod < 8) ?
            ((adc_base->SMPLCNT0 & (ADC_SMPLCNT0_CH0SAMPCNT_Msk << (smp_mod * 4))) >> (smp_mod * 4)) :
            ((adc_base->SMPLCNT1 & (ADC_SMPLCNT1_CH8SAMPCNT_Msk << ((smp_mod - 8) * 4))) >> ((smp_mod - 8) * 4));
#elif TARGET_NUMAKER_PFM_NUC472
        extsmpt_value = (smp_mod <EADC1_SAMPLE_MODULE0) ?
            ((eadc_base->EXTSMPT & EADC_EXTSMPT_EXTSMPT0_Msk) >> EADC_EXTSMPT_EXTSMPT0_Pos) :
            ((eadc_base->EXTSMPT & EADC_EXTSMPT_EXTSMPT1_Msk) >> EADC_EXTSMPT_EXTSMPT1_Pos);
#else
        extsmpt_value = (eadc_base->SCTL[smp_chn] & EADC_SCTL_EXTSMPT_Msk) >> EADC_SCTL_EXTSMPT_Pos;
#endif
        printf("AudioIn: PIN: %d.%d, (E)ADC: %d.%d, EXTSMPT: %d\n", port_index, pin_index, smp_mod, smp_chn, extsmpt_value);
    }

    return 0;
}
