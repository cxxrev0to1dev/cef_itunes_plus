#include "itunes_plus/main.h"
#include <list>
#include <sstream>
#include <string>
#include <gtk/gtk.h>
#include <glib/gthread.h>
#ifdef OS_WIN
#include <gdk/gdkwin32.h>
#include <Windows.h>
#pragma comment(lib,"WebKit.lib")
#pragma comment(lib,"WebKitGUID.lib")
#pragma comment(lib,"JavaScriptCore.lib")
#pragma comment(lib,"libhunspell.lib")
#pragma comment(lib,"gobject-2.0.lib")
#pragma comment(lib,"gtk-win32-2.0.lib")
#pragma comment(lib,"gdk-win32-2.0.lib")
#pragma comment(lib,"gthread-2.0.lib")
#pragma comment(lib,"glib-2.0.lib")
#pragma comment(lib,"IPHLPAPI.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"Shlwapi.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"WinMM.lib")
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"Winhttp.lib")
#pragma comment(lib,"comsupp.lib")
#pragma comment(lib,"cef_sandbox.lib")
#pragma comment(lib,"libcef.lib")
#pragma comment(lib,"libcef_dll_wrapper.lib")
#endif
#include "include/cef_client.h"
#include "include/cef_app.h"
#include "include/cef_urlrequest.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_message_router.h"
#include "itunes_plus/itunes_registed_state.h"
#include "itunes_plus/itunes_web_render_process.h"
#include "itunes_plus/itunes_calc_services.h"
#include "itunes_plus/itunes_dom_node_visitor.h"
#include "itunes_plus/itunes_artificial_event.h"
#include "itunes_plus/gtk_window_core.h"
#include "itunes_plus/multi_part_form_data.h"

