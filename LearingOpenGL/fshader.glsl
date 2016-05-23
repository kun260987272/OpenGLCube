#version 150

in vec3 fN;
in vec3 fE;

in vec3 fL1;
in vec3 fL2;
in vec3 fL3;

out vec4 fColor;

uniform vec4 AmbientProduct;
uniform vec4 DiffuseProduct1,SpecularProduct1;
uniform vec4 DiffuseProduct2,SpecularProduct2;
uniform vec4 DiffuseProduct3,SpecularProduct3;

uniform float Shininess;

void main()
{
vec3 N=normalize(fN);
vec3 E=normalize(fE);
vec3 L1=normalize(fL1);
vec3 L2=normalize(fL2);
vec3 L3=normalize(fL3);

vec3 H1=normalize(L1+E);
vec3 H2=normalize(L2+E);
vec3 H3=normalize(L3+E);

vec4 ambient=AmbientProduct;

float Kd1=max(dot(L1,N),0.0);
vec4 diffuse1=Kd1*DiffuseProduct1;

float Ks1=pow(max(dot(N,H1),0.0),Shininess);
vec4 specular1=Ks1*SpecularProduct1;

if(dot(L1,N)<0.0){
specular1=vec4(0.0,0.0,0.0,1.0);
}

float Kd2=max(dot(L2,N),0.0);
vec4 diffuse2=Kd2*DiffuseProduct2;

float Ks2=pow(max(dot(N,H2),0.0),Shininess);
vec4 specular2=Ks2*SpecularProduct2;

if(dot(L2,N)<0.0){
specular2=vec4(0.0,0.0,0.0,1.0);
}

float Kd3=max(dot(L3,N),0.0);
vec4 diffuse3=Kd3*DiffuseProduct3;

float Ks3=pow(max(dot(N,H3),0.0),Shininess);
vec4 specular3=Ks3*SpecularProduct3;

if(dot(L3,N)<0.0){
specular3=vec4(0.0,0.0,0.0,1.0);
}


fColor=ambient+diffuse1+specular1+diffuse2+specular2+diffuse3+specular3;
fColor.a=1.0;

}