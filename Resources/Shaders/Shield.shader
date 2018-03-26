
#include "Common.inc.shader"
#include "UniformBuffers.inc.shader"

#ifdef VERTEX_SHADER

layout(location = 0) in vec4 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec4 _tangent;
layout(location = 3) in vec2 _texcoord;

out vec4 worldPosition;
out vec3 worldNormal;
out vec2 texcoord;

void main()
{
    worldPosition = _LocalToWorld * _position;
    worldNormal = normalize(mat3(_LocalToWorld) * _normal);
    texcoord = _texcoord * 4.0;

    gl_Position = _ViewProjectionMatrix * worldPosition;
}

#endif


#ifdef FRAGMENT_SHADER

layout(binding = 6) uniform sampler2D _ShieldFlowTexture;
layout(binding = 7) uniform sampler2D _ShieldOpacityTexture;

in vec4 worldPosition;
in vec3 worldNormal;
in vec2 texcoord;

out vec4 fragColor;

void main()
{
    // We are using a flow texture system, based on [Vlachos2010]
    // "Water Flow in Portal 2"
    // http://www.valvesoftware.com/publications/2010/siggraph2010_vlachos_waterflow.pdf
    //
    // This system uses a "flow texture", containing a per-pixel flow vector,
    // which is applied to the opacity texture coordinate over time.
    //
    // To prevent the texture being stretched too much, we sample the opacity texture twice,
    // with different period offsets, and combine the results.
    // This was suggested in [Vlachos2010] and is based on [Max1995], 
    // "Flow visualization using moving textures" in Proceedings of the ICASW / LaRC Symposium
    // on Visualizing Time - Varying Data

    const float flowDistance = 0.5;

    // First, read the flow texture
    vec2 flow = texture(_ShieldFlowTexture, texcoord).ga;

    // Build 2 flow values that are half a phase apart from each other
    vec2 phaseTimes = mod(vec2(_Time.r * 0.1) + vec2(0.0, 0.5), 1.0);
    vec2 flowOffset1 = flow * phaseTimes.x * flowDistance;
    vec2 flowOffset2 = flow * phaseTimes.y * flowDistance;

    // Pick weights that are 0% at phase times 0 and 1, and 100% at phase time 0.5
    vec2 phaseWeights = 1.0 - abs(phaseTimes - 0.5) * 2.0;

    // The opacity is stored in the shield texture
    // Sample for each flow offset and combine the results [Max1995]
    float opacity1 = texture(_ShieldOpacityTexture, texcoord + flowOffset1).g;
    float opacity2 = texture(_ShieldOpacityTexture, texcoord + flowOffset2).g;
    float opacity = (opacity1 * phaseWeights.x) + (opacity2 * phaseWeights.y) / (phaseWeights.x + phaseWeights.y);

    // Use a fresnel-like term for the brightness
    // This gives the shield a glow at the edges
    vec3 viewDir = normalize(_CameraPosition.xyz - worldPosition.xyz);
    float glowTerm = 1.0 - dot(viewDir, worldNormal);

    // Combine the opacity and glow
    opacity *= glowTerm;

    // Determine the color based on the glow term
    vec3 color = mix(vec3(0.02, 0.2, 0.8), vec3(1.0), glowTerm);

    // Output the color and opacity
    fragColor = vec4(color, opacity);
}

#endif // FRAGMENT_SHADER