#include "itunes_plus/gtk_window_core.h"

namespace iPlus{
	GTKWindowCore::ExitFunctionMap gtk_exit_func;
	void* GTKPendEventThread(void* arg){
		for(;;){
			if(gtk_events_pending()){
				gtk_main_iteration();
			}
			Sleep(8);
		}
		return NULL;
	}
	void GTKDestroy(){
 		if(gtk_exit_func)
 			gtk_exit_func();
		gtk_main_quit();
	}
	GTKWindowCore::GTKWindowCore(void):window_(NULL){
		g_thread_init(NULL);
		gdk_threads_init();
		gdk_threads_enter();
	}
	GTKWindowCore::~GTKWindowCore(void){
		gdk_threads_leave();
	}
	void GTKWindowCore::GTKPendWindowEvent(){
		GError* gt_error = NULL;
		g_thread_create(GTKPendEventThread,NULL,true,&gt_error);
	}
	void GTKWindowCore::GTKNewTopLevelWindow(ExitFunctionMap exit){
		window_ = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		g_signal_connect(G_OBJECT(window()),"close",G_CALLBACK(GTKDestroy),NULL);
		g_signal_connect(G_OBJECT(window()),"destroy",G_CALLBACK(GTKDestroy),NULL);
		gtk_exit_func = exit;
	}
	void GTKWindowCore::GTKWindowLayout(gchar* title,int width,int height){
		gtk_window_set_title(GTK_WINDOW(window()),title);
		gtk_window_set_resizable(GTK_WINDOW(window()),FALSE);
		gtk_widget_set_usize((GtkWidget*)window(),width,height);
		gtk_window_set_position(GTK_WINDOW(window()),GTK_WIN_POS_CENTER);
		gtk_widget_show_all((GtkWidget*)window());
	}
	void GTKWindowCore::GTKGetWindowRect(const GtkWidget* window,GdkRectangle* rect){
		int x, y, a, b;
		gdk_window_get_position(GDK_WINDOW(window),(int*)&rect->x,(int*)&rect->y);
		gtk_window_get_position(GTK_WINDOW(window),&a,&b);
		gtk_window_get_size(GTK_WINDOW(window),&x,&y);
		rect->width = rect->x + x;
		rect->height = rect->y + y;
	}
	const GtkWidget* GTKWindowCore::window() const{
		return window_;
	}
}