#version  150

in vec4 vPosition;
in vec3 vNormal;

out vec3 fN;
out vec3 fE;

out vec3 fL1;
out vec3 fL2;
out vec3 fL3;

uniform mat4 model_view;
uniform mat4 projection;
uniform vec4 LightPosition1;
uniform vec4 LightPosition2;
uniform vec4 LightPosition3;

void main()
{
vec3 pos=(model_view*vPosition).xyz;

fN=(model_view*vec4(vNormal,0.0)).xyz;
fE=-pos;

fL1=(model_view*LightPosition1).xyz;
fL2=(model_view*LightPosition2).xyz;
fL3=(model_view*LightPosition3).xyz;

if(LightPosition1.w!=0.0){
fL1=(model_view*LightPosition1).xyz-pos;
}

if(LightPosition2.w!=0.0){
fL2=(model_view*LightPosition2).xyz-pos;
}

if(LightPosition3.w!=0.0){
fL3=(model_view*LightPosition3).xyz-pos;
}

gl_Position = projection*model_view*vPosition;
}