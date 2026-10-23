// Super Mario Bros. by HLorenzi
// GLSL -> HLSL 変換

// Uncomment for totally random level!
// Just to show off Mario's dynamic movements :P
//#define TOTALLY_RANDOM_LEVEL 1

// Positions to start and end Mario simulation (relative to screen position)
// You can try changing these! (the difference between the two should be a multiple of 4)
// Put startX too close to endX, and Mario'll experience jittering!
#define startX  0.0f
#define endX   80.0f

struct PSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

struct PSOutput
{
    float4 Color : SV_TARGET;
};

cbuffer Time : register(b0)
{
    float iTime;
};

cbuffer Resolution : register(b1)
{
    float2 iResolution; // 描画先RTのピクセルサイズ
};

// GLSL の mod は floor 基準(負でも 0〜y を返す)。HLSL の fmod は切り捨て基準で負の数で結果が変わるので自作する
// (画面下端は y がマイナスになるので、fmod だと地面の模様が崩れる)
float mod(float x, float y)
{
    return x - y * floor(x / y);
}

#define RGB(r,g,b) float4(float(r)/255.0f,float(g)/255.0f,float(b)/255.0f,1.0f)

#define SPRROW(x,a,b,c,d,e,f,g,h, i,j,k,l,m,n,o,p) (x <= 7 ? SPRROW_H(a,b,c,d,e,f,g,h) : SPRROW_H(i,j,k,l,m,n,o,p))
#define SPRROW_H(a,b,c,d,e,f,g,h) (a+4.0f*(b+4.0f*(c+4.0f*(d+4.0f*(e+4.0f*(f+4.0f*(g+4.0f*(h))))))))
#define SECROW(x,a,b,c,d,e,f,g,h) (x <= 3 ? SECROW_H(a,b,c,d) : SECROW_H(e,f,g,h))
#define SECROW_H(a,b,c,d) (a+8.0f*(b+8.0f*(c+8.0f*(d))))
#define SELECT(x,i) mod(floor(i/pow(4.0f,float(x))),4.0f)
#define SELECTSEC(x,i) mod(floor(i/pow(8.0f,float(x))),8.0f)

float rand(float2 co)
{
    return frac(sin(dot(co.xy, float2(12.9898f, 78.233f))) * 43758.5453f);
}

float4 sprGround(int x, int y)
{
    float col = 0.0f;
    if (y == 15)
        col = SPRROW(x, 1., 0., 0., 0., 0., 0., 0., 0., 0., 2., 1., 0., 0., 0., 0., 1.);
    if (y == 14)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 2., 0., 1., 1., 1., 1., 2.);
    if (y == 13)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 2., 0., 1., 1., 1., 1., 2.);
    if (y == 12)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 2., 0., 1., 1., 1., 1., 2.);
    if (y == 11)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 2., 0., 2., 1., 1., 1., 2.);
    if (y == 10)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 2., 1., 2., 2., 2., 2., 1.);
    if (y == 9)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 2., 0., 0., 0., 0., 0., 2.);
    if (y == 8)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 2., 0., 1., 1., 1., 1., 2.);

    if (y == 7)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 2., 0., 1., 1., 1., 1., 2.);
    if (y == 6)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 2., 0., 1., 1., 1., 1., 2.);
    if (y == 5)
        col = SPRROW(x, 2., 2., 1., 1., 1., 1., 1., 1., 2., 0., 1., 1., 1., 1., 1., 2.);
    if (y == 4)
        col = SPRROW(x, 0., 0., 2., 2., 1., 1., 1., 1., 2., 0., 1., 1., 1., 1., 1., 2.);
    if (y == 3)
        col = SPRROW(x, 0., 1., 0., 0., 2., 2., 2., 2., 0., 1., 1., 1., 1., 1., 1., 2.);
    if (y == 2)
        col = SPRROW(x, 0., 1., 1., 1., 0., 0., 0., 2., 0., 1., 1., 1., 1., 1., 1., 2.);
    if (y == 1)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 2., 0., 1., 1., 1., 1., 1., 2., 2.);
    if (y == 0)
        col = SPRROW(x, 1., 2., 2., 2., 2., 2., 2., 1., 0., 2., 2., 2., 2., 2., 2., 1.);

    col = SELECT(mod(float(x), 8.0f), col);
    if (col == 0.0f)
        return RGB(247, 214, 181);
    if (col == 1.0f)
        return RGB(231, 90, 16);
    return RGB(0, 0, 0);
}

