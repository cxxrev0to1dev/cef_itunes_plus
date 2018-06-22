#ifndef ITUNES_PLUS_ITUNES_WEB_RENDER_PROCESS_H_
#define ITUNES_PLUS_ITUNES_WEB_RENDER_PROCESS_H_

#include "include/cef_app.h"
#include "include/wrapper/cef_helpers.h"
#include <tuple>

namespace iPlus{
	class iTunesWebRenderProcess: 
		public CefApp,
		public CefRenderProcessHandler
	{
	public:
		typedef std::tuple<CefString,CefString> DOMVisitorMessageArgs;
		iTunesWebRenderProcess(void);
		~iTunesWebRenderProcess(void);
		virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() OVERRIDE;
		virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) OVERRIDE;
		virtual void OnWebKitInitialized() OVERRIDE;
		virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) OVERRIDE;
	private:
		CefString GetITunesInjectedJS();
		IMPLEMENT_REFCOUNTING(iTunesWebRenderProcess);
	};

}

#endif // !ITUNES_PLUS_ITUNES_WEB_RENDER_PROCESS_H_
