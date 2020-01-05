/*
 * Uint162STR.c
 *
 *  Created on: 2018��6��5��
 *      Author: Thinkpad
 */

#include <stdio.h>
#include <math.h>
#define INT_MAX 2147483647
#define INT_MIN (-2147483647-1)//���������ֱ�ʾ��ʽ��-2147483648�ᱨ��

const char* int_to_str(int iVal)
{
    static char str[12];
    int isNegative = 0;
    int i = 0,j=0;
    if (iVal == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }
    if (iVal == INT_MIN)
    {
        int_to_str(iVal + 1);
        char *tmp = str;
        while (*tmp != '\0')
            tmp++;
        tmp--;
        *tmp += 1;
        return str;
    }
    if (iVal < 0)
    {
        iVal *= -1;
        isNegative = 1;
        str[i++] = '-';
        j++;
    }
    while (iVal)
    {
        str[i++]=iVal % 10+'0';
        iVal /= 10;
    }
    str[i--] = '\0';
    while (j < i)
    {
        char ch = str[i];
        str[i--] = str[j];
        str[j++] = ch;
    }
    return str;

}


