#include "math_common.h"

math_errno_t math_errno = MATH_SUCCESS;

/**
 * @brief 斜坡函数
 * @param[in] target_value 期望值
 * @param[in] current_value 当前值
 * @param[in] step_length 步长
 * @return 本次迭代结果
 */
float Ramp_float(float target_value,float current_value,float step_length)
{
	float s = sign(target_value - current_value);
	float temp = current_value + s * step_length;
	if(s*(temp - target_value)>=0)
	{
		return target_value;
	}
	else
	{
		return temp;
	}
}

/**
 * @brief 求二进制数x的最高位 
 * @param[in] x 输入值
 * @param[out] y x最高位所在位数
 * @return x保留最高位结果
*/
integer_t high_bit(integer_t x, integer_t *y)
{
    integer_t i = 1, temp, lb = 1, rb = sizeof(integer_t) * 8, mid;
    // 将最高位以下部分赋1
    while (i < sizeof(integer_t) * 8)
    {
        x |= x >> i;
        i <<= 1;
    }
    x = x - (x >> 1);
    // 二分求所在位数
    if (y != NULL)
    {
        while (lb + 1 <= rb)
        {
            mid = (rb + lb) >> 1;
            temp = 1 << (mid - 1);
            if (x == temp)
            {
                break;
            }
            else if (x > temp)
            {
                lb = mid;
            }
            else
            {
                rb = mid;
            }
        }
        *y = mid;
    }
    return x;
}

/**
 * @brief 求二进制数x的最低位
 * @param x 输入值
 * @param y x最低位所在位数
 * @return x保留最低位结果 
 */
integer_t low_bit(integer_t x, integer_t *y)
{
    integer_t temp, lb = 1, rb = sizeof(integer_t) * 8, mid;
    //求最低位
    x = x & -x;
    // 二分求所在位数
    if (y != NULL)
    {
        while (lb + 1 <= rb)
        {
            mid = (rb + lb) >> 1;
            temp = 1 << (mid - 1);
            if (x == temp)
            {
                break;
            }
            else if (x > temp)
            {
                lb = mid;
            }
            else
            {
                rb = mid;
            }
        }
        *y = mid;
    }
    return x;
}

/**
 * @brief 求阶乘
 * @param x 输入值
 * @return x!的结果 
 */
float math_factorial(integer_t x)
{
    
    const float factor[9] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320};
    integer_t i;
    float temp = factor[8];
    // 小于0报错
    if (x < 0)
    {
        math_errno = MATH_PARAMETERS_ERROR;
        return 0;
    }
    // 查表返回
    if (x < 9)
        return factor[x];
    // 递归计算
    for (i = 9; i <= x; i++)
    {
        temp *= i;
    }
    return temp;
}

/**
 * @brief 浮点取模函数
 * @param[in] x 被除数
 * @param[in] y 模值
 * @return x mod y结果
 */
float math_fmod(float x, float y)
{
    return x - (int)(x / y) * y;
}


/**
 * @brief 快速幂
 * @param[in] a 底数
 * @param[in] n 指数(必须为整型)
 * @return a^n的结果
 */
float math_pow(float a, integer_t n)
{
    float res = 1.0f;

    math_assert_param(!IS_NAN_INF(a));
    
    if (n < 0)
        return 1.0f / pow(a, -n);
    // 快速幂
    while (n)
    {
        if (n & 1)
            res *= a;
        a *= a;
        n >>= 1;
    }
    return res;
}

/**
 * @brief 求exp
 * @param[in] x 输入值
 * @return e^x的结果
 */
float math_exp(float x)
{
    return exp(x);
}

// /// @brief 求exp
// /// @param[in] x 输入值
// /// @return e^x的结果
// /// @bug 比标准库慢,吉祥物来着
// float math_exp(float x)
// {
//     const integer_t iterations = 7;
//     integer_t n = (integer_t)x;
//     float e1 = math_pow(e, n), e2;
//     float temp;
//     int i;

//     math_assert_param(!IS_NAN_INF(x));
    
//     if (x < 0)
//         return 1 / math_exp(-x);

//     x -= (float)n;
//     e2 = 1;
//     //为了提高精度，大于0.5进行约化
//     if (x > 0.5f)
//     {
//         temp = x / 2;
//         //泰勒展开
//         for (i = 1; i <= iterations; i++)
//         {
//             e2 += temp / math_factorial(i);
//             temp *= x / 2;
//         }
//         e2 *= e2;
//     }
//     else
//     {
//         temp = x;
//         //泰勒展开
//         for (i = 1; i <= iterations; i++)
//         {
//             e2 += temp / math_factorial(i);
//             temp *= x;
//         }
//     }
//     return e1 * e2;
// }