float4 sprQuestionBlock(int x, int y)
{
    float col = 0.0f;
    if (y == 15)
        col = SPRROW(x, 3., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 3.);
    if (y == 14)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 2.);
    if (y == 13)
        col = SPRROW(x, 0., 1., 2., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 2., 1., 2.);
    if (y == 12)
        col = SPRROW(x, 0., 1., 1., 1., 1., 0., 0., 0., 0., 0., 1., 1., 1., 1., 1., 2.);
    if (y == 11)
        col = SPRROW(x, 0., 1., 1., 1., 0., 0., 2., 2., 2., 0., 0., 1., 1., 1., 1., 2.);
    if (y == 10)
        col = SPRROW(x, 0., 1., 1., 1., 0., 0., 2., 1., 1., 0., 0., 2., 1., 1., 1., 2.);
    if (y == 9)
        col = SPRROW(x, 0., 1., 1., 1., 0., 0., 2., 1., 1., 0., 0., 2., 1., 1., 1., 2.);
    if (y == 8)
        col = SPRROW(x, 0., 1., 1., 1., 1., 2., 2., 1., 0., 0., 0., 2., 1., 1., 1., 2.);

    if (y == 7)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 0., 0., 2., 2., 2., 1., 1., 1., 2.);
    if (y == 6)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 0., 0., 2., 1., 1., 1., 1., 1., 2.);
    if (y == 5)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 2., 2., 1., 1., 1., 1., 1., 2.);
    if (y == 4)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 0., 0., 0., 1., 1., 1., 1., 1., 2.);
    if (y == 3)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 0., 0., 2., 1., 1., 1., 1., 1., 2.);
    if (y == 2)
        col = SPRROW(x, 0., 1., 2., 1., 1., 1., 1., 1., 2., 2., 1., 1., 1., 2., 1., 2.);
    if (y == 1)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 2.);
    if (y == 0)
        col = SPRROW(x, 2., 2., 2., 2., 2., 2., 2., 2., 2., 2., 2., 2., 2., 2., 2., 2.);

    if (y < 0 || y > 15)
        return RGB(107, 140, 255);

    col = SELECT(mod(float(x), 8.0f), col);
    if (col == 0.0f)
        return RGB(231, 90, 16);
    if (col == 1.0f)
        return RGB(255, 165, 66);
    if (col == 2.0f)
        return RGB(0, 0, 0);
    return RGB(107, 140, 255);
}

float4 sprUsedBlock(int x, int y)
{
    float col = 0.0f;
    if (y == 15)
        col = SPRROW(x, 3., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 3.);
    if (y == 14)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);
    if (y == 13)
        col = SPRROW(x, 0., 1., 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0., 1., 0.);
    if (y == 12)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);
    if (y == 11)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);
    if (y == 10)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);
    if (y == 9)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);
    if (y == 8)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);

    if (y == 7)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);
    if (y == 6)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);
    if (y == 5)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);
    if (y == 4)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);
    if (y == 3)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);
    if (y == 2)
        col = SPRROW(x, 0., 1., 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0., 1., 0.);
    if (y == 1)
        col = SPRROW(x, 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0.);
    if (y == 0)
        col = SPRROW(x, 3., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 3.);

    if (y < 0 || y > 15)
        return RGB(107, 140, 255);

    col = SELECT(mod(float(x), 8.0f), col);
    if (col == 0.0f)
        return RGB(0, 0, 0);
    if (col == 1.0f)
        return RGB(231, 90, 16);
    return RGB(107, 140, 255);
}

