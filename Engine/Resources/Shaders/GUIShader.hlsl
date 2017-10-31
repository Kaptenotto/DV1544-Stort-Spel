struct VSIn
{
    float2 pos : POSITION;
    float2 uv : TEXCOORD;
    uint elm : ELEMENT;
};

struct VSOut
{
    float4 pos : SV_POSITION;
    float2 uv : UV;
    uint elm : ELEMENT;
};

cbuffer hudInfo : register(b0)
{
    float hpProcent;
    float offset0;
    float offset1;
    float offset2;
    float offset3;
    float offset4;
}

cbuffer positionOffset : register(b1)
{
    float2 offset;
}

VSOut VS(VSIn vsin)
{
    VSOut vsout;

    vsout.pos = float4(vsin.pos, 0, 1);

    if (vsin.elm == 2)
    {

        if (vsin.uv.x > 0.5f)
        {
            vsin.uv.x -= (vsin.uv.x * hpProcent );
            vsin.pos.x -= (abs(-1.0f - vsin.pos.x) * hpProcent);
        }
        vsout.uv = vsin.uv;
        vsout.pos = float4(vsin.pos, 0.f, 1.f);

    }
    else if (vsin.elm == 3)
    {
        vsout.uv = vsin.uv;
        vsout.uv.y += offset0;
    }
    else if (vsin.elm == 4)
    {
        vsout.uv = vsin.uv;
        vsout.uv.y += offset1;
    }
    else if (vsin.elm == 5)
    {
        vsout.uv = vsin.uv;
        vsout.uv.y += offset2;
    }
    else if (vsin.elm == 6)
    {
        vsout.uv = vsin.uv;
        vsout.uv.y += offset3;
    }
    else if (vsin.elm == 7)
    {
        vsout.uv = vsin.uv;
        vsout.uv.y += offset4;
    }
    else
    {
        vsout.uv = vsin.uv;
    }
        
    

    vsout.elm = vsin.elm;


    vsout.pos.xy += offset.xy * min(vsout.elm, 1);



    return vsout;
}

Texture2D crosshair : register(t0);
Texture2D HP : register(t1);
Texture2D outline : register(t2);
Texture2D skillbar : register(t3);

SamplerState sState;

float4 PS(VSOut psin) : SV_Target0
{
    if (psin.elm == 0)
    {
        //return float4(1.0f, 0.0f, 0.0f, 0.0f);
        return float4(crosshair.Sample(sState, psin.uv));

    }
    else if (psin.elm == 1)
    {
        //return float4(0.0f, 1.0f, 0.0f, 1.0f);
        //return float4(HP.Sample(sState, psin.uv).xyz, 0.0f);
        return float4(HP.Sample(sState, psin.uv));
    }
    else if (psin.elm == 2)
    {
        return float4(HP.Sample(sState, psin.uv));
    }else
    {
        return float4(skillbar.Sample(sState, psin.uv));
        //return float4(psin.uv.x, 0.0f, 0.0f, 1.0f);
    }

    //return float4(HP.Sample(sState, psin.uv));
};