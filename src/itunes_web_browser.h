#ifndef ITUNES_PLUS_ITUNES_WEB_BROWSER_H_
#define ITUNES_PLUS_ITUNES_WEB_BROWSER_H_

#include "include/cef_app.h"
#include "include/internal/cef_types.h"
#include "include/wrapper/cef_helpers.h"

namespace iPlus{
	class ItunesWebBrowser
	{
	public:
		ItunesWebBrowser(){
			set_browser(nullptr);
		}
		~ItunesWebBrowser(){
			set_browser(nullptr);
		}
		void set_browser(CefRefPtr<CefBrowser> browser){
			browser_ = browser;
		}
		CefRefPtr<CefBrowser> get_browser() const{
			return browser_;
		}
	private:
		CefRefPtr<CefBrowser> browser_;
		IMPLEMENT_REFCOUNTING(ItunesWebBrowser);
	};
}

#endif // !#ITUNES_PLUS_ITUNES_WEB_BROWSER_H_
