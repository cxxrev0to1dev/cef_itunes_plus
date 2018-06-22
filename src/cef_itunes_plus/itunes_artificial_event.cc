#include "itunes_plus/itunes_artificial_event.h"

/*
				iPlus::iTunesArtificialEvent artificial;
				artificial.set_browser(browser);
				artificial.keyEvent(true,VK_TAB);
				artificial.keyEvent(true,VK_TAB);
				browser->GetFocusedFrame()->Paste();
				artificial.keyEvent(true,VK_TAB);
				artificial.keyEvent(false,VK_TAB);
				browser->GetFocusedFrame()->Paste();
				artificial.keyEvent(false,VK_TAB);
				artificial.keyEvent(true,VK_TAB);
				browser->GetFocusedFrame()->Paste();
*/

namespace iPlus{
	iTunesArtificialEvent::iTunesArtificialEvent(void){
		CEF_REQUIRE_UI_THREAD();
		set_browser(nullptr);
	}
	iTunesArtificialEvent::iTunesArtificialEvent(CefRefPtr<CefBrowser> browser){
		CEF_REQUIRE_UI_THREAD();
		set_browser(browser);
	}
	iTunesArtificialEvent::~iTunesArtificialEvent(void){
		CEF_REQUIRE_UI_THREAD();
		set_browser(nullptr);
	}
	void iTunesArtificialEvent::set_browser(CefRefPtr<CefBrowser> browser){
		CEF_REQUIRE_UI_THREAD();
		browser_ = browser;
	}
	CefRefPtr<CefBrowser> iTunesArtificialEvent::get_browser() const{
		CEF_REQUIRE_UI_THREAD();
		return browser_;
	}
	void iTunesArtificialEvent::mouseMoved(int32 xPos, int32 yPos){
		CEF_REQUIRE_UI_THREAD();
		CefMouseEvent mouseEvent;
		mouseEvent.x = xPos;
		mouseEvent.y = yPos;
		mouseEvent.modifiers = 0;
		bool mouseLeave = false;
		get_browser()->GetHost()->SendMouseMoveEvent(mouseEvent, mouseLeave);
	}

	void iTunesArtificialEvent::mouseButton(uint32 buttonId, int32 xPos, int32 yPos, bool down, int32 clickCount){
		CEF_REQUIRE_UI_THREAD();
		CefBrowserHost::MouseButtonType buttonType = MBT_LEFT;
		switch (buttonId){
		case 0:
			break;
		case 1:
			buttonType = MBT_MIDDLE;
			break;
		case 2:
			buttonType = MBT_RIGHT;
			break;
		default:
			// Other mouse buttons are not handled here.
			std::stringstream msg;
			msg << "Unable to determine mouse button: " << buttonId;
			return;
		}
		CefMouseEvent mouseEvent;
		mouseEvent.x = xPos;
		mouseEvent.y = yPos;
		mouseEvent.modifiers = 0;
		bool mouseUp = !down;
		get_browser()->GetHost()->SendMouseClickEvent(mouseEvent, buttonType, mouseUp, clickCount);
	}

	void iTunesArtificialEvent::mouseClick(uint32 buttonId, int32 xPos, int32 yPos){
		CEF_REQUIRE_UI_THREAD();
		mouseButton(buttonId, xPos, yPos, true, 1);
		mouseButton(buttonId, xPos, yPos, false, 1);
	}

	void iTunesArtificialEvent::mouseWheel(int32 xPos, int32 yPos, int32 xScroll, int32 yScroll){
		CEF_REQUIRE_UI_THREAD();
		CefMouseEvent event;
		event.x = xPos;
		event.y = yPos;
		event.modifiers = 0;
		get_browser()->GetHost()->SendMouseWheelEvent( event, xScroll, yScroll );
	}

	void iTunesArtificialEvent::keyEvent(bool pressed,int32 virtualKeyCode,int32 mods){
		CEF_REQUIRE_UI_THREAD();
		CefKeyEvent keyEvent;
		keyEvent.windows_key_code = virtualKeyCode;
		keyEvent.native_key_code = virtualKeyCode;
		keyEvent.modifiers = mods;
		if (pressed){
			keyEvent.type = KEYEVENT_RAWKEYDOWN;
			get_browser()->GetHost()->SendKeyEvent(keyEvent);
		}
		else{
			keyEvent.type = KEYEVENT_KEYUP;
			get_browser()->GetHost()->SendKeyEvent(keyEvent);
			keyEvent.type = KEYEVENT_CHAR;
			get_browser()->GetHost()->SendKeyEvent(keyEvent);
		}
	}
}