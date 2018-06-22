#ifndef ITUNES_PLUS_ITUNES_ARTIFICIAL_EVENT_H_
#define ITUNES_PLUS_ITUNES_ARTIFICIAL_EVENT_H_

#include "include/cef_app.h"
#include "include/internal/cef_types.h"
#include "include/wrapper/cef_helpers.h"

namespace iPlus{
	class iTunesArtificialEvent
	{
	public:
		iTunesArtificialEvent(void);
		explicit iTunesArtificialEvent(CefRefPtr<CefBrowser> browser);
		~iTunesArtificialEvent(void);
		void set_browser(CefRefPtr<CefBrowser> browser);
		CefRefPtr<CefBrowser> get_browser() const;
		void mouseMoved(int32 xPos, int32 yPos);
		void mouseButton(uint32 buttonId, int32 xPos, int32 yPos, bool down, int32 clickCount = 1);
		void mouseClick(uint32 buttonId, int32 xPos, int32 yPos);
		void mouseWheel(int32 xPos, int32 yPos, int32 xScroll, int32 yScroll);
		void keyEvent(bool pressed,int32 virtualKeyCode,int32 mods = EVENTFLAG_NONE);
	private:
		CefRefPtr<CefBrowser> browser_;
		IMPLEMENT_REFCOUNTING(iTunesArtificialEvent);
	};
}

#endif // !ITUNES_PLUS_ITUNES_ARTIFICIAL_EVENT_H_