float4 sprMarioJump(int x, int y)
{
    float col = 0.0f;
    if (y == 15)
        col = SPRROW(x, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 2., 2., 2.);
    if (y == 14)
        col = SPRROW(x, 0., 0., 0., 0., 0., 0., 1., 1., 1., 1., 1., 0., 0., 2., 2., 2.);
    if (y == 13)
        col = SPRROW(x, 0., 0., 0., 0., 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 2., 2.);
    if (y == 12)
        col = SPRROW(x, 0., 0., 0., 0., 0., 3., 3., 3., 2., 2., 3., 2., 0., 3., 3., 3.);
    if (y == 11)
        col = SPRROW(x, 0., 0., 0., 0., 3., 2., 3., 2., 2., 2., 3., 2., 2., 3., 3., 3.);
    if (y == 10)
        col = SPRROW(x, 0., 0., 0., 0., 3., 2., 3., 3., 2., 2., 2., 3., 2., 2., 2., 3.);
    if (y == 9)
        col = SPRROW(x, 0., 0., 0., 0., 3., 3., 2., 2., 2., 2., 3., 3., 3., 3., 3., 0.);
    if (y == 8)
        col = SPRROW(x, 0., 0., 0., 0., 0., 0., 2., 2., 2., 2., 2., 2., 2., 3., 0., 0.);

    if (y == 7)
        col = SPRROW(x, 0., 0., 3., 3., 3., 3., 3., 1., 3., 3., 3., 1., 3., 0., 0., 0.);
    if (y == 6)
        col = SPRROW(x, 0., 3., 3., 3., 3., 3., 3., 3., 1., 3., 3., 3., 1., 0., 0., 3.);
    if (y == 5)
        col = SPRROW(x, 2., 2., 3., 3., 3., 3., 3., 3., 1., 1., 1., 1., 1., 0., 0., 3.);
    if (y == 4)
        col = SPRROW(x, 2., 2., 2., 0., 1., 1., 3., 1., 1., 2., 1., 1., 2., 1., 3., 3.);
    if (y == 3)
        col = SPRROW(x, 0., 2., 0., 3., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 3., 3.);
    if (y == 2)
        col = SPRROW(x, 0., 0., 3., 3., 3., 1., 1., 1., 1., 1., 1., 1., 1., 1., 3., 3.);
    if (y == 1)
        col = SPRROW(x, 0., 3., 3., 3., 1., 1., 1., 1., 1., 1., 1., 0., 0., 0., 0., 0.);
    if (y == 0)
        col = SPRROW(x, 0., 3., 0., 0., 1., 1., 1., 1., 0., 0., 0., 0., 0., 0., 0., 0.);

    col = SELECT(mod(float(x), 8.0f), col);
    if (col == 0.0f)
        return RGB(0, 0, 0);
    if (col == 1.0f)
        return RGB(177, 52, 37);
    if (col == 2.0f)
        return RGB(227, 157, 37);
    if (col == 3.0f)
        return RGB(106, 107, 4);
    return RGB(0, 0, 0);
}

