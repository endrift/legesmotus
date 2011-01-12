uniform sampler2D tex;
uniform float d;
uniform int ksize;

void main() {
	vec4 csum;
	float sum;
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
	gl_FragColor = vec4(csum.r, csum.g, csum.b, csum.a);
}