uniform sampler2D tex;
uniform float time;
uniform float zoom;
uniform vec2 center;
uniform float arc;
uniform float theta;
uniform float life;

void main() {
	vec2 st = (gl_FragCoord.st/512.0)-center;
	float phase = cos(radians(time * 180.0));
	float t = 1.0 - (phase * 0.5 + 0.25);
	float r = length(st)/zoom; //+ t;
	float ang = (degrees(atan(st.y, st.x))/360.0);
	ang -= theta;
	ang = mod(ang, 1.0);
	ang = ang*2.0 - 1.0;
	ang = abs(ang);
	ang += arc*2.0 - 1.0;
	ang = clamp(ang, 0.0, 1.0);
	r -= t;
	float scale = cos(radians(90.0)*8.0*r + radians(t*90.0)*3.0);
	scale *= 2.0*(time*(1.0 - time)); //(1.0 - phase*phase);
	r -= 0.5;
	float a = 0.08;
	scale = scale*(16.0*a*a*a)/(r*r + 4.0*a*a);
	scale = 1.0 + 1.0*scale;
	scale = (scale - 1.0) * pow(ang, 8.0) + 1.0;
	scale = (scale - 1.0) * 2.0/arc + 1.0;
	//gl_FragColor = vec4(ang, ang, ang, 1);
	//gl_FragColor = vec4(r, r, r, 1);
	//gl_FragColor = vec4(scale-1.0, scale, scale+1.0, 1.0);
	st = gl_TexCoord[0].st;
	st = st - vec2(0.5, 0.5);
	st *= scale;
	st = st + vec2(0.5, 0.5);
	//gl_FragColor = vec4(st.r, st.g, scale, 1.0);
	gl_FragColor = gl_Color*texture2D(tex, st)*vec4(scale, scale, scale, 1.0);
}
