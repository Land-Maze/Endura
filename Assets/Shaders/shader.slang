cbuffer GlobalUBO : register(b0)
{
    float iTime;
};

float rand(float n) { return frac(sin(n) * 43758.5453); }

struct VSInput {
    float2 inPosition;
    float3 inColor;
};

struct VertexOutput {
  float3 color;
  float2 uv;
  float4 pos : SV_Position;
};

[shader("vertex")]
VertexOutput vertMain(VSInput input) {
  VertexOutput output;
  output.pos = float4(input.inPosition, 0.0, 1.0);
  output.color = input.inColor;

  output.uv = input.inPosition + 0.5;

  return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
  float2 uv = inVert.uv;

  float3 col = float3(inVert.color.r * abs(tanh(iTime)), inVert.color.g * abs(tanh(iTime)), inVert.color.b * abs(tanh(iTime)));

  float n = dot(uv, float2(117.9898, 180.288));
  float r = rand(n + 0.5);

  // Interesting patterns
  // col = lerp(col, float3(r, 1.0-r, sin(r*3.14)), 0.5);
  col = lerp(col, float3(sinpi(iTime * r)), 0.1);

  return float4(col, 1.0);
}
