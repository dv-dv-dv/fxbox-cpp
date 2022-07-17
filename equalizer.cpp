#include "equalizer.h"
Equalizer::Filter::Filter() {}
Equalizer::Filter::Filter(char type, pflt wo, pflt bfac, pflt gdb) {
	this->set(type, wo, bfac, gdb);
}
void Equalizer::Filter::set(short type, pflt wo, pflt bfac, pflt gdb) {
	using namespace filt;
	this->type = type, this->N = 2, this->wo = wo, this->bfac = bfac;
	switch (type) {
	case 0:
		lp2lp<pflt>(butt2, analog, bt_freq_warp<pflt>(wo, sampfreq));
		break;
	case 1:
		lp2hp<pflt>(butt2, analog, bt_freq_warp<pflt>(wo, sampfreq));
		break;
	case 2:
		pflt R = 0.5 * (bfac + sqrt(bfac * bfac + 4));
		pflt g = pow((pflt)10, gdb / 20) - 1;
		lp2bp<pflt>(butt2, analog, bt_freq_warp<pflt>(wo / R, sampfreq), bt_freq_warp<pflt>(wo * R, sampfreq));
		for (auto i = 0; i < analog.getN() + 1; i++) {
			analog.a[i] = analog.a[i] + analog.b[i] * g;
		}
		this->N = 4;
		break;
	}
	prevx.resize(N, 2);
	prevy.resize(N, 2);
	fb.bilin(analog, digital, sampfreq);
	analog.print();
	digital.print();
}
void Equalizer::filter(a2d::Array2D<pflt>& x, Filter& f) {
	using namespace a2d;
	auto asdf = f.digital.getN() + 1;
	aset(temp, x.vreset());
	kset(x, 0);
	for (auto n = 0; n < x.rows; n++) {
		x(n, 0) = f.digital.b[0] * temp(n, 0);
		x(n, 1) = f.digital.b[0] * temp(n, 1);
		for (auto j = 1; j < asdf; j++) {
			if (n - j < 0) {
				x(n, 0) += f.digital.b[j] * f.prevx(f.N - j + n, 0) - f.digital.a[j] * f.prevy(f.N - j + n, 0);
				x(n, 1) += f.digital.b[j] * f.prevx(f.N - j + n, 1) - f.digital.a[j] * f.prevy(f.N - j + n, 1);
			}
			else {
				x(n, 0) += f.digital.b[j] * temp(n - j, 0) - f.digital.a[j] * x(n - j, 0);
				x(n, 1) += f.digital.b[j] * temp(n - j, 1) - f.digital.a[j] * x(n - j, 1);
			}
		}
	}
	aset(f.prevx, temp.vpart(x.rows - f.N));
	aset(f.prevy, x.vpart(x.rows - f.N));
	x.vreset(); temp.vreset();
}
Equalizer::Equalizer() {
	temp.resize(buffer_size, 2);
	lp.set(1, 1000, -1, 1);
}
void Equalizer::add_filter(short type, pflt wo, pflt bfac = -1, pflt gdb = -1) {
}
void Equalizer::equalize(a2d::Array2D<pflt>& data) {
	this->filter(data, lp);
}