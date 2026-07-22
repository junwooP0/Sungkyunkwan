#include "hpfp.h"
#include <limits.h>
#include <stdlib.h>
#define TMAX INT_MAX
#define TMIN INT_MIN

hpfp int_converter(int input)
{
    unsigned int s = 0;
    unsigned int exp = 0;
    unsigned int frac = 0;
    unsigned int temp = 0;
    unsigned int E = 0;
    unsigned int total = 0;
    int length = 0;

    if (input > 0)
    {
        s = 0;
        temp = input;
    }
    else if (input < 0)
    {
        s = 1;
        temp = -input;
    }
    else
        return 0;

    while (temp >= 2)
    {
        temp >>= 1;
        E++;
    }

    exp = E + 15;

    if (exp > 30)
        return (s << 15) + (0x1F << 10);

    if (input > 0)
        temp = input;

    else
        temp = -input;

    while (temp > 0)
    {
        temp >>= 1;
        length++;
    }
    if (input > 0)
        temp = input;

    else
        temp = -input;

    if (length > 10)
    {
        frac = (temp >> (length - 11)) & 0x3FF;
    }
    else
    {
        frac = (temp << (11 - length)) & 0x3FF;
    }

    total = (s << 15) + (exp << 10) + frac;

    return total;
}

int hpfp_to_int_converter(hpfp input)
{
    unsigned int s = (input >> 15) & 1;
    unsigned int exp = (input >> 10) & 0x1F;
    unsigned int frac = input & 0x3FF;
    unsigned int total = 0;
    int E = exp - 15;
    int result = 0;

    if (exp == 0x1F)
    {
        if (frac == 0)
        {
            if (s == 0)
                return TMAX;
            else
                return TMIN;
        }
        else
            return TMIN;
    }

    if (E >= 0)
    {
        total = (1 << E) * (1 + (float)frac / 1024);
    }
    else
    {
        return 0;
    }

    if (s == 0)
        result = total;
    else
        result = -total;

    return result;
}

hpfp float_converter(float input)
{
    unsigned int s = 0;
    unsigned int exp = 0;
    unsigned int frac = 0;
    unsigned int E = 0;
    unsigned int total = 0;
    float temp = 0;

    if (input > 0)
    {
        s = 0;
        temp = input;
    }
    else if (input < 0)
    {
        s = 1;
        temp = -input;
    }
    else
        return 0;

    if (temp >= 1)
    {
        while (temp >= 2)
        {
            temp /= 2;
            E++;
        }
    }
    else
    {
        while (temp < 1)
        {
            temp *= 2;
            E--;
        }
    }
    exp = E + 15;

    if (exp > 30)
        return (s << 15) + (0x1F << 10);
    if (exp < 0)
        return s << 15;

    frac = (unsigned int)((temp - 1) * 1024) & 0x3FF;

    total = (s << 15) + (exp << 10) + frac;
    return total;
}

float hpfp_to_float_converter(hpfp input)
{
    unsigned int s = (input >> 15) & 1;
    unsigned int exp = (input >> 10) & 0x1F;
    unsigned int frac = input & 0x3FF;
    int E = exp - 15;
    float result = 0;
    float total = 0;

    if (exp == 0x1F)
    {
        if (frac == 0)
        {
            if (s == 0)
                return 1.0 / 0.0;
            else
                return -1.0 / 0.0;
        }
        else
            return 0.0 / 0.0;
    }
    if (exp == 0)
    {
        total = (float)frac / 1024;
        for (int i = 0; i < E; i++)
        {
            total /= 2;
        }
        if (s == 0)
            result = total;
        else
            result = -total;
        return result;
    }

    total = 1 + (float)frac / 1024;

    if (E >= 0)
    {
        for (int i = 0; i < E; i++)
        {
            total *= 2;
        }
    }
    else
    {
        for (int i = 0; i < -E; i++)
        {
            total /= 2;
        }
    }

    if (s == 0)
        result = total;
    else
        result = -total;

    return result;
}

hpfp addition_function(hpfp a, hpfp b)
{
    unsigned int s1 = (a >> 15) & 1;
    unsigned int exp1 = (a >> 10) & 0x1F;
    unsigned int frac1 = a & 0x3FF;
    unsigned int s2 = (b >> 15) & 1;
    unsigned int exp2 = (b >> 10) & 0x1F;
    unsigned int frac2 = b & 0x3FF;
    unsigned int s = 0;
    unsigned int result = 0;
    unsigned int frac3 = 0;
    unsigned int frac4 = 0;
    unsigned int frac = 0;
    int exp = 0;
    int diff = 0;

    if (exp1 == 0x1F && frac1 == 0)
    {
        if (exp2 == 0x1F && frac2 == 0 && s1 != s2)
            return (s << 15) + (0x1F << 10) + 1;
        else
            return a;
    }
    if (exp2 == 0x1F && frac2 == 0)
    {
        if (exp1 == 0x1F && frac1 == 0 && s1 != s2)
            return (s << 15) + (0x1F << 10) + 1;
        else
            return b;
    }
    if (exp1 == 0x1F && frac1 != 0)
        return (s << 15) + (0x1F << 10) + 1;

    if (exp2 == 0x1F && frac2 != 0)
        return (s << 15) + (0x1F << 10) + 1;

    frac3 = (frac1 + 1024);
    frac4 = (frac2 + 1024);

    if (exp1 == 0)
        frac3 = frac1;
    if (exp2 == 0)
        frac4 = frac2;

    if (exp1 > exp2)
    {
        s = s1;
        exp = exp1;
        diff = exp1 - exp2;
        frac4 >>= diff;
    }
    else
    {
        s = s2;
        exp = exp2;
        diff = exp2 - exp1;
        frac3 >>= diff;
    }

    if (s1 == s2)
        frac = frac3 + frac4;
    else
    {
        if (frac3 > frac4)
        {
            frac = frac3 - frac4;
            s = s1;
        }
        else
        {
            frac = frac4 - frac3;
            s = s2;
        }
    }

    while (frac >= 2048)
    {
        frac >>= 1;
        exp++;
    }

    while (frac < 1024)
    {
        frac <<= 1;
        exp--;
    }

    if (exp >= 31)
        return (s << 15) + (0x1F << 10);
    if (exp < 0)
        return 0;

    result = (s << 15) + (exp << 10) + ((frac) & 0x3FF);
    return result;
}

