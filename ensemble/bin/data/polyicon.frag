
#ifdef GL_ES
precision mediump float;
#endif

#define PI 3.14159265359
#define TWO_PI 6.28318530718

uniform vec2 u_resolution;
uniform float u_time;
varying vec2 st; // this is already calculated in the vertex

uniform vec4 u_envelopes;
uniform vec4 u_crossmods;

mat2 rotate2d( in float _angle ){ return mat2(cos(_angle),-sin(_angle), sin(_angle),cos(_angle)); }
vec2 rotated( vec2 _st, in float _angle ){ _st -= 0.5; _st *= rotate2d( _angle ); _st += 0.5; return _st;}

float rect( vec2 st, vec2 size ){
    return step( 0.5-size.x, st.x ) * step( 0.5-size.y, st.y ) * step( st.x, 0.5+size.x ) * step( st.y, 0.5+size.y );
}

float rect( vec2 st, vec2 size, float w ){
    return rect( st, size) - rect( st, size-(w*2.0));
}

// canonic random one-liner
float rand(vec2 st, float t){ return fract(sin(dot(st.xy + fract(t*0.0013) ,vec2(12.9898,78.233))) * 43758.5453); }


vec4 element( vec2 st, float env, float cross, vec3 crosscolor, float side, float width ){
    vec2 t = rotated(st, PI*0.25);
    float shape = rect( t, vec2(side), width ); 
    float a = shape * env;
    vec3 color = mix( vec3(1.0), crosscolor, cross ) * shape;
    return vec4( color, a );
}

void main(){
    
    float env = u_envelopes[0];
    float cross = u_crossmods[2];

    float r = rand( vec2(st.x), u_time );  
    float gate = step(r, 0.3);  
    vec3 crosscolor = vec3(1.0, gate, gate);
    
    vec4 sum = vec4( 0.0 );
    
    float separation = 0.15;
    float side = separation;
    float width = 0.05;
    
    sum += element( vec2(st.x+separation, st.y+separation), u_envelopes[0], u_crossmods[0], crosscolor, side, width );
    sum += element( vec2(st.x-separation, st.y+separation), u_envelopes[1], u_crossmods[1], crosscolor, side, width  );
    sum += element( vec2(st.x-separation, st.y-separation), u_envelopes[2], u_crossmods[2], crosscolor, side, width  );
    sum += element( vec2(st.x+separation, st.y-separation), u_envelopes[3], u_crossmods[3], crosscolor, side, width  );
    
    gl_FragColor = sum;
  
}