class HTMLMessageHandler : 
	public CefClient,
	public CefDisplayHandler,
	public CefLifeSpanHandler,
	public CefLoadHandler,
	public CefRequestHandler,
	public CefResourceHandler,
	public CefURLRequestClient,
	public CefContextMenuHandler,
	public iPlus::iTunesCalcServices{
public:
	HTMLMessageHandler():is_closing_(false){
		set_xaa_sig_1("");
	}
	DECLARE_EMPTY_DESTRUCTOR(HTMLMessageHandler);
	DISALLOW_EVIL_CONSTRUCTORS(HTMLMessageHandler);
	virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE {
		return NULL;
	}
	virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE {
		return this;
	}
	virtual CefRefPtr<CefResourceHandler> GetResourceHandler(
		CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request) OVERRIDE {
			CEF_REQUIRE_IO_THREAD();
			if(iPlus::ITunesState::Instance()->wsw_xaa_sig()==iPlus::ITunesState::SignupWizard::S_ERROR){
				return NULL;
			}
			return this;
	}
	virtual void BrowserCreated(){
		CEF_REQUIRE_UI_THREAD();
		iPlus::iTunesCalcServices::Init();
	}
	CefRequest::HeaderMap iTunesHTTPSHeaders(CefString xaa_sig){
		CefRequest::HeaderMap headers;
		if(!xaa_sig.empty()){
			headers.insert(
				KeyPairType(L"X-Apple-ActionSignature",[xaa_sig]() -> CefString {
					if(xaa_sig.empty())
						return "";
					else
						return xaa_sig;
			}()));
		}
		headers.insert(
			KeyPairType(L"X-Apple-Tz",L"28800"));
		headers.insert(
			KeyPairType(L"X-Apple-Store-Front",L"143465-19,28"));
		return headers;
	}
	void OnAfterCreated(CefRefPtr<CefBrowser> browser) {
		CEF_REQUIRE_UI_THREAD();
		browser_list_.push_back(browser);
	}
	bool DoClose(CefRefPtr<CefBrowser> browser) {
		CEF_REQUIRE_UI_THREAD();
		if(browser_list_.size() == 1) {
			is_closing_ = true;
		}
		return false;
	}
	void OnBeforeClose(CefRefPtr<CefBrowser> browser) {
		CEF_REQUIRE_UI_THREAD();
		BrowserList::iterator bit = browser_list_.begin();
		for (; bit != browser_list_.end(); ++bit) {
			if ((*bit)->IsSame(browser)) {
				browser_list_.erase(bit);
				break;
			}
		}
		if (browser_list_.empty()) {
			CefQuitMessageLoop();
		}
	}
	//CefContextMenuHandler method
	void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model) OVERRIDE{
			model->Clear();
	}
	//CefURLRequestClient method
	virtual void OnRequestComplete(CefRefPtr<CefURLRequest> request){
		CEF_REQUIRE_IO_THREAD();
		set_response_hold(request->GetResponse());
		if(response_ok_callback()){
			response_ok_callback()->Continue();
		}
		return;
	}
	virtual void OnUploadProgress(CefRefPtr<CefURLRequest> request,
		uint64 current,
		uint64 total) {
			CEF_REQUIRE_IO_THREAD();
			return;
	}
	virtual void OnDownloadProgress(CefRefPtr<CefURLRequest> request,
		uint64 current,
		uint64 total){
			CEF_REQUIRE_IO_THREAD();
			return;
	}
	virtual void OnDownloadData(CefRefPtr<CefURLRequest> request,
		const void* data,
		size_t data_length){
			CEF_REQUIRE_IO_THREAD();
			if(data_length){
				response_data_append((const char*)data,data_length);
			}
			return;
	}
	virtual bool GetAuthCredentials(bool isProxy,
		const CefString& host,
		int port,
		const CefString& realm,
		const CefString& scheme,
		CefRefPtr<CefAuthCallback> callback){
			CEF_REQUIRE_IO_THREAD();
			return true;
	}
	//CefResourceHandler methods
	virtual bool ProcessRequest(CefRefPtr<CefRequest> request,
		CefRefPtr<CefCallback> callback) OVERRIDE{
			CEF_REQUIRE_IO_THREAD();
			set_response_ok_callback(callback);
			set_request_hold(CefURLRequest::Create(request,this));
			return true;
	}
	virtual void GetResponseHeaders(CefRefPtr<CefResponse> response,
		int64& response_length,
		CefString& redirectUrl) OVERRIDE{
			CEF_REQUIRE_IO_THREAD();
			CefResponse::HeaderMap header_map;
			if(response_hold()){
				response_hold()->GetHeaderMap(header_map);
				response->SetHeaderMap(header_map);
				CefString status_text = response_hold()->GetStatusText();
				CefString mime_type = response_hold()->GetMimeType();
				response->SetStatus(response_hold()->GetStatus());
				if(status_text.length()){
					response->SetStatusText(status_text);
				}
				if(mime_type.length()){
					response->SetMimeType(mime_type);
				}
			}
			if(iPlus::ITunesState::Instance()->wsw_xaa_sig()==iPlus::ITunesState::SignupWizard::SUCCESS){
				CefResponse::HeaderMap::iterator it = header_map.find("x-apple-actionsignature");
				if(it!=header_map.end()){
					const std::string src = it->second;
					iTunesCalcServices::XAppleActionSignature_2(src.c_str(),src.length());
					iPlus::ITunesState::Instance()->set_wsw_xaa_sig(iPlus::ITunesState::SignupWizard::S_ERROR);
				}
			}
			response_length = response_data().length();
			set_response_offset(0);
			return;
	}
	virtual void Cancel() OVERRIDE{
		if(request_hold()){
			set_request_hold(NULL);
		}
		if(response_hold()){
			set_response_hold(NULL);
		}
		if(response_ok_callback()){
			set_response_ok_callback(NULL);
		}
		if(response_data().length()){
			set_response_data("");
		}
		return;
	}
	virtual bool ReadResponse(void* data_out, int bytes_to_read,
		int& bytes_read,
		CefRefPtr<CefCallback> callback) OVERRIDE{
			CEF_REQUIRE_IO_THREAD();
			size_t size = response_data().size();
			if(response_offset() < size){
				int transfer_size = min(bytes_to_read, static_cast<int>(size - response_offset()));
				memcpy(data_out, response_data().c_str() + response_offset(), transfer_size);
				response_offset_append(transfer_size);
				bytes_read = transfer_size;
				callback->Continue();
				return true;
			}
			return false;
	}
	//
	bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool is_redirect){
			CEF_REQUIRE_UI_THREAD();
			const CefString url = request->GetURL();
			unsigned long load_index = 0;
			while(browser->IsLoading()){
				if(browser->IsLoading()&&
					url==iPlus::xp_cookie){
						load_index = 1;
				}
				else if(browser->IsLoading()&&
					!iPlus::ITunesState::Instance()->IsFilterKey(url)&&
					iPlus::ITunesState::Instance()->IsClickContinueButton()){
						load_index = 2;
				}
				else if(browser->IsLoading()&&
					iPlus::ITunesState::Instance()->get_reg_state()==iPlus::ITunesState::RegistedState::ITUNES_CONNTINUE_COMPLETE){
						load_index = 3;
				}
				break;
			}
			switch(load_index){
			case 1:{
				browser->StopLoad();
				iPlus::MultiPartFormData multipart;
				const std::string machineguid = iPlus::ITunesState::Instance()->machineGUID();
				const std::string machinename = "YYH-PC";
				multipart.AddMultiPart("guid",machineguid.c_str(),machineguid.length());
				multipart.AddMultiPart("machineName",machinename.c_str(),machinename.length());
				const std::string content_data = multipart.ContentData();
				const CefString content_type = multipart.ContentType();
				CefRefPtr<CefRequest> custom = CefRequest::Create();
				CefRefPtr<CefPostData> custom_data = CefPostData::Create();
				set_xaa_sig_1(iTunesCalcServices::XAppleActionSignature_1());
				CefRequest::HeaderMap headers = iTunesHTTPSHeaders(xaa_sig_1());
				CefRefPtr<CefPostDataElement> element = CefPostDataElement::Create();
				element->SetToBytes(content_data.length(),content_data.c_str());
				custom_data->AddElement(element);
				headers.insert(
					std::pair<CefString,CefString>(L"Content-Type",content_type));
				custom->Set(iPlus::signup_wizard,"POST",custom_data,headers);
				frame->LoadRequest(custom);
				iPlus::ITunesState::Instance()->set_wsw_xaa_sig(iPlus::ITunesState::SignupWizard::SUCCESS);
				iPlus::ITunesState::Instance()->set_reg_state(iPlus::ITunesState::RegistedState::ITUNES_CONTINUE_BUTTON);
				break;
			}
			case 2:{
				browser->StopLoad();
				CefRefPtr<CefRequest> custom = CefRequest::Create();
				custom->Set(url,
					request->GetMethod(),
					request->GetPostData(),
					iTunesHTTPSHeaders(xaa_sig_1()));
				frame->LoadRequest(custom);
				iPlus::ITunesState::Instance()->set_reg_state(iPlus::ITunesState::RegistedState::ITUNES_CONNTINUE_COMPLETE);
				iPlus::ITunesState::Instance()->set_request_end(false);
				break;
			}
			case 3:{
				if(iPlus::ITunesState::Instance()->is_request_end()){
					browser->StopLoad();
					CefRefPtr<CefRequest> custom = CefRequest::Create();
					custom->Set(url,
						request->GetMethod(),
						request->GetPostData(),
						iTunesHTTPSHeaders(""));
					frame->LoadRequest(custom);
					iPlus::ITunesState::Instance()->set_request_end(false);
				}
				break;
			}
			default:
				break;
			}
			return false;
	}
	void OnLoadEnd(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		int httpStatusCode){
			CEF_REQUIRE_UI_THREAD();
			if(httpStatusCode==200&&browser->HasDocument()){
				const CefRefPtr<CefBrowser> readerer = browser_list_.begin()->get();
				CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("dom_visitor_message");
				CefRefPtr<CefListValue> args = msg->GetArgumentList();
				iPlus::iTunesWebRenderProcess::DOMVisitorMessageArgs argv(
					iTunesCalcServices::key(),
					iPlus::ITunesState::Instance()->machineGUID());
				args->SetString(0,std::get<0>(argv));
				args->SetString(1,std::get<1>(argv));
				readerer->SendProcessMessage(PID_RENDERER,msg);
				iPlus::ITunesState::Instance()->set_request_end(true);
			}
	}
	void OnLoadError(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		ErrorCode errorCode,
		const CefString& errorText,
		const CefString& failedUrl) {
			CEF_REQUIRE_UI_THREAD();
			if (errorCode == ERR_ABORTED)
				return;
			std::stringstream ss;
			ss << "<html><body bgcolor=\"white\">"
				"<h2>Failed to load URL " << std::string(failedUrl) <<
				" with error " << std::string(errorText) << " (" << errorCode <<
				").</h2></body></html>";
			frame->LoadString(ss.str(), failedUrl);
	}

	void CloseAllBrowsers(bool force_close) {
		if (!CefCurrentlyOn(TID_UI)) {
			CefPostTask(TID_UI,
				base::Bind(&HTMLMessageHandler::CloseAllBrowsers, this, force_close));
			return;
		}
		if (browser_list_.empty())
			return;
		DestroyAllBrowsers(force_close);
	}
	void DestroyAllBrowsers(bool force_close){
		BrowserList::const_iterator it = browser_list_.begin();
		for(;it != browser_list_.end();++it)
			(*it)->GetHost()->CloseBrowser(force_close);
		Cancel();
		return;
	}
	void Destroy(void){
		DestroyAllBrowsers(true);
	}
