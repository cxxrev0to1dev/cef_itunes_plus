#ifndef ITUNES_PLUS_WINDOW_CORE_H_
#define ITUNES_PLUS_WINDOW_CORE_H_

#include <gtk/gtk.h>
#include <glib/gthread.h>
#if defined(OS_WIN)
#include <gdk/gdkwin32.h>
#endif
#include <functional>
#include "cpp_support.h"

namespace iPlus{
	class GTKWindowCore
	{
	public:
		typedef std::function<void (void)> ExitFunctionMap;
		GTKWindowCore(void);
		virtual ~GTKWindowCore(void);
		void GTKPendWindowEvent();
		void GTKNewTopLevelWindow(ExitFunctionMap exit);
		void GTKWindowLayout(gchar* title,int width,int height);
		void GTKGetWindowRect(const GtkWidget* window,GdkRectangle* rect);
		const GtkWidget* window() const;
	private:
		DISALLOW_EVIL_CONSTRUCTORS(GTKWindowCore);
		GtkWidget* window_;
	};
}

#endif