hpfp multiply_function(hpfp a, hpfp b)
{
    unsigned int s1 = (a >> 15) & 1;
    unsigned int exp1 = (a >> 10) & 0x1F;
    unsigned int frac1 = a & 0x3FF;
    unsigned int s2 = (b >> 15) & 1;
    unsigned int exp2 = (b >> 10) & 0x1F;
    unsigned int frac2 = b & 0x3FF;
    unsigned int s = 0;
    unsigned int frac3 = 0;
    unsigned int frac4 = 0;
    unsigned int frac = 0;
    unsigned int result = 0;
    unsigned int temp = 0;
    unsigned int temp2 = 0;
    int exp = 0;
    int near = 0;
    int remain = 0;

    s = s1 ^ s2;

    if (exp1 == 0x1F && frac1 == 0)
    {
        if (exp2 == 0 && frac2 == 0)
        {
            return (s1 << 15) + (0x1F << 10) + 1; // NaN
        }
        else
            return (s << 15) + (0x1F << 10);
    }
    if (exp2 == 0x1F && frac2 == 0)
    {
        if (exp1 == 0 && frac1 == 0)
        {
            return (s2 << 15) + (0x1F << 10) + 1; // NaN
        }
        else
            return (s << 15) + (0x1F << 10);
    }
    if ((exp1 == 0 && frac1 == 0) || (exp2 == 0 && frac2 == 0))
    {
        return 0;
    }

    frac3 = frac1 + 1024;
    frac4 = frac2 + 1024;

    if (exp1 == 0)
        frac3 = frac1;
    if (exp2 == 0)
        frac4 = frac2;

    frac = frac3 * frac4 >> 10;
    exp = exp1 + exp2 - 15;
    temp = frac3 * frac4;
    near = (temp >> 9) & 1;
    temp2 = temp & 0x1FF;
    if (temp2 == 0)
        remain = 0;
    else
        remain = 1;

    if (near == 1)
    {
        if (remain == 1)
        {
            frac++;
        }
        else
        {
            if (frac & 1)
                frac++;
        }
    }

    if (frac >= 2048)
    {
        frac >>= 1;
        exp++;
    }

    while (frac < 1024)
    {
        frac <<= 1;
        exp--;
    }

    if (exp >= 31)
        return (s << 15) + (0x1F << 10);
    if (exp < 0)
        return 0;

    result = (s << 15) + (exp << 10) + ((frac) & 0x3FF);
    return result;
}

char *comparison_function(hpfp a, hpfp b)
{
    unsigned int s1 = (a >> 15) & 1;
    unsigned int exp1 = (a >> 10) & 0x1F;
    unsigned int frac1 = a & 0x3FF;
    unsigned int s2 = (b >> 15) & 1;
    unsigned int exp2 = (b >> 10) & 0x1F;
    unsigned int frac2 = b & 0x3FF;

    if ((exp1 == 0x1F && frac1 != 0) || (exp2) == 0x1F && frac2 != 0)
    {
        return "=";
    }
    if ((exp1 == 0x1F && frac1 == 0) && (exp2 == 0x1F && frac2 == 0))
    {
        if (s1 == 0 && s2 == 1)
        {
            return ">";
        }
        else if (s1 == 1 && s2 == 0)
        {
            return "<";
        }
        else
        {
            return "=";
        }
    }
    if ((exp1 == 0x1F && frac1 == 0) || (exp2 == 0x1F && frac2 == 0))
    {
        if (exp1 == 0x1F && frac1 == 0)
        {
            if (s1 == 0)
            {
                return ">";
            }
            else
            {
                return "<";
            }
        }
        else
        {
            if (s2 == 0)
            {
                return "<";
            }
            else
            {
                return ">";
            }
        }
    }

    if (s1 == 0 && s2 == 1)
    {
        return ">";
    }
    else if (s1 == 1 && s2 == 0)
    {
        return "<";
    }
    else if (s1 == 0 && s2 == 0)
    {
        if (exp1 > exp2)
        {
            return ">";
        }
        else if (exp1 < exp2)
        {
            return "<";
        }
        else
        {
            if (frac1 > frac2)
            {
                return ">";
            }
            else if (frac1 < frac2)
            {
                return "<";
            }
            else
            {
                return "=";
            }
        }
    }
    else
    {
        if (exp1 > exp2)
        {
            return "<";
        }
        else if (exp1 < exp2)
        {
            return ">";
        }
        else
        {
            if (frac1 > frac2)
            {
                return "<";
            }
            else if (frac1 < frac2)
            {
                return ">";
            }
            else
            {
                return "=";
            }
        }
    }
}

char *hpfp_to_bits_converter(hpfp result)
{
    char *bits;
    bits = (char *)malloc(sizeof(char) * 17);
    {
        for (int i = 0; i <= 15; i++)
        {
            bits[i] = ((result >> (15 - i)) & 1) + '0';
        }
    }
    bits[16] = '\0';
    
    return bits;
}

char *hpfp_flipper(char *input)
{
}