float4 sprMarioWalk3(int x, int y)
{
    float col = 0.0f;
    if (y == 15)
        col = SPRROW(x, 0., 0., 0., 0., 0., 1., 1., 1., 1., 1., 0., 0., 0., 0., 0., 0.);
    if (y == 14)
        col = SPRROW(x, 0., 0., 0., 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0., 0., 0.);
    if (y == 13)
        col = SPRROW(x, 0., 0., 0., 0., 3., 3., 3., 2., 2., 3., 2., 0., 0., 0., 0., 0.);
    if (y == 12)
        col = SPRROW(x, 0., 0., 0., 3., 2., 3., 2., 2., 2., 3., 2., 2., 2., 0., 0., 0.);
    if (y == 11)
        col = SPRROW(x, 0., 0., 0., 3., 2., 3., 3., 2., 2., 2., 3., 2., 2., 2., 0., 0.);
    if (y == 10)
        col = SPRROW(x, 0., 0., 0., 3., 3., 2., 2., 2., 2., 3., 3., 3., 3., 0., 0., 0.);
    if (y == 9)
        col = SPRROW(x, 0., 0., 0., 0., 0., 2., 2., 2., 2., 2., 2., 2., 0., 0., 0., 0.);
    if (y == 8)
        col = SPRROW(x, 0., 0., 3., 3., 3., 3., 1., 1., 3., 3., 0., 0., 0., 0., 0., 0.);

    if (y == 7)
        col = SPRROW(x, 2., 2., 3., 3., 3., 3., 1., 1., 1., 3., 3., 3., 2., 2., 2., 0.);
    if (y == 6)
        col = SPRROW(x, 2., 2., 2., 0., 3., 3., 1., 2., 1., 1., 1., 3., 3., 2., 2., 0.);
    if (y == 5)
        col = SPRROW(x, 2., 2., 0., 0., 1., 1., 1., 1., 1., 1., 1., 0., 0., 3., 0., 0.);
    if (y == 4)
        col = SPRROW(x, 0., 0., 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 3., 3., 0., 0.);
    if (y == 3)
        col = SPRROW(x, 0., 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 1., 3., 3., 0., 0.);
    if (y == 2)
        col = SPRROW(x, 0., 3., 3., 1., 1., 1., 0., 0., 0., 1., 1., 1., 3., 3., 0., 0.);
    if (y == 1)
        col = SPRROW(x, 0., 3., 3., 3., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.);
    if (y == 0)
        col = SPRROW(x, 0., 0., 3., 3., 3., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.);

    col = SELECT(mod(float(x), 8.0f), col);
    if (col == 0.0f)
        return RGB(0, 0, 0);
    if (col == 1.0f)
        return RGB(177, 52, 37);
    if (col == 2.0f)
        return RGB(227, 157, 37);
    if (col == 3.0f)
        return RGB(106, 107, 4);
    return RGB(0, 0, 0);
}


float4 sprMarioWalk2(int x, int y)
{
    float col = 0.0f;
    if (y == 15)
        col = SPRROW(x, 0., 0., 0., 0., 0., 1., 1., 1., 1., 1., 0., 0., 0., 0., 0., 0.);
    if (y == 14)
        col = SPRROW(x, 0., 0., 0., 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0., 0., 0.);
    if (y == 13)
        col = SPRROW(x, 0., 0., 0., 0., 3., 3., 3., 2., 2., 3., 2., 0., 0., 0., 0., 0.);
    if (y == 12)
        col = SPRROW(x, 0., 0., 0., 3., 2., 3., 2., 2., 2., 3., 2., 2., 2., 0., 0., 0.);
    if (y == 11)
        col = SPRROW(x, 0., 0., 0., 3., 2., 3., 3., 2., 2., 2., 3., 2., 2., 2., 0., 0.);
    if (y == 10)
        col = SPRROW(x, 0., 0., 0., 3., 3., 2., 2., 2., 2., 3., 3., 3., 3., 0., 0., 0.);
    if (y == 9)
        col = SPRROW(x, 0., 0., 0., 0., 0., 2., 2., 2., 2., 2., 2., 2., 0., 0., 0., 0.);
    if (y == 8)
        col = SPRROW(x, 0., 0., 0., 0., 3., 3., 1., 3., 3., 3., 0., 0., 0., 0., 0., 0.);

    if (y == 7)
        col = SPRROW(x, 0., 0., 0., 3., 3., 3., 3., 1., 1., 3., 3., 0., 0., 0., 0., 0.);
    if (y == 6)
        col = SPRROW(x, 0., 0., 0., 3., 3., 3., 1., 1., 2., 1., 1., 2., 0., 0., 0., 0.);
    if (y == 5)
        col = SPRROW(x, 0., 0., 0., 3., 3., 3., 3., 1., 1., 1., 1., 1., 0., 0., 0., 0.);
    if (y == 4)
        col = SPRROW(x, 0., 0., 0., 1., 3., 3., 2., 2., 2., 1., 1., 1., 0., 0., 0., 0.);
    if (y == 3)
        col = SPRROW(x, 0., 0., 0., 0., 1., 3., 2., 2., 1., 1., 1., 0., 0., 0., 0., 0.);
    if (y == 2)
        col = SPRROW(x, 0., 0., 0., 0., 0., 1., 1., 1., 3., 3., 3., 0., 0., 0., 0., 0.);
    if (y == 1)
        col = SPRROW(x, 0., 0., 0., 0., 0., 3., 3., 3., 3., 3., 3., 3., 0., 0., 0., 0.);
    if (y == 0)
        col = SPRROW(x, 0., 0., 0., 0., 0., 3., 3., 3., 3., 0., 0., 0., 0., 0., 0., 0.);

    col = SELECT(mod(float(x), 8.0f), col);
    if (col == 0.0f)
        return RGB(0, 0, 0);
    if (col == 1.0f)
        return RGB(177, 52, 37);
    if (col == 2.0f)
        return RGB(227, 157, 37);
    if (col == 3.0f)
        return RGB(106, 107, 4);
    return RGB(0, 0, 0);
}