private:
	void set_xaa_sig_1(const std::string& sig){
		xaa_sig_1_ = sig;
	}
	const std::string xaa_sig_1() const{
		return xaa_sig_1_;
	}
	void set_response_ok_callback(const CefRefPtr<CefCallback>& ptr){
		response_ok_callback_ = ptr;
	}
	CefRefPtr<CefCallback> response_ok_callback() const{
		return response_ok_callback_;
	}
	void set_request_hold(const CefRefPtr<CefURLRequest>& ptr){
		request_hold_ = ptr;
	}
	CefRefPtr<CefURLRequest> request_hold() const{
		return request_hold_;
	}
	void set_response_hold(const CefRefPtr<CefResponse>& ptr){
		response_hold_ = ptr;
	}
	CefRefPtr<CefResponse> response_hold() const{
		return response_hold_;
	}
	void response_data_append(const char* src,const size_t len){
		if(src&&len){
			response_data_.append(src,len);
		}
	}
	void set_response_data(const char* src){
		if(src){
			response_data_ = src;
		}
	}
	const std::string response_data() const{
		return response_data_;
	}
	void response_offset_append(const size_t len){
		response_offset_ += len;
	}
	void set_response_offset(const size_t len){
		response_offset_ = len;
	}
	const size_t response_offset() const{
		return response_offset_;
	}
	typedef std::pair<CefString, CefString> KeyPairType;
	typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
	BrowserList browser_list_;
	CefRefPtr<CefURLRequest> request_hold_;
	CefRefPtr<CefResponse> response_hold_;
	std::string response_data_;
	size_t response_offset_;
	CefRefPtr<CefCallback> response_ok_callback_;
	bool is_closing_;
	std::string xaa_sig_1_;
	IMPLEMENT_REFCOUNTING(HTMLMessageHandler);
};
class HTMLShowApp :
	public iPlus::iTunesWebRenderProcess,
	public CefBrowserProcessHandler,
	public HTMLMessageHandler{
public:
	HTMLShowApp(){
		iPlus::GTKWindowCore::ExitFunctionMap func = std::bind(&HTMLShowApp::Destroy,this);
		browser_exit_ = std::bind(&HTMLMessageHandler::Destroy,this);
		browser_window_.GTKNewTopLevelWindow(func);
	}
	DECLARE_EMPTY_DESTRUCTOR(HTMLShowApp);
	DISALLOW_EVIL_CONSTRUCTORS(HTMLShowApp);
	static HTMLShowApp* Instance(){
		static CefRefPtr<HTMLShowApp> g_handler;
		if(g_handler.get()==NULL){
			g_handler = new HTMLShowApp();
			assert(g_handler.get()!=NULL);
		}
		return g_handler.get();
	}
	void Destroy(void){
		if(browser_exit_)
			browser_exit_();
	}
	void CEFWebViewWindow(){
		browser_window_.GTKWindowLayout("ITUNES PLUS",1152,864);
	}
	void CEFWebViewMessageLoop(){
		browser_window_.GTKPendWindowEvent();
		CefRunMessageLoop();
		CefShutdown();
	}
	void OnTitleChange(CefRefPtr<CefBrowser> browser,
		const CefString& title){
			if(title.length()){
				std::string titleStr(title);
				gtk_window_set_title(GTK_WINDOW(browser_window_.window()), titleStr.c_str());
			}
	}
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() OVERRIDE {
		return this;
	}
	virtual void BrowserCreated() override{
		CEF_REQUIRE_UI_THREAD();
		HTMLMessageHandler::BrowserCreated();
	}
	virtual void OnContextInitialized() OVERRIDE {
		CEF_REQUIRE_UI_THREAD();
		BrowserCreated();
		CefWindowInfo window_info;
#if defined(OS_WIN)
		RECT rect = {0};
		browser_window_.GTKGetWindowRect(browser_window_.window(),
			reinterpret_cast<GdkRectangle*>(&rect));
		HWND parent_hwnd = static_cast<HWND>(
			GDK_WINDOW_HWND(browser_window_.window()->window));
		window_info.SetAsChild(parent_hwnd,rect);
#endif
		CefBrowserSettings browser_settings;
		browser_settings.size = sizeof(CefBrowserSettings);
		CefBrowserHost::CreateBrowserSync(window_info,
			this,
			iPlus::xp_cookie,
			browser_settings,
			NULL);
	}
private:
	iPlus::GTKWindowCore browser_window_;
	iPlus::GTKWindowCore::ExitFunctionMap browser_exit_;
	IMPLEMENT_REFCOUNTING(HTMLShowApp);
};
#if defined(OS_WIN)
int APIENTRY wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPWSTR lpCmdLine,int nShowCmd){
	int argc = 1;
	char* argv[] = {reinterpret_cast<char*>(GetCommandLineA()),NULL};
	CefMainArgs main_args(hInstance);
	OleInitialize(NULL);
#else
int main(int argc,char* argv[]){
	CefMainArgs main_args;
	main_args.argc = argc;
	main_args.argv = argv;
#endif
	void* sandbox_info = NULL;
	gtk_init(&argc,reinterpret_cast<char***>(&argv));
	int exitCode = CefExecuteProcess(main_args,HTMLShowApp::Instance(),sandbox_info);
	if(exitCode >= 0){
		return exitCode;
	}
	HTMLShowApp::Instance()->CEFWebViewWindow();
	CefSettings settings;
	settings.size = sizeof(CefSettings);
	settings.no_sandbox = true;
	settings.user_agent = iPlus::ITunesState::Instance()->UserAgent();
	CefInitialize(main_args,settings,HTMLShowApp::Instance(),sandbox_info);
	HTMLShowApp::Instance()->CEFWebViewMessageLoop();
	return 0;
}