/**
 * @brief 实数幂函数
 * @param[in] a 底数
 * @param[in] n 指数
 * @return a^n的结果 
 */
float math_powf(float a, float n)
{
    math_assert_param(!IS_NAN_INF(a));
    math_assert_param(!IS_NAN_INF(n));

    if (a == 0)
    {
        if (n == 0)
        {
            math_errno = MATH_PARAMETERS_ERROR;
        }
        return 0;
    }
    return math_exp(n * math_ln(a));
}

/// @brief 负对数拟然求对数
/// @param[in] q 输入,范围[0.01-0.1)
/// @return lnx结果
/// @see https://www.cnblogs.com/skyivben/archive/2013/02/15/2912914.html
static float negative_log(float q)
{
    float r = q, s = q, n = q, q2 = q * q, q1 = q2 * q;
    int p = 1;
    math_assert_param(!IS_NAN_INF(q));

    for (; (n *= q1) > eps1; s += n, q1 *= q2)
    {
        r += (p = !p) ? n : -n;
    }
    float u = 1 - 2 * r, v = 1 + 2 * s, t = u / v;
    float a = 1, b = sqrt(1 - t * t * t * t);
    for (; a - b > eps2; b = sqrt(a * b), a = t)
    {
        t = (a + b) / 2;
    }
    return 2 * PI / (a + b) / v / v;
}

/**
 * @brief 求自然对数
 * @param[in] x 输入值
 * @return lnx结果 
 */
float math_ln(float x)
{
    math_assert_param(!IS_NAN_INF(x));

    if (x <= 0)
    {
        // 输入参数错误
        if(x < 0)
        {
            math_errno = MATH_PARAMETERS_ERROR;
        }
        // INF错误
        else
        {
            math_errno = MATH_NANINF;
        }
        return 0;
    }
    if (x == 1)
    {
        return 0;
    }
    int k = 0;
    for (; x > 0.1; k++)
    {
        x /= 10;
    }
    for (; x <= 0.01; k--)
    {
        x *= 10;
    }
    return k * ln_10 - negative_log(x);
}

/**
 * @brief 求对数
 * @param[in] a 真数
 * @param[in] x 底数
 * @return log_a(x)结果
 */
float math_log(float a,float x)
{
    return math_ln(x)/math_ln(a);
}

#ifdef _ARM_MATH_H
/**
 * @brief 求平方根倒数
 * @param[in] x 输入值
 * @return 1/sqrt(x)结果 
 */
float math_invsqrt(float x)
{
    return 1 / math_sqrt(x);
}

/**
 * @brief 求平方根
 * @param[in] x 输入值
 * @return sqrt(x)结果 
 */
float math_sqrt(float x)
{
    math_assert_param(!IS_NAN_INF(x));

    float temp;
    arm_sqrt_f32(x,&temp);
    return temp;
}
#else
/// @brief 求平方根倒数
/// @param[in] x 输入值
/// @return 1/sqrt(x)结果
float math_invsqrt(float x)
{
    long i;
    float x2, y;

    math_assert_param(!IS_NAN_INF(x));

    if (x < 0)
    {
        math_errno = MATH_PARAMETERS_ERROR;
    }
    
    // 快速平方根倒数
    x2 = x * 0.5F;
    y = x;
    i = *(long *)&y;
    i = 0x5f3759df - (i >> 1);
    y = *(float *)&i;
    y *= 1.5F - (x2 * y * y);
    y *= 1.5F - (x2 * y * y);

    return y;
}

/// @brief 求平方根
/// @param[in] x 输入值
/// @return sqrt(x)结果
float math_sqrt(float x)
{
    math_assert_param(!IS_NAN_INF(x));

    return 1/math_invsqrt(x);
}
#endif
/**
 * 双曲函数
 */

/**
 * @brief 求双曲正弦
 * @param[in] x 输入值
 * @return sinhx的结果
*/
float math_sinh(float x)
{
    math_assert_param(!IS_NAN_INF(x));

    return (math_exp(x) - math_exp(-x)) / 2;
}

/**
 * @brief 求双曲余弦
 * @param[in] x 输入值
 * @return coshx的结果 
 */
float math_cosh(float x)
{
    math_assert_param(!IS_NAN_INF(x));

    return (math_exp(x) + math_exp(-x)) / 2;
}