float4 sprMarioWalk1(int x, int y)
{
    float col = 0.0f;
    if (y == 15)
        col = SPRROW(x, 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.);
    if (y == 14)
        col = SPRROW(x, 0., 0., 0., 0., 0., 0., 1., 1., 1., 1., 1., 0., 0., 0., 0., 0.);
    if (y == 13)
        col = SPRROW(x, 0., 0., 0., 0., 0., 1., 1., 1., 1., 1., 1., 1., 1., 1., 0., 0.);
    if (y == 12)
        col = SPRROW(x, 0., 0., 0., 0., 0., 3., 3., 3., 2., 2., 3., 2., 0., 0., 0., 0.);
    if (y == 11)
        col = SPRROW(x, 0., 0., 0., 0., 3., 2., 3., 2., 2., 2., 3., 2., 2., 2., 0., 0.);
    if (y == 10)
        col = SPRROW(x, 0., 0., 0., 0., 3., 2., 3., 3., 2., 2., 2., 3., 2., 2., 2., 0.);
    if (y == 9)
        col = SPRROW(x, 0., 0., 0., 0., 3., 3., 2., 2., 2., 2., 3., 3., 3., 3., 0., 0.);
    if (y == 8)
        col = SPRROW(x, 0., 0., 0., 0., 0., 0., 2., 2., 2., 2., 2., 2., 2., 0., 0., 0.);

    if (y == 7)
        col = SPRROW(x, 0., 0., 0., 0., 0., 3., 3., 3., 3., 1., 3., 0., 2., 0., 0., 0.);
    if (y == 6)
        col = SPRROW(x, 0., 0., 0., 0., 2., 3., 3., 3., 3., 3., 3., 2., 2., 2., 0., 0.);
    if (y == 5)
        col = SPRROW(x, 0., 0., 0., 2., 2., 1., 3., 3., 3., 3., 3., 2., 2., 0., 0., 0.);
    if (y == 4)
        col = SPRROW(x, 0., 0., 0., 3., 3., 1., 1., 1., 1., 1., 1., 1., 0., 0., 0., 0.);
    if (y == 3)
        col = SPRROW(x, 0., 0., 0., 3., 1., 1., 1., 1., 1., 1., 1., 1., 0., 0., 0., 0.);
    if (y == 2)
        col = SPRROW(x, 0., 0., 3., 3., 1., 1., 1., 0., 1., 1., 1., 0., 0., 0., 0., 0.);
    if (y == 1)
        col = SPRROW(x, 0., 0., 3., 0., 0., 0., 0., 3., 3., 3., 0., 0., 0., 0., 0., 0.);
    if (y == 0)
        col = SPRROW(x, 0., 0., 0., 0., 0., 0., 0., 3., 3., 3., 3., 0., 0., 0., 0., 0.);

    col = SELECT(mod(float(x), 8.0f), col);
    if (col == 0.0f)
        return RGB(0, 0, 0);
    if (col == 1.0f)
        return RGB(177, 52, 37);
    if (col == 2.0f)
        return RGB(227, 157, 37);
    if (col == 3.0f)
        return RGB(106, 107, 4);
    return RGB(0, 0, 0);
}

