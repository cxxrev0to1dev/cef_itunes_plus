#include "itunes_plus/itunes_web_render_process.h"
#include "itunes_plus/itunes_registed_state.h"
#include "itunes_plus/itunes_dom_node_visitor.h"

namespace iPlus{
	iTunesWebRenderProcess::iTunesWebRenderProcess(void){
	}
	iTunesWebRenderProcess::~iTunesWebRenderProcess(void){
	}
	CefRefPtr<CefRenderProcessHandler> iTunesWebRenderProcess::GetRenderProcessHandler() {
		CEF_REQUIRE_RENDERER_THREAD();
		return this;
	}
	bool iTunesWebRenderProcess::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
		CefProcessId source_process,
		CefRefPtr<CefProcessMessage> message) {
			const std::string& msg_name = message->GetName();
			if(msg_name == "dom_visitor_message"&&browser->HasDocument()){
				CefRefPtr<CefListValue> args = message->GetArgumentList();
				DOMVisitorMessageArgs argv(args->GetString(0),args->GetString(1));
				CefRefPtr<iPlus::iTunesDOMNodeVisitor> distort_html(new iPlus::iTunesDOMNodeVisitor);
				distort_html->ItunesWebBrowser::set_browser(browser);
				distort_html->set_conn_services_key(std::get<0>(argv));
				distort_html->set_machine_guid(std::get<1>(argv));
				CefRefPtr<CefDOMVisitor> distort(dynamic_cast<CefDOMVisitor*>(distort_html.get()));
				browser->GetMainFrame()->VisitDOM(distort);
				return false;
			}
			else{
				return true;
			}
	}
	void iTunesWebRenderProcess::OnWebKitInitialized(){
		CEF_REQUIRE_RENDERER_THREAD();
		CefRegisterExtension("CustomITunes",GetITunesInjectedJS(),NULL);
		return;
	}
	void iTunesWebRenderProcess::OnContextCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context){
			CEF_REQUIRE_RENDERER_THREAD();
			if(browser->GetMainFrame()->GetIdentifier() != frame->GetIdentifier()){
				return;
			}
	}
	CefString iTunesWebRenderProcess::GetITunesInjectedJS(){
		const std::string plugin_name = "\\plugins\\itunes.js";
		char filefull[_MAX_PATH] = {0};
		GetModuleFileNameA(NULL,filefull,_MAX_PATH);
		std::string full = g_path_get_dirname(filefull);
		full.append(plugin_name);
		std::ifstream is(full.c_str(),std::ifstream::binary);
		if(is) {
			is.seekg(0,is.end);
			int length = static_cast<int>(is.tellg());
			is.seekg(0,is.beg);
			std::string str;
			str.resize(length+sizeof(char),0);
			char* begin = &*str.begin();
			is.read(begin,length);
			is.close();
			return CefString(str);
		}
		return CefString("");
	}
}