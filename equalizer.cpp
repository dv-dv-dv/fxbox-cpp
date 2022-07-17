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
	prevx.resize(N + 1, 2);
	prevy.resize(N + 1, 2);
	fb.bilin(analog, digital, sampfreq);
	analog.print();
	digital.print();
}

void Equalizer::Filter::add_prevx(a2d::Array2D<pflt>& x, int index) {
	indx = (indx + 1) % prevx.rows;
	prevx(indx, 0) = x(index, 0);
	prevx(indx, 1) = x(index, 1);
}
void Equalizer::Filter::add_prevy(a2d::Array2D<pflt>& y, int index) {
	prevy(indy, 0) = y(index, 0);
	prevy(indy, 1) = y(index, 1);
	indy = (indy + 1) % prevy.rows;
}
pflt* Equalizer::Filter::get_prevx(int index, int channel) {
	pflt test = indx - index;
	if (test < 0) {
		return &prevx(test + prevx.rows, channel);
	}
	else {
		return &prevx(test, channel);
	}
}
pflt* Equalizer::Filter::get_prevy(int index, int channel) {
	pflt test = indy - index;
	if (test < 0) {
		return &prevy(test + prevy.rows, channel);
	}
	else {
		return &prevy(test, channel);
	}
}
void Equalizer::filter(a2d::Array2D<pflt>& x, Filter& f) {
	using namespace a2d;
	pflt* px; pflt* py;
	for (auto n = 0; n < x.rows; n++) {
		f.add_prevx(x, n);
		x(n, 0) *= f.digital.b[0];
		x(n, 1) *= f.digital.b[0];
		for (auto j = 1; j <= f.N; j++) {
			px = f.get_prevx(j, 0);
			py = f.get_prevy(j, 0);
			x(n, 0) += f.digital.b[j] * (*px)- f.digital.a[j] * (*py);
			x(n, 1) += f.digital.b[j] * (*(px + 1)) - f.digital.a[j] * (*(py + 1));
		}
		f.add_prevy(x, n);
	}
}
Equalizer::Equalizer() {
	temp.resize(buffer_size, 2);
	lp.set(0, 1000, -1, 1);
}
void Equalizer::add_filter(short type, pflt wo, pflt bfac = -1, pflt gdb = -1) {
}
void Equalizer::equalize(a2d::Array2D<pflt>& data) {
	this->filter(data, lp);
}