/**
 * @brief 求双曲正切
 * @param x 输入值
 * @return tanhx的结果 
 */
float math_tanh(float x)
{
    math_assert_param(!IS_NAN_INF(x));

    float ext = exp(x);
    return 1 - 2 / (ext * ext + 1);
}

/**
 * 三角函数
 */
#ifdef _ARM_MATH_H
/**
 * @brief 求正弦函数
 * @param x 输入值(弧度)
 * @return sinx的结果 
 */
float math_sin(float x)
{
    math_assert_param(!IS_NAN_INF(x));

    return arm_sin_f32(x);
}

/**
 * @brief 求余弦函数
 * @param x 输入值(弧度)
 * @return cosx的结果 
 */
float math_cos(float x)
{
    math_assert_param(!IS_NAN_INF(x));

    return arm_cos_f32(x);
}

/**
 * @brief 求sinx和cosx
 * @param[in] x 输入值(弧度)
 * @param[out] sinx sinx结果
 * @param[out] cosx cosx结果
*/
void math_sin_cos(float x, float *sinx, float *cosx)
{
    float temp;
    math_assert_param(!IS_NAN_INF(x));
    math_assert_param(IS_NULL_POINT(sinx));
    math_assert_param(IS_NULL_POINT(cosx));
    // arm_sin_cos_f32输入的是度数，需要对其进行转换
    temp = x / (2.0f * PI) * 360;
    arm_sin_cos_f32(temp, sinx, cosx);
}

#else  //_ARM_MATH_H

/**
 * @brief 求正弦函数
 * @param x 输入值(弧度)
 * @return sinx的结果 
 * @see https://www.cnblogs.com/pgzhang/p/9069623.html
 */
float math_sin(float x)
{
    float n, sum = 0;
	int i = 1;

    math_assert_param(!IS_NAN_INF(x));
    x = x - (int)(x / (2*PI)) * 2*PI;
    if(x>2*PI)
        x -= 2*PI;
    n = x;
    //泰勒展开
	do
	{
		sum += n;
		i++;
		n = -n * x * x / (2 * i - 1) / (2 * i - 2);

	}
	while (abs(n) >= eps2);

	return sum;
}

/**
 * @brief 求余弦函数
 * @param x 输入值(弧度)
 * @return cosx的结果 
 */
float math_cos(float x)
{
    math_assert_param(!IS_NAN_INF(x));

    return math_sin(x + PI / 2.0f);
}

/**
 * @brief 求sinx和cosx
 * @param[in] x 输入值(弧度)
 * @param[out] sinx sinx结果
 * @param[out] cosx cosx结果
*/
void math_sin_cos(float x, float *sinx, float *cosx)
{
    math_assert_param(!IS_NAN_INF(x));
    math_assert_param(IS_NULL_POINT(sinx));
    math_assert_param(IS_NULL_POINT(cosx));

    *sinx = math_sin(x);
    *cosx = math_cos(x);
}

#endif //_ARM_MATH_H

/**
 * @brief 求正切函数
 * @param x 输入值(弧度)
 * @return tanx的结果 
 */
float math_tan(float x)
{
    math_assert_param(!IS_NAN_INF(x));

    float sinx, cosx;
    math_sin_cos(x, &sinx, &cosx);
    if(cosx == 0)
    {
        math_errno = MATH_NANINF;
        return 0;
    }
    return sinx / cosx;
}
/**
 * @brief 正切计算
 * @param[in] x 输入值
 * @return arctan估算值(0.1%误差)
 */
float math_atan(float x)
{
	float absx;
	if (x > 1)
		return PI / 2.0f - math_atan(1 / x);
	else if (x < -1)
		return -(PI / 2.0f - math_atan(-1 / x));
	*(int32_t *)&absx = *(int32_t *)&x & 0x7fffffff;
	return x * ((-0.0663f * absx - 0.1784f) * absx + PI / 4.0f + 0.2447f);
}

/**
 * @brief 正切计算(全象限)
 * @param[in] x cosx值
 * @param[in] y sinx值
 * @return arctan估算值(0.1%误差)
 */
float math_atan2(float y,float x)
{
    float temp = math_atan(y/x);
    if (x>0)  
        return temp;  
    else if (y >= 0 && x < 0)  
        return temp + PI;  
    else if (y < 0 && x < 0)  
        return temp - PI;  
    else if (y > 0 && x == 0)  
        return PI / 2;  
    else if (y < 0 && x == 0)  
        return -1 * PI / 2;  
    else  
        return 0;
}
