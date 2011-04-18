uniform sampler2D tex;
uniform float d;
uniform int ksize;

void main() {
	vec4 csum = vec4(0.0, 0.0, 0.0, 0.0);
	float sum = 0.0;
	vec2 t = gl_TexCoord[0].st;
	for (int i = 0; i < ksize; ++i) {
		for (int j = 0; j < ksize; ++j) {
			vec2 to = vec2(t.x + float(i - ksize/2)*d, t.y + float(j - ksize/2)*d);
			vec4 c = texture2D(tex, to);
			float s = sqrt(float(i*i) + float(j*j));
			sum += s;
			csum += c*s;
		}
	}
	csum /= sum;
	gl_FragColor = gl_Color*vec4(csum.r/csum.a, csum.g/csum.a, csum.b/csum.a, csum.a);
}
