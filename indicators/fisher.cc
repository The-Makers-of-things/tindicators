/*
 * This file is part of tindicators, licensed under GNU LGPL v3.
 * Author: Ilya Pikulin <ilya.pikulin@gmail.com>, 2019, 2021
 * Author: Lewis Van Winkle <lv@codeplea.com>, 2016-2018
 */


#include <new>

#include "../indicators.h"


int ti_fisher_start(TI_REAL const *options) {
    return (int)options[0]-1;
}

#define HL(X) (0.5 * (high[(X)] + low[(X)]))

int ti_fisher(int size, TI_REAL const *const *inputs, TI_REAL const *options, TI_REAL *const *outputs) {
    const TI_REAL *high = inputs[0];
    const TI_REAL *low = inputs[1];

    TI_REAL *fisher = outputs[0];
    TI_REAL *signal = outputs[1];
    const int period = (int)options[0];

    if (period < 1) return TI_INVALID_OPTION;
    if (size <= ti_fisher_start(options)) return TI_OKAY;

    int trail = 0, maxi = -1, mini = -1;
    TI_REAL max = HL(0);
    TI_REAL min = HL(0);
    TI_REAL val1 = 0.0;
    TI_REAL bar;
    TI_REAL fish = 0.0;

    int i, j;
    for (i = period-1; i < size; ++i, ++trail) {
        /* Maintain highest. */
        bar = HL(i);
        if (maxi < trail) {
            maxi = trail;
            max = HL(maxi);
            j = trail;
            while(++j <= i) {
                bar = HL(j);
                if (bar >= max) {
                    max = bar;
                    maxi = j;
                }
            }
        } else if (bar >= max) {
            maxi = i;
            max = bar;
        }


        /* Maintain lowest. */
        bar = HL(i);
        if (mini < trail) {
            mini = trail;
            min = HL(mini);
            j = trail;
            while(++j <= i) {
                bar = HL(j);
                if (bar <= min) {
                    min = bar;
                    mini = j;
                }
            }
        } else if (bar <= min) {
            mini = i;
            min = bar;
        }

        TI_REAL mm = max - min;
        if (mm == 0.0) mm = 0.001;
        val1 = 0.33 * 2.0 * ( (HL(i)-min) / (mm) - 0.5) + 0.67 * val1;
        if (val1 > 0.99) val1 = .999;
        if (val1 < -0.99) val1 = -.999;

        *signal++ = fish;
        fish = 0.5 * log((1.0+val1)/(1.0-val1)) + 0.5 * fish;
        *fisher++ = fish;
    }


    assert(fisher - outputs[0] == size - ti_fisher_start(options));
    assert(signal - outputs[1] == size - ti_fisher_start(options));
    return TI_OKAY;
}