float4 getTile(int t, int x, int y)
{
    if (t == 0)
        return RGB(107, 140, 255);
    if (t == 1)
        return sprGround(x, y);
    if (t == 2)
        return sprQuestionBlock(x, y);
    if (t == 3)
        return sprUsedBlock(x, y);

    return RGB(107, 140, 255);
}

int getSection(int s, int x, int y)
{
    float col = 0.0f;
    if (s == 0)
    {
        if (y == 6)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 5)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 4)
            col = SECROW(x, 0., 0., 3., 3., 3., 0., 0., 0.);
        if (y == 3)
            col = SECROW(x, 0., 0., 2., 2., 2., 0., 0., 0.);
        if (y == 2)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 1)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y <= 0)
            col = SECROW(x, 1., 1., 1., 1., 1., 1., 1., 1.);
    }
    if (s == 1)
    {
        if (y == 6)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 5)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 4)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 3)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 2)
            col = SECROW(x, 0., 0., 0., 0., 0., 1., 0., 0.);
        if (y == 1)
            col = SECROW(x, 0., 0., 0., 1., 1., 1., 0., 0.);
        if (y <= 0)
            col = SECROW(x, 1., 1., 1., 1., 1., 1., 1., 1.);
    }
    if (s == 2)
    {
        if (y == 6)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 5)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 4)
            col = SECROW(x, 0., 0., 3., 0., 0., 3., 0., 0.);
        if (y == 3)
            col = SECROW(x, 0., 0., 2., 0., 0., 2., 0., 0.);
        if (y == 2)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 1)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y <= 0)
            col = SECROW(x, 1., 1., 1., 1., 1., 1., 1., 1.);
    }
    if (s == 3)
    {
        if (y == 6)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 5)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 4)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 3)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 2)
            col = SECROW(x, 0., 0., 0., 1., 1., 0., 0., 0.);
        if (y == 1)
            col = SECROW(x, 0., 0., 0., 1., 1., 1., 0., 0.);
        if (y <= 0)
            col = SECROW(x, 1., 1., 1., 1., 1., 1., 1., 1.);
    }
    if (s == 4)
    {
        if (y == 6)
            col = SECROW(x, 0., 0., 0., 0., 3., 0., 0., 0.);
        if (y == 5)
            col = SECROW(x, 0., 0., 0., 0., 2., 0., 0., 0.);
        if (y == 4)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 3)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 2)
            col = SECROW(x, 0., 0., 0., 1., 1., 1., 0., 0.);
        if (y == 1)
            col = SECROW(x, 0., 0., 0., 1., 1., 1., 0., 0.);
        if (y <= 0)
            col = SECROW(x, 1., 1., 1., 1., 1., 1., 1., 1.);
    }
    if (s == 5)
    {
        if (y == 6)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 5)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 4)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 3)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 2)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y == 1)
            col = SECROW(x, 0., 0., 0., 0., 0., 0., 0., 0.);
        if (y <= 0)
            col = SECROW(x, 1., 1., 1., 0., 0., 1., 1., 1.);
    }

    return int(SELECTSEC(mod(float(x), 4.0f), col));
}

int getBlock(int x, int y)
{
#ifdef TOTALLY_RANDOM_LEVEL
    int height = 1 + int(rand(float2(int(float(x) / 3.0f), 2.3f)) * 3.0f);
    return (y < height ? 1 : 0);
#else
    if (y > 6)
        return 0;

    int section = int(rand(float2(int(float(x) / 8.0f), 3.0f)) * 6.0f);
    int sectionX = int(mod(float(x), 8.0f));

    return getSection(section, sectionX, y - int(rand(float2(section, 2.0f)) * 0.0f));
#endif
}

bool isSolid(int b)
{
    return (b != 0);
}

