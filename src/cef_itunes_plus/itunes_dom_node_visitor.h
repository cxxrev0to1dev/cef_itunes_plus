#ifndef ITUNES_PLUS_ITUNES_DOM_NODE_VISITOR_H_
#define ITUNES_PLUS_ITUNES_DOM_NODE_VISITOR_H_

#include "include/cef_app.h"
#include "include/internal/cef_types.h"
#include "include/wrapper/cef_helpers.h"
#include "itunes_plus/itunes_calc_services.h"
#include "itunes_plus/itunes_web_browser.h"

namespace iPlus{
	class iTunesDOMNodeVisitor:
		public CefDOMVisitor,
		public ItunesWebBrowser
	{
	public:
		iTunesDOMNodeVisitor(void);
		~iTunesDOMNodeVisitor(void);
		void set_conn_services_key(const CefString& key);
		void set_machine_guid(const CefString& machine_guid);
		virtual void Visit(CefRefPtr<CefDOMDocument> document);
	private:
		bool IsStayAcceptTermsProtocol(CefRefPtr<CefDOMDocument> document);
		bool WrittenAcceptTermsProtocol(CefRefPtr<CefDOMDocument> document);
		bool WrittenAccountInformation(CefRefPtr<CefDOMDocument> document);
		bool IsStayAccountInformation(CefRefPtr<CefDOMDocument> document);
		bool WrittenBillingInformation(CefRefPtr<CefDOMDocument> document);
		bool IsStayBillingInformation(CefRefPtr<CefDOMDocument> document,const CefString name = "");
		void set_x_apple_action_signature(const CefString& x_apple_action_signature){
			x_apple_action_signature_ = x_apple_action_signature;
		}
		CefString conn_services_key_;
		CefString machine_guid_;
		CefString x_apple_action_signature_;
		IMPLEMENT_REFCOUNTING(iTunesDOMNodeVisitor);
	};
}

#endif // !ITUNES_PLUS_ITUNES_DOM_NODE_VISITOR_H_


