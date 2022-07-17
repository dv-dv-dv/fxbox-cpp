#include <vector>
#include "globals.h"
#include "a2d.h"
#include "filt.h"
class Equalizer {
private:
	class Filter {
	private:
		filt::filtba<pflt> butt2 = filt::filtba<pflt>({ 0, 0, 1 }, { 1, 1.41421, 1 });
		filt::FastBilin<pflt> fb;
		short indx = 0, indy = 0;
	public:
		a2d::Array2Dy<pflt> prevx;
		a2d::Array2Dy<pflt> prevy;
		short type;
		int N;
		pflt wo, bfac;
		filt::filtba<pflt> analog, digital;
		Filter();
		Filter(char type, pflt wo, pflt bfac, pflt gdb);
		void set(short type, pflt wo, pflt bfac, pflt gdb);
		void add_prevx(a2d::Array2D<pflt>& x, int index);
		void add_prevy(a2d::Array2D<pflt>& y, int index);
		pflt* get_prevx(int index, int channel);
		pflt* get_prevy(int index, int channel);
	};
	Filter lp, hp, bp;
	a2d::Array2Dy<pflt> temp;
	void filter(a2d::Array2D<pflt>& data, Filter& f);
public:
	Equalizer();
	void add_filter(short type, pflt wo, pflt bfac, pflt gdb);
	void equalize(a2d::Array2D<pflt>& data);
};