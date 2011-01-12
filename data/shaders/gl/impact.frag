uniform sampler2D tex;
uniform float time;
uniform vec2 size;
uniform vec2 center;
uniform float arc;
uniform float theta;
uniform float speed;
uniform float offset;
uniform float waves;
const float PI = 3.14159265358979323846264;

void main() {
	vec2 st = (gl_TexCoord[0].st)-center;
	float phase = radians(time * 180.0);
	//float t = 1.0 - (phase * 0.5 + 0.25);
	//t *= speed;
	st /= (size*0.5);
	float r = length(st); //+ t;

	float ang = (atan(st.y, st.x) - radians(theta*360.0));
	r /= (1.0 - (arc - 1.0)*cos(ang))*sqrt(arc);
	// Optimization: if out of bounds, just go normally
	if (r/time > 1.0) {
		gl_FragColor = gl_Color*texture2D(tex, gl_TexCoord[0].st);
		return;
	}
	/*float ang = (degrees(atan(st.y, st.x))/360.0);
	ang -= theta;
	ang = mod(ang, 1.0);
	ang = ang*2.0 - 1.0;
	ang = abs(ang);
	ang += arc*2.0 - 1.0;
	ang = clamp(ang, 0.0, 1.0);*/
	//r -= t;
	// Scale: [-1, 1]
	float scale;// = cos(PI*2.0*r - radians(t*90.0));
	//scale *= 2.0*(time*(time - 1.0)); //(1.0 - phase*phase);
	//r -= 0.5;
	//r -= time;
	//scale *= pow(ang, 2.0);
	phase /= PI;
	// Gauss function
	//float g = exp(-(r - 0.5*phase)*(r - 0.5*phase)/(2.0*(0.25*phase)*(0.25*phase)));
	//float g = exp(-(r - 0.9*phase)*(r - 0.9*phase)/(2.0*(0.5*phase)*(0.5*phase)*(phase-r)));
	// Sine scaling factor
	//float sfact = (1.0 - sqrt(phase));
	// Scaled sine function
	//float s = 4.0*sin((r - (phase-offset)*speed)*waves*PI)*sfact;
	//s = s * 0.5 + 0.5;
	//scale *= s*g*(1.0-phase);
	scale = sin((r - (phase - offset)*speed)*waves*PI)*0.5 + 0.5;
	scale *= 16.0*time*(1.0 - r/time)*r*(1.0 - time)*(1.0 - time);
	scale += 1.0;
	// scale : [1.0, 2.0]
	//scale = scale*(4.0)/(64.0*r*r + 4.0);
	//scale = clamp((r*r - 2.0*phase*r)/(phase*phase), 0.0, 1.0)*scale;
	//scale = scale*(1.0 - (r-1.0)*(r-phase));
	//scale = scale*(1.0 - sqrt(time));
	//scale = 1.0 + 1.0*scale;
	//scale = (scale - 1.0) * 2.0/arc + 1.0;
	//gl_FragColor = vec4(ang, ang, ang, 1);
	//gl_FragColor = vec4(r, r, r, 1);
	gl_FragColor = vec4(scale-1.0, scale, scale+1.0, 1.0);
	// Remap scale: [-1, 1] -> [1, 1.5]
	//scale = scale * 0.25 + 1.25;
	// Move center
	//st = st - center;
	//scale = 2.0*scale - 1.0;
	st = (2.0-scale)*(gl_TexCoord[0].st - center);
	st = st + center;
	// Map colors
	// [0.25, 1.25] -> [1.125, 0.875]
	//scale = 2.0 - scale;
	//scale = (1.0 + scale);
	//gl_FragColor = vec4(st.y-0.5, st.y, st.y+0.5, 1.0);
	//gl_FragColor = vec4(st.x-0.5, st.x, st.x+0.5, 1.0);
	gl_FragColor = gl_Color*texture2D(tex, st)*vec4(scale, scale, scale, 1.0);
}