#include "itunes_plus/itunes_dom_node_visitor.h"
#include <glib/gthread.h>
#include "itunes_plus/itunes_artificial_event.h"

namespace iPlus{
	void* ClickAccountInformationNextStep(void* arg){
		Sleep(3000);
		keybd_event(VK_RETURN,0,0,0);
		keybd_event(VK_RETURN,0,KEYEVENTF_KEYUP,0);
		return NULL;
	}
	iTunesDOMNodeVisitor::iTunesDOMNodeVisitor(void){
		set_conn_services_key("");
		set_machine_guid("");
		set_x_apple_action_signature("");
	}
	iTunesDOMNodeVisitor::~iTunesDOMNodeVisitor(void){
		set_conn_services_key("");
		set_machine_guid("");
		set_x_apple_action_signature("");
	}
	void iTunesDOMNodeVisitor::set_conn_services_key(const CefString& key){
		conn_services_key_ = key;
	}
	void iTunesDOMNodeVisitor::set_machine_guid(const CefString& machine_guid){
		machine_guid_ = machine_guid;
	}
	void iTunesDOMNodeVisitor::Visit(CefRefPtr<CefDOMDocument> document) {
		CEF_REQUIRE_RENDERER_THREAD();
		if(document==nullptr){
			return;
		}
		CefRefPtr<CefDOMNode> node = document->GetDocument();
		if(node!=NULL){
			node = document->GetElementById("machineGUID");
			if(node&&machine_guid_.length()){
				node->SetElementAttribute("value",machine_guid_);
				OutputDebugStringW(machine_guid_.c_str());
			}
			node = document->GetElementById("pageUUID");
			if(node){
				iPlus::iTunesCalcServices conn;
				conn.set_key(conn_services_key_);
				const std::string page_uuid = node->GetElementAttribute("value");
				if(!page_uuid.empty()){
					const std::string machine_guid = machine_guid_;
					const std::string page_info = page_uuid + machine_guid+[document,this](const bool is_calc_mail) -> std::string {
						if(is_calc_mail){
							WrittenAccountInformation(document);
							CefRefPtr<CefDOMNode> email = document->GetElementById("emailAddress");
							return std::string(email->GetElementAttribute("value"));
						}
						else{
							/*if(IsStayBillingInformation(document,"cc_number")){
								const CefString js_code = "document.getElementsByClassName('payease')[0].click();";
								const CefString url = ItunesWebBrowser::get_browser()->GetMainFrame()->GetURL();
								ItunesWebBrowser::get_browser()->GetMainFrame()->ExecuteJavaScript(js_code,url,0);
							}
							else */if(IsStayBillingInformation(document)){
								WrittenBillingInformation(document);
							}
							else if(IsStayAcceptTermsProtocol(document)){
								WrittenAcceptTermsProtocol(document);
							}
							return std::string("");
						}
					}(IsStayAccountInformation(document));
					const std::string sig_info = conn.XAppleActionSignature_3(page_info.c_str(),page_info.length());
					set_x_apple_action_signature(sig_info.c_str());
					OutputDebugStringA(page_info.c_str());
					OutputDebugStringA(page_uuid.c_str());
				}		
				node = document->GetElementById("signature");
				if(node&&x_apple_action_signature_.length()){
					node->SetElementAttribute("value",x_apple_action_signature_);
					OutputDebugStringW(x_apple_action_signature_.c_str());		
				}
				/*if(IsStayAccountInformation(document)){
					const CefString js_code = 
						"document.getElementById('newsletter').click();\r\n"
						"document.getElementById('marketing').click();";
					const CefString url = ItunesWebBrowser::get_browser()->GetMainFrame()->GetURL();
					ItunesWebBrowser::get_browser()->GetMainFrame()->ExecuteJavaScript(js_code,url,0);
					GError* gt_error = NULL;
					g_thread_create(ClickAccountInformationNextStep,NULL,true,&gt_error);
				}
				else if(IsStayBillingInformation(document)){
					const CefString js_code = "document.getElementsByClassName('continue')[0].click();";
					const CefString url = ItunesWebBrowser::get_browser()->GetMainFrame()->GetURL();
					ItunesWebBrowser::get_browser()->GetMainFrame()->ExecuteJavaScript(js_code,url,0);
				}*/
			}
			else{
				/*const CefString js_code = "document.getElementsByClassName('emphasized')[0].click();";
				const CefString url = ItunesWebBrowser::get_browser()->GetMainFrame()->GetURL();
				ItunesWebBrowser::get_browser()->GetMainFrame()->ExecuteJavaScript(js_code,url,0);*/
			}
		}
	}
	bool iTunesDOMNodeVisitor::IsStayAcceptTermsProtocol(CefRefPtr<CefDOMDocument> document){
		CefRefPtr<CefDOMNode> node = document->GetElementById("iagree");
		return (node!=nullptr);
	}
	bool iTunesDOMNodeVisitor::WrittenAcceptTermsProtocol(CefRefPtr<CefDOMDocument> document){
		keybd_event(VK_TAB,0,0,0);
		keybd_event(VK_TAB,0,KEYEVENTF_KEYUP,0);
		keybd_event(VK_TAB,0,0,0);
		keybd_event(VK_TAB,0,KEYEVENTF_KEYUP,0);
		/*const CefString js_code = "document.getElementById('iagree').click();\r\ndocument.getElementsByClassName('emphasized')[0].click();";
		const CefString url = ItunesWebBrowser::get_browser()->GetMainFrame()->GetURL();
		ItunesWebBrowser::get_browser()->GetMainFrame()->ExecuteJavaScript(js_code,url,0);*/
		return true;
	}
	bool iTunesDOMNodeVisitor::WrittenAccountInformation(CefRefPtr<CefDOMDocument> document){
		const std::string registed_email = "sdfsfsdfffsfs@163.com";
		const std::string password = "App1234567";
		CefRefPtr<CefDOMNode> email = document->GetElementById("emailAddress");
		email->SetElementAttribute("value",registed_email);
		keybd_event(VK_TAB,0,0,0);
		keybd_event(VK_TAB,0,KEYEVENTF_KEYUP,0);
		keybd_event(VK_TAB,0,0,0);
		keybd_event(VK_TAB,0,KEYEVENTF_KEYUP,0);
		srand((uint32)time(nullptr));
		for(uint32 i=0;i<password.length();i++){
			if(i==1||i==2){
				keybd_event(VK_SHIFT,0,0,0);
				keybd_event(toupper(password[i]),0,0,0);
				keybd_event(toupper(password[i]),0,KEYEVENTF_KEYUP,0);
				keybd_event(VK_SHIFT,0,KEYEVENTF_KEYUP,0);
			}
			else{
				keybd_event(password[i],0,0,0);
				keybd_event(password[i],0,KEYEVENTF_KEYUP,0);
			}
			Sleep(rand()%200);
		}
		CefRefPtr<CefDOMNode> pass1 = document->GetElementById("pass1");
		pass1->SetElementAttribute("value",password);
		keybd_event(VK_RETURN,0,0,0);
		keybd_event(VK_RETURN,0,KEYEVENTF_KEYUP,0);
		keybd_event(VK_TAB,0,0,0);
		keybd_event(VK_TAB,0,KEYEVENTF_KEYUP,0);
		srand((uint32)time(nullptr));
		for(uint32 i=0;i<password.length();i++){
			if(i==1||i==2){
				keybd_event(VK_SHIFT,0,0,0);
				keybd_event(toupper(password[i]),0,0,0);
				keybd_event(toupper(password[i]),0,KEYEVENTF_KEYUP,0);
				keybd_event(VK_SHIFT,0,KEYEVENTF_KEYUP,0);
			}
			else{
				keybd_event(password[i],0,0,0);
				keybd_event(password[i],0,KEYEVENTF_KEYUP,0);
			}
			Sleep(rand()%200);
		}
		CefRefPtr<CefDOMNode> pass2 = document->GetElementById("pass2");
		pass2->SetElementAttribute("value",password);
		CefRefPtr<CefDOMNode> question1 = document->GetElementById("question1");
		question1->GetLastChild()->SetElementAttribute("selected","selected");
		CefRefPtr<CefDOMNode> question2 = document->GetElementById("question2");
		question2->GetLastChild()->SetElementAttribute("selected","selected");
		CefRefPtr<CefDOMNode> question3 = document->GetElementById("question3");
		question3->GetLastChild()->SetElementAttribute("selected","selected");
		CefRefPtr<CefDOMNode> answer1 = document->GetElementById("answer1");
		answer1->SetElementAttribute("value","12345");
		CefRefPtr<CefDOMNode> answer2 = document->GetElementById("answer2");
		answer2->SetElementAttribute("value","123456");
		CefRefPtr<CefDOMNode> answer3 = document->GetElementById("answer3");
		answer3->SetElementAttribute("value","1234567");
		CefRefPtr<CefDOMNode> birthYear = document->GetElementById("birthYear");
		birthYear->SetElementAttribute("value","1992");
		CefRefPtr<CefDOMNode> birthMonthPopup = document->GetElementById("birthMonthPopup");
		birthMonthPopup->GetLastChild()->SetElementAttribute("selected","selected");
		CefRefPtr<CefDOMNode> birthDayPopup = document->GetElementById("birthDayPopup");
		birthDayPopup->GetLastChild()->SetElementAttribute("selected","selected");
		return true;
	}
	bool iTunesDOMNodeVisitor::IsStayAccountInformation(CefRefPtr<CefDOMDocument> document){
		CefRefPtr<CefDOMNode> node = document->GetElementById("emailAddress");
		return (node!=nullptr);
	}
	bool iTunesDOMNodeVisitor::WrittenBillingInformation(CefRefPtr<CefDOMDocument> document){
		CefRefPtr<CefDOMNode> lastFirstName = document->GetElementById("lastFirstName");
		lastFirstName->SetElementAttribute("value","dfgdsdss");
		CefRefPtr<CefDOMNode> firstName = document->GetElementById("firstName");
		firstName->SetElementAttribute("value","dfgdgasdasdfs");
		CefRefPtr<CefDOMNode> street1 = document->GetElementById("street1");
		street1->SetElementAttribute("value","dfgdasdagsds");
		CefRefPtr<CefDOMNode> city = document->GetElementById("city");
		city->SetElementAttribute("value","sdfssdfsfssd");
		CefRefPtr<CefDOMNode> postalcode = document->GetElementById("postalcode");
		postalcode->SetElementAttribute("value","761384");
		CefRefPtr<CefDOMNode> phone1Number = document->GetElementById("phone1Number");
		phone1Number->SetElementAttribute("value","18323354313");
		return true;
	}
	bool iTunesDOMNodeVisitor::IsStayBillingInformation(CefRefPtr<CefDOMDocument> document,const CefString name){
		if(name.length()){
			CefRefPtr<CefDOMNode> node = document->GetElementById(name);
			return (node!=nullptr);
		}
		else{
			CefRefPtr<CefDOMNode> node = document->GetElementById("cc_payease");
			return (node!=nullptr);
		}
	}
}