// sRGB → リニア(RTが _SRGB なので、書き込み時の自動変換と打ち消し合わせる)
float3 SRGBToLinear(float3 c)
{
    return lerp(c / 12.92f, pow((c + 0.055f) / 1.055f, 2.4f), step(0.04045f, c));
}

PSOutput main(PSInput input)
{
    PSOutput output;

    // Shadertoy の fragCoord は「左下原点・ピクセル単位」なので、左上原点の TexCoord を上下反転して作る
    float2 fragCoord = float2(input.TexCoord.x, 1.0f - input.TexCoord.y) * iResolution;

    float4 fragColor;

    const float gameSpeed = 60.0f;

    // Get the current game pixel
    // (Each game pixel is two screen pixels)
    //  (or four, if the screen is larger)
    float x = fragCoord.x / 2.0f;
    float y = fragCoord.y / 2.0f;
    if (iResolution.y >= 640.0f)
    {
        x /= 2.0f;
        y /= 2.0f;
    }
    if (iResolution.y < 200.0f)
    {
        x *= 2.0f;
        y *= 2.0f;
    }

    // Just move the screen up for half a block's size
    y -= 8.0f;

    // Get the grid index of the block at this pixel,
    // and of the block at the screen's leftmost position
    int firstBlockX = int((iTime * gameSpeed) / 16.0f);
    int blockX = int((x + iTime * gameSpeed) / 16.0f);
    int blockY = int(y / 16.0f);

    // Ask for the block ID that exists in the current position
    int block = getBlock(blockX, blockY);

    // Get the fractional position inside current block
    int subx = int(mod((x + iTime * gameSpeed), 16.0f));
    int suby = int(mod(y, 16.0f));

    // Animate block if it's a Question Block
    if (block == 2)
    {
        if (blockX - firstBlockX == 5)
        {
            suby -= int(max(0.0f, (sin(mod((iTime * gameSpeed / 16.0f), 1.0f) * 3.141592f * 1.5f) * 8.0f)));
        }

        if ((floor((x + iTime * gameSpeed) / 16.0f) - (iTime * gameSpeed) / 16.0f) < 4.25f)
            block = 3;
    // Animate block if it's on top of a Question Block
    }
    else if (block == 3)
    {
        block = 2;
        suby += 16;
        if (blockX - firstBlockX == 5)
        {
            suby -= int(max(0.0f, (sin(mod((iTime * gameSpeed / 16.0f), 1.0f) * 3.141592f * 1.5f) * 8.0f)));
        }
    }
    // Get the final color for this pixel
    // (Mario can override this later on)
    fragColor = getTile(block, subx, suby);


    // If this is the column where Mario stops simulating...
    // (it's the only column he can appear in)
    if (x >= endX && x < endX + 16.0f)
    {

        // Screen position in pixels:
        // Every block is 16 pixels wide
        float screenX = iTime * gameSpeed;

        // Mario's starting position and speed
        float marioX = screenX + startX;
        float marioY = 16.0f;
        float marioXSpd = 4.0f;
        float marioYSpd = 0.0f;

        // Find out the first empty block in this column,
        // starting from the bottom, as to put Mario on top of it
        for (int i = 1; i < 4; i++)
        {
            if (!isSolid(getBlock(int(marioX / 16.0f), i)))
            {
                marioY = float(i) * 16.0f;
                break;
            }
        }

        // Number of steps to simulate;
        // We'll simulate at 15 FPS and interpolate later,
        // hence the division by 4.0
        // (Mario should actually be walking 1 pixel every 1/60th of a second,
        //  but he'll be walking 4 pixels every 1/15th)
        const int simSteps = int((endX - startX) / 4.0f);

        // Previous position, as to interpolate later, for high frame rates
        float lastX = 0.0f;
        float lastY = 0.0f;

        // Start simulating
        bool onGround = false;
        for (int sim = 0; sim < simSteps; sim++)
        {
            // Store the previous position
            lastX = marioX;
            lastY = marioY;

            // If Mario is inside a block, move him up
            // (This happens only at the start of the simulation,
            //  sometimes because he is heads-up with a wall and
            //  cannot make a jump properly)
            onGround = false;
            if (isSolid(getBlock(int(marioX / 16.0f) + 1, int(marioY / 16.0f))))
            {
                marioY = (floor(marioY / 16.0f) * 16.0f) + 16.0f;
            }

            // Next, pretty standard platforming code

            // Apply gravity and move in the Y-axis
            marioYSpd -= 2.5f;
            marioY += marioYSpd;

            // If he is going up,
            // and if there is a block above him,
            // align him with the grid (as to avoid getting inside the block),
            // and invert his YSpeed, as to fall quickly (because he bounced his head)
            if (marioYSpd > 0.0f)
            {
                if (isSolid(getBlock(int(floor((marioX + 12.0f) / 16.0f)), int(floor((marioY + 15.9f) / 16.0f)))))
                {
                    marioYSpd *= -0.5f;
                    marioY = (floor(marioY / 16.0f) * 16.0f);
                }
            }

            // If he is going down,
            // and if there is a block beneath him,
            // align him with the grid (as to land properly on top of the block),
            // and mark him as onGround (to be able to perform a jump)
            if (marioYSpd < 0.0f)
            {
                if (isSolid(getBlock(int(floor((marioX) / 16.0f)), int(floor(marioY / 16.0f)))) ||
                    isSolid(getBlock(int(floor((marioX + 15.9f) / 16.0f)), int(floor(marioY / 16.0f)))))
                {
                    marioYSpd = 0.0f;
                    marioY = (floor(marioY / 16.0f) * 16.0f) + 16.0f;
                    onGround = true;
                }
            }

            // Finally, move him in the X-axis
            // I assume here he'll never hit a block horizontally
            marioX += marioXSpd;

            // Now, if he's onGround,
            // and if there are blocks in front of him,
            // or if there is a pit right next to him,
            // set his YSpeed to jump
            if (onGround)
            {
                if (!isSolid(getBlock(int((marioX) / 16.0f) + 1, 0)))
                {
                    marioYSpd = 15.5f;
                }
                else if (isSolid(getBlock(int((marioX + 36.0f) / 16.0f), int((marioY + 24.0f) / 16.0f))))
                {
                    marioYSpd = 15.5f;
                }
                else if (isSolid(getBlock(int((marioX) / 16.0f) + 2, int((marioY + 8.0f) / 16.0f))))
                {
                    marioYSpd = 12.5f;
                }
                else if (getBlock(int((marioX) / 16.0f) + 1, int((marioY + 8.0f) / 16.0f) + 2) == 2)
                {
                    marioYSpd = 15.5f;
                }

            }
        }

        // Interpolate Y-pos for smooth high-frame-rate movement
        marioY = lerp(lastY, marioY, mod(iTime * 15.0f, 1.0f)) - 1.0f;

        // Finally, if he appears at this row, fetch a pixel from his sprites
        if (y >= marioY && y < marioY + 16.0f)
        {
            float4 spr = float4(0.0f, 0.0f, 0.0f, 0.0f);
            if (onGround)
            {
                // Which frame?
                int f = int(mod(iTime * 10.0f, 3.0f));
                if (f == 0)
                    spr = sprMarioWalk1(int(x - (marioX - screenX)), int(y - marioY));
                if (f == 1)
                    spr = sprMarioWalk2(int(x - (marioX - screenX)), int(y - marioY));
                if (f == 2)
                    spr = sprMarioWalk3(int(x - (marioX - screenX)), int(y - marioY));
            }
            else
            {
                spr = sprMarioJump(int(x - (marioX - screenX)), int(y - marioY));
            }
            // Transparency check
            if (spr.x != 0.0f)
                fragColor = spr;
        }
    }

    // ドット絵の色(sRGBの値そのもの)が画面にそのまま出るよう、リニアに戻してから書く
    output.Color = float4(SRGBToLinear(fragColor.rgb), 1.0f);
    return output;
}