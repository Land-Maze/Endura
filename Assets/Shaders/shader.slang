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

struct UniformBuffer {
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};
ConstantBuffer<UniformBuffer> ubo;

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

  float3 col = float3(inVert.color.r * abs(tanh(uv.r)), inVert.color.g * abs(tanh(uv.g)), inVert.color.b * abs(tanh(1 - uv.r)));

  float n = dot(uv, float2(117.9898, 180.288));
  float r = rand(n + 0.5);

  col = lerp(col, float3(sinpi(r)), 0.1);

  return float4(col, 1.0);
}
