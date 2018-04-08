
#ifndef AniCntrlH
#define AniCntrlH

#include <Control.h>

class CAnimatedControl: public CControl {
public:
	CAnimatedControl(int Left, int Top, int Width, int Height, int Visible, int OnTop, void *HandlerClass);
	virtual ~CAnimatedControl();

	virtual int MouseMove(int X, int Y);

	virtual void MouseOver();
	virtual void MouseOut();

	static bool Animate;

protected:
	bool MouseIsOver;
	virtual void Draw(long Left, long Top, long Width, long Height);   //ML Pure virtual, definition required !!
};

#endif
