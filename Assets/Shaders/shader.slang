static float3 colors[6] = float3[](
    float3(1.0, 0.2, 0.1), float3(0.1, 1.0, 0.2), float3(0.1, 0.2, 1.0),
    float3(0.1, 0.2, 1.0), float3(0.1, 1.0, 0.2), float3(1.0, 0.2, 0.1), );

static float2 positions[6] =
    float2[](float2(-0.5, -0.5), float2(0.5, -0.5), float2(-0.5, 0.5),
             float2(-0.5, 0.5), float2(0.5, -0.5), float2(0.5, 0.5));

cbuffer GlobalUBO : register(b0)
{
    float iTime;
};

float rand(float n) { return frac(sin(n) * 43758.5453); }

struct VertexOutput {
  float3 color;
  float2 uv;
  float4 sv_position : SV_Position;
};

[shader("vertex")]
VertexOutput vertMain(uint vid: SV_VertexID) {
  VertexOutput output;
  output.sv_position = float4(positions[vid], 0.0, 1.0);
  output.color = colors[vid];

  output.uv = positions[vid] + 0.5;

  return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVert) : SV_Target {
  float2 uv = inVert.uv;

  float3 col = float3(inVert.color.y * abs(sin(iTime)), inVert.color.r * abs(sin(iTime)), inVert.color.z * abs(sin(iTime)));

float n = dot(uv, float2(1217.9898, 180.288));
  float r = rand(n + 0.5);

  // Interesting patterns
  // col = lerp(col, float3(r, 1.0-r, sin(r*3.14)), 0.5);
  col = lerp(col, float3(sinpi(iTime * r)), 0.8);

  return float4(col, 1.0